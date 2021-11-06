#include "include/interfaces.h"
#include <stdio.h>
#include <string.h> 
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

//### READ FILES ###//
int getConfig(struct Configuration *cfg){

	int i, n, n_sens, n_acts;
	char *token, **config_txt, aux[MAX_ROOMS][MAX_WORD_SZ];

	n = 0;
	config_txt = (char**) malloc (sizeof(char*));
	config_txt[n] = (char*) malloc(MAX_MSG_SIZE*sizeof(char));

	FILE *configFile = fopen("files/SensorConfigurations.txt", "r");
		if (configFile==NULL) {fputs ("File Config Error\n",stderr); exit (1);}
	
	for(int j=0; j<MAX_ROOMS; j++){
		for (int i = 0; i < MAX_SENSORS; i++) cfg[j].sensors[i][0] = '\0';
		for (int i = 0; i < MAX_ACTUATORS; i++) cfg[j].actuators[i][0] = '\0';
		cfg[j].total_act = 0;
		cfg[j].total_sens = 0;
	}

	while (fgets(config_txt[n],MAX_MSG_SIZE,configFile) != NULL)
	{	
		token = strtok(config_txt[n], ":");
		i=0;
		while(token != NULL) {
			strcpy(aux[i], token);
			token = strtok(NULL, ":");
			i++;
		}
		strcpy(aux[i-1],strtok(aux[i-1],"\n")); // remove \n
		strcpy(cfg[n].room, aux[0]);

		n_sens=0;
		token = strtok(aux[1], ",");
		while(token != NULL ) {

			strcpy(cfg[n].sensors[n_sens], token);
			cfg[n].total_sens++;
	
			token = strtok(NULL, ",");
			n_sens++;
		}

		n_acts=0;
		token = strtok(aux[2], ",");
		while(token != NULL) {

			strcpy(cfg[n].actuators[n_acts], token);
			cfg[n].total_act++;
	
			token = strtok(NULL, ",");
			n_acts++;
		}
		//printf("ROOM:%s SENS:%s %s %s ACT:%s %s %s %d\n", cfg[n].room, cfg[n].sensor1,cfg[n].sensor2, cfg[n].sensor3, cfg[n].actuator1, cfg[n].actuator2, cfg[n].actuator3, cfg[n].total_act);
		n++;
		config_txt = (char**) realloc(config_txt, (n+1)*sizeof(char*));
		config_txt[n] = (char*) malloc(MAX_MSG_SIZE*sizeof(char));
	}

	for(i=0; i<n; i++){
		free(config_txt[i]);
	}
	free(config_txt);

	fclose(configFile);

	return n;
}

int getRules(struct Configuration *cfg, struct ROOM_Rules *set, int rooms){

	int n, i, room_num, rules_num, sens;
	char *token, rules_msg[MAX_RULES][MAX_MSG_SIZE], aux[5][MAX_MSG_SIZE];
	struct Rules rules_aux; 

	FILE *rulesFile = fopen("files/SensorRules.txt", "r");
		if (rulesFile==NULL) {fputs ("File Rules Error\n",stderr); exit (1);}

	for(int j=0; j<MAX_ROOMS; j++) set[j].total_rules = 0;
	
	n = 0;
	while (fgets(rules_msg[n],MAX_MSG_SIZE,rulesFile) != NULL)
	{
		// Inicializações
		strcpy(rules_aux.sensor2, "NONE");
		strcpy(rules_aux.actuator1, "NONE");	
		strcpy(rules_aux.actuator2, "NONE");
		rules_aux.logic = 'X';
		rules_aux.signal2 = 'X';
		rules_aux.value2 = -1;
		rules_aux.state2 = -1;

		token = strtok(rules_msg[n], ":");

		room_num = getRoom(cfg, rooms, token);

		set[room_num].total_rules++;

		rules_num = set[room_num].total_rules -1;

		token = strtok(NULL, " ");

		i=0;
		while(token != NULL) {
			strcpy(aux[i], token);
			token = strtok(NULL, " ");
			i++;
		}
		strcpy(aux[i-1],strtok(aux[i-1],"\n")); // remove \n

		sens=1;
		for(int j = 0; j<i; j++){

			//Logic?
			if (!strcmp(aux[j], "AND")) {	
				rules_aux.logic = 'A';
			}
			else if (!strcmp(aux[j], "OR")) {
				rules_aux.logic = 'O';
			}
			//Sensor Threshold?
			else if (!strstr(aux[j], "OFF") && !strstr(aux[j], "ON")) {
				getThresholds(cfg, &rules_aux, room_num, aux[j], sens);
				sens ++; 
			}
			//Actuator?
			else getActuatores(cfg, &rules_aux, room_num, aux[j]);		
		}
		
		strcpy(set[room_num].rule[rules_num].sensor1, rules_aux.sensor1);
		strcpy(set[room_num].rule[rules_num].sensor2, rules_aux.sensor2);
		strcpy(set[room_num].rule[rules_num].actuator1, rules_aux.actuator1);
		strcpy(set[room_num].rule[rules_num].actuator2, rules_aux.actuator2);
		set[room_num].rule[rules_num].signal1=rules_aux.signal1;
		set[room_num].rule[rules_num].signal2=rules_aux.signal2;
		set[room_num].rule[rules_num].logic=rules_aux.logic;
		set[room_num].rule[rules_num].value1=rules_aux.value1;
		set[room_num].rule[rules_num].value2=rules_aux.value2;
		set[room_num].rule[rules_num].state1=rules_aux.state1;
		set[room_num].rule[rules_num].state2=rules_aux.state2;

		//printf("ROOM:%d #RULE:%d ", room_num, rules_num);
		//printf("%s %c %.0f\t%c\t%s %c %.0f\t\t%s:%d,%s:%d\n",rules_aux.sensor1, rules_aux.signal1, rules_aux.value1, rules_aux.logic, rules_aux.sensor2, rules_aux.signal2, rules_aux.value2, rules_aux.actuator1, rules_aux.state1, rules_aux.actuator2, rules_aux.state2);
		n++;
	}
	fclose(rulesFile);
	return n;
}

//### COMPONENTS PARTITION ###//

int getRoom (struct Configuration *cfg, int rooms, char *compare){

	int room_number;

	for(room_number=0; room_number<rooms; room_number++){
		if(!strcmp(compare, cfg[room_number].room)) return room_number;
	}
	return 0;
}

float getSensorVal (char *settings, struct Measurements *sensors, struct Configuration *cfg, int rooms){

	for(int cfg_number=0; cfg_number<rooms; cfg_number++){

		for(int k=0; k<cfg[cfg_number].total_sens; k++){

			if( cfg[cfg_number].sensors[k][0]=='T' && !strcmp(settings, cfg[cfg_number].sensors[k])){
				//printf("TEMP(%d): %f\n",cfg_number, sensors[cfg_number].temperature);
			return sensors[cfg_number].temperature;
			}
			else if( cfg[cfg_number].sensors[k][0]=='L' && !strcmp(settings, cfg[cfg_number].sensors[k])){
				//printf("LIGHT(%d): %f\n",cfg_number, sensors[cfg_number].light);
				return sensors[cfg_number].light;
			}
			else if( cfg[cfg_number].sensors[k][0]=='H' && !strcmp(settings, cfg[cfg_number].sensors[k])){
				//printf("HUM(%d): %f\n",cfg_number, sensors[cfg_number].humidity);
				return sensors[cfg_number].humidity;
			}
		}
	}
	return 0;
}

void getActuatores (struct Configuration *cfg, struct Rules *rule, int room, char *str) {
	
	char *token, *token1, *token2, aux[2][MAX_WORD_SZ]={"\0"};
	int i=0;
	//printf("str: %s\n", str);

	token = strtok(str, ",");
	while(token != NULL) {
		strcpy(aux[i], token);
		//printf("str: %s\n", aux[i]);
		token = strtok(NULL, "\n");
		i++;
	}
	//printf("%s\n", aux[0]);
	token1 = strtok(aux[0], ":");
	strcpy(rule->actuator1, token1);

	token1 = strtok(NULL, "\n");
	if (!strcmp(token1, "ON")) rule->state1 = 1;
	else if (!strcmp(token1, "OFF")) rule->state1 = 0;
	//printf("AC1:%s ST1:%d\n",rule->actuator1, rule->state1);

	if(i>1){
		token2 = strtok(aux[1], ":");
		strcpy(rule->actuator2,token2);
		token2 = strtok(NULL, "\n");
		if (!strcmp(token2, "ON")) rule->state2 = 1;
		else if (!strcmp(token1, "OFF")) rule->state2 = 0;
	}
	//printf("AC2:%s ST2:%d\n",rule->actuator2, rule->state2);
}

void getThresholds (struct Configuration *cfg, struct Rules *rule, int room, char *str, int section){

	char *token;
	int str_size = strlen(str);

	if(section==1){
		token = strtok(str, ">");
		if(strlen(token)!=str_size){
			rule->signal1='>';
		}
		else
		{
			token = strtok(str, "<");
			if(strlen(token)!=str_size){
				rule->signal1='<';
			}
			else
			{
				token = strtok(str,"=");
				if(strlen(token)!=str_size) {
					rule->signal1='=';
				}
			}
		}
		strcpy(rule->sensor1,token);
		token = strtok(NULL, "<=>");
		rule->value1 = atof(token);
	}
	
	else if (section==2){
		token = strtok(str, ">");
		if(strlen(token)!=str_size){
			rule->signal2='>';
		}
		else
		{
			token = strtok(str, "<");
			if(strlen(token)!=str_size){
				rule->signal2='<';
			}
			else
			{
				token = strtok(str,"=");
				if(strlen(token)!=str_size) {
					rule->signal2='=';
				}
			}
		}
		strcpy(rule->sensor2, token);
		token = strtok(NULL, "<=>");
		rule->value2 = atof(token);		
	}
}

//### MAIN FUNCTIONS ###//

void readSensors(struct Measurements *sensors, int n_rooms, char* received_msg){
	
    int i, mote_id; 
	char *token, sense[23][3];
	
	/* get the first token */
	token = strtok(received_msg, " ");
	/* walk through other tokens */
	i=0;
	while( token != NULL && i<23) {
		strcpy(sense[i], token);
		token = strtok(NULL, " ");
		i++;
	}			
	mote_id = (int)byte2long(sense[5], sense[6]);
	//printf("%d\n", mote_id);

	if(mote_id <= n_rooms && mote_id>0){
		sensors[mote_id-1].mote_id 		= mote_id; 
		sensors[mote_id-1].light 		= value2light	( byte2long(sense[12], sense[13]) );
		sensors[mote_id-1].temperature 	= value2temp	( byte2long(sense[16], sense[17]) );
		sensors[mote_id-1].humidity 	= value2hum		( byte2long(sense[18], sense[19]) );
		//sensors[mote_id-1].current 		= value2curr	( byte2long(sense[14], sense[15]) );
		//sensors[mote_id-1].voltage 		= value2voltage	( byte2long(sense[10], sense[11]) );
	}

}

void checkRules(struct Configuration *cfg, struct ROOM_Rules *settings, struct Measurements *sensors, struct Actuators *actuators, int rooms){

	for(int i=0; i<rooms; i++){	// RUNS ALL ROOMS (i)

		for(int j=0; j<settings[i].total_rules; j++){ 	// RUNS ALL RULES (j)

			if(settings[i].rule[j].logic=='A'){			// LOGIC AND
				if(settings[i].rule[j].signal1=='>'){
					if(settings[i].rule[j].signal2=='>'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) > settings[i].rule[j].value1 && getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) > settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}		
					}
					else if(settings[i].rule[j].signal2=='<'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) > settings[i].rule[j].value1 && getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) < settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}
					}	
				}
				else if(settings[i].rule[j].signal1=='<'){
					if(settings[i].rule[j].signal2=='>'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) < settings[i].rule[j].value1 && getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) > settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}						
					}
					else if(settings[i].rule[j].signal2=='<'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) < settings[i].rule[j].value1 && getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) < settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}						
					}
				}
			}
			else if(settings[i].rule[j].logic=='O'){	// LOGIC OR
				if(settings[i].rule[j].signal1=='>'){
					if(settings[i].rule[j].signal2=='>'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) > settings[i].rule[j].value1 || getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) > settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}								
					}
					else if(settings[i].rule[j].signal2=='<'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) > settings[i].rule[j].value1 || getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) < settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}						
					}
				}

				else if(settings[i].rule[j].signal1=='<'){
					if(settings[i].rule[j].signal2=='>'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) < settings[i].rule[j].value1 || getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) > settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}		
					}
					else if(settings[i].rule[j].signal2=='<'){
						if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) < settings[i].rule[j].value1 || getSensorVal(settings[i].rule[j].sensor2, sensors, cfg, rooms) < settings[i].rule[j].value2){

							if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
							else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
							else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
							else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
							else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

							if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
							else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
						}
					}
				}
			}
			else{										// NO LOGIC
				if(settings[i].rule[j].signal1=='>'){
					//printf("%s(%.0f) > (%.0f) \n",settings[i].rule[j].sensor1, getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms), settings[i].rule[j].value1);
					if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) > settings[i].rule[j].value1){
						
						if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
						else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
						else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
						else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
						else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

						if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
					}					
				}
				else if(settings[i].rule[j].signal1=='<'){
					//printf("%s(%.0f) < (%.0f) \n",settings[i].rule[j].sensor1, getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms), settings[i].rule[j].value1);
					if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) < settings[i].rule[j].value1){

						if		(settings[i].rule[j].actuator1[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state1; 			// check HEAT
						else if (settings[i].rule[j].actuator1[0] == 'L') actuators[i].light 	= settings[i].rule[j].state1;			// check LIGHTS
						else if (settings[i].rule[j].actuator1[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state1;			// check DEHUMIDIFIER
						else if (settings[i].rule[j].actuator1[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state1;			// check BLIDS
						else if (settings[i].rule[j].actuator1[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state1;

						if 		(settings[i].rule[j].actuator2[0] == 'H') actuators[i].heater 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'L') actuators[i].light 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'D') actuators[i].dehum 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'B') actuators[i].blinds 	= settings[i].rule[j].state2;
						else if (settings[i].rule[j].actuator2[0] == 'S') actuators[i].sprink 	= settings[i].rule[j].state2;
					}					
				}
				else if(settings[i].rule[j].signal1=='='){
					if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) < settings[i].rule[j].value1){

						if	(settings[i].rule[j].sensor1[0] == 'T') actuators[i].heater = 1;
						else if	(settings[i].rule[j].sensor1[0] == 'H') actuators[i].dehum = 0; 
											
					}
					else if(getSensorVal(settings[i].rule[j].sensor1, sensors, cfg, rooms) > settings[i].rule[j].value1){

						if	(settings[i].rule[j].sensor1[0] == 'T')	actuators[i].heater = 0;
						else if	(settings[i].rule[j].sensor1[0] == 'H') actuators[i].dehum = 1;
					}
				}
			}
		}
		if(actuators[i].heater == 1) actuators[i].cooler = 0;
		else if(actuators[i].heater == 0) actuators[i].cooler = 1;

		if(actuators[i].dehum == 1) actuators[i].humid = 0;
		else if(actuators[i].dehum == 0) actuators[i].humid = 1;
	}
}

void writeRGBMatrix(struct Actuators *actuators){
	
	char act[NUMBER_CELLS][MAX_WORD_SZ], print_array[512]={"\0"}; 
	int j=0;
	
	while(j < NUMBER_CELLS){

		for(int n=0; n<MAX_ROOMS; n++){
			if(actuators[n].heater==1) strcpy(act[j], GREEN);
			else if(actuators[n].heater==0) strcpy(act[j], RED);
			else strcpy(act[j], BLACK);
			j++;
		
			if(actuators[n].light==1) strcpy(act[j], GREEN);
			else if(actuators[n].light==0) strcpy(act[j], RED);
			else strcpy(act[j], BLACK);
			j++;
		
			if(actuators[n].dehum==1) strcpy(act[j], GREEN);
			else if(actuators[n].dehum==0) strcpy(act[j], RED);
			else strcpy(act[j], BLACK);
			j++;
	
			if(actuators[n].blinds==1) strcpy(act[j], GREEN);
			else if(actuators[n].blinds==0) strcpy(act[j], RED);
			else  strcpy(act[j], BLACK);
			j++;

			if(actuators[n].sprink==1) strcpy(act[j], GREEN);
			else if(actuators[n].sprink==0) strcpy(act[j], RED);
			else  strcpy(act[j], BLACK);
			j++;
		}	
	}
	strcat(print_array, "[");
	for(int i=0; i<NUMBER_CELLS; i++){
		strcat(print_array, act[i]);
		if(i<NUMBER_CELLS-1) strcat(print_array, ",");
	}
	strcat(print_array, "]\n");
	printf("%s", print_array);	
}

void HASController(struct Measurements *sensors, struct Actuators *actuators, int rooms){
	
	int T;	

	// ATENÇÃO : VER DIRETORIO MSGCREATORCONF.TXT
	FILE *msg_file = fopen("/home/vitorpbabo/Desktop/Simulators2/MsgCreatorConf.txt", "w");
		if (msg_file == NULL) {fputs ("Msg Error\n",stderr); exit (1);}
	
	if (actuators[3].heater == 1) T = 1;
	else if (actuators[3].heater == 0) T = -1;
	else T = 0;

	if(T!=0)
	fprintf(msg_file, "-n 1 -l 50 -f 1 -c 1 -s [1,3,4] -d [['U',50.0,500.0,4.5],['L',15.0,35.0,%d],['C',30.0,90.0,5.0]] -i 4", T);
	else
	fprintf(msg_file, "-n 1 -l 50 -f 1 -c 1 -s [1,3,4] -d [['U',50.0,500.0,4.5],['C',15.0,35.0,1.5],['C',30.0,90.0,5.0]] -i 4");

	fclose (msg_file);
	
}

//### Values Conversion ###//

long byte2long(char *byte1, char *byte2){
    
	long ret;
	char *aux, *ptr;

	int len = strlen(byte1)+strlen(byte2);

	aux = (char*) malloc(sizeof(len));

	strcpy(aux, byte1);
	strcat(aux, byte2);
	//printf("STR: %s // ", aux);

	ret = strtol(aux, &ptr, 16);
	//printf(" RAW VALUE: %li\n", ret);	

	free(aux);

	return ret;
}

float value2voltage(long volt){
	return ((float)(volt)/4096.0)*VREF*(2.0);
}

float value2temp(long temp){
    return (-39.6)+(0.01*temp)	;
}

float value2hum(long hum){ 	// Humidade Relativa
    return (-2.0468) +(float)(0.0367*hum) + (float)(-1.5955/(float)1000000)*hum*hum;
}

float value2humcomp(long temp, long hum, long value){ // Humidade Compensada pela Temperatura
    return (temp-25)* (0.01*0.00008*hum)+value;
}

float value2light(long light){
    return (0.625)*((float)light/(float)4096)*(1.5)*10000;
}

float value2curr(long curr){
	return (0.769)*((float)(curr)/4096.0)*(1.5)*(1000.0);	 
}

//######################//