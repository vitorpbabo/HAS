#include "include/interfaces.h"
#include <stdio.h>
#include <string.h> 
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <postgresql/libpq-fe.h>

int getdb_rules(PGconn *conn, struct Configuration *cfg, struct ROOM_Rules *set, int NUM_ROOMS){

	char query[MAX_MSG_SIZE] = "SELECT * FROM rules ORDER BY ruleid ASC";
	int room, row, rule_num;

	PGresult *rules = PQexec(conn, query);
	if (PQresultStatus(rules) != PGRES_TUPLES_OK) puts(PQresultErrorMessage(rules));

	for(int j=0; j<MAX_ROOMS; j++) set[j].total_rules = 0;

	int NUM_RULES = PQntuples(rules);

	if(NUM_RULES == 0 ) printf("DB: Rules Empty\n");

	if (rules != 0) {

		for (row = 0; row < NUM_RULES; row++){

			for(room = 0; room < NUM_ROOMS; room++){
				
				if(!strcmp(PQgetvalue(rules, row , 12), cfg[room].room)){

					set[room].total_rules++;
					rule_num = set[room].total_rules -1;

					//MINIMAL OCURRENCES - NOT NULL FIELDS
					strcpy(set[room].rule[rule_num].sensor1, PQgetvalue(rules, row , 1));

					set[room].rule[rule_num].signal1 = PQgetvalue(rules, row , 2)[0];

					set[room].rule[rule_num].value1 = atof(PQgetvalue(rules, row , 3));

					//OTHER OCURRENCES - POSSIBLE NULL FIELDS
					if(!PQgetisnull(rules,row,4)){ //LOGIC
						if		(PQgetvalue(rules, row , 4)[0] == 'A') set[room].rule[rule_num].logic = '&';
						else if (PQgetvalue(rules, row , 4)[0] == 'O') set[room].rule[rule_num].logic = '|';
					}
					else set[room].rule[rule_num].logic = 'X';
				
					if(!PQgetisnull(rules,row,5)) 	strcpy(set[room].rule[rule_num].sensor2, PQgetvalue(rules, row , 5));
					else strcpy(set[room].rule[rule_num].sensor2, "NONE");
		
					if(!PQgetisnull(rules,row,6))	set[room].rule[rule_num].signal2	= PQgetvalue(rules, row , 6)[0];
					else set[room].rule[rule_num].signal2 = 'X';
				
					if(!PQgetisnull(rules,row,7))	set[room].rule[rule_num].value2 = atof(PQgetvalue(rules, row , 7));
					else set[room].rule[rule_num].value2 = -1;
				
					if(!PQgetisnull(rules,row,8))	strcpy(set[room].rule[rule_num].actuator1, PQgetvalue(rules, row , 8));
					else strcpy(set[room].rule[rule_num].actuator1, "NONE");
				
					if(!PQgetisnull(rules,row,9)){
						if		(PQgetvalue(rules, row , 9)[0] == 't') set[room].rule[rule_num].state1 = 1;
						else if (PQgetvalue(rules, row , 9)[0] == 'f') set[room].rule[rule_num].state1 = 0;
					}
					else set[room].rule[rule_num].state1 = -1;
				
					if(!PQgetisnull(rules,row,10))	strcpy(set[room].rule[rule_num].actuator2, PQgetvalue(rules, row , 10));
					else strcpy(set[room].rule[rule_num].actuator2, "NONE");
					
					if(!PQgetisnull(rules,row,11)){
						if		(PQgetvalue(rules, row , 9)[0] == 't') set[room].rule[rule_num].state2 = 1;
						else if (PQgetvalue(rules, row , 9)[0] == 'f') set[room].rule[rule_num].state2 = 0;
					}
					else set[room].rule[rule_num].state2 = -1;
				}	
			}
		}
	}
	return NUM_RULES;
}

int getdb_config(PGconn *conn, struct Configuration *cfg, int *NUM_SENS, int *NUM_ACT) {

	char query[MAX_MSG_SIZE] = "SELECT c.* FROM configuration AS c JOIN room AS r ON r.room_name = c.room_name ORDER BY r.moteid";

	PGresult *config = PQexec(conn, query);
	if (PQresultStatus(config) != PGRES_TUPLES_OK) puts(PQresultErrorMessage(config));

	for(int j=0; j<MAX_ROOMS; j++){
		for (int i = 0; i < MAX_SENSORS; i++) cfg[j].sensors[i][0] = '\0';
		for (int i = 0; i < MAX_ACTUATORS; i++) cfg[j].actuators[i][0] = '\0';
		cfg[j].total_act = 0;
		cfg[j].total_sens = 0;
	}

	int NUM_ROOMS = PQntuples(config);

	if(NUM_ROOMS == 0 ) printf("DB: Config Empty\n");

	if (config != 0) {
		for (int row = 0; row < NUM_ROOMS; row++){
			strcpy(cfg[row].room, PQgetvalue(config, row , 0));
			if(!PQgetisnull(config,row,1)){
				strcpy(cfg[row].sensors[0], PQgetvalue(config, row , 1));
				cfg[row].total_sens++;
				(*NUM_SENS)++;
			}
		
			if(!PQgetisnull(config,row,2)){
				strcpy(cfg[row].sensors[1], PQgetvalue(config, row , 2));
				cfg[row].total_sens++;
				(*NUM_SENS)++;
			}
			
			if(!PQgetisnull(config,row,3)){
				strcpy(cfg[row].sensors[2], PQgetvalue(config, row , 3));
				cfg[row].total_sens++;
				(*NUM_SENS)++;
			}
			
			if(!PQgetisnull(config,row,4)){
				strcpy(cfg[row].actuators[0], PQgetvalue(config, row , 4));
				cfg[row].total_act++;
				(*NUM_ACT)++;
			} 
			
			if(!PQgetisnull(config,row,5)){
				strcpy(cfg[row].actuators[1], PQgetvalue(config, row , 5));
				cfg[row].total_act++;
				(*NUM_ACT)++;
			} 
			
			if(!PQgetisnull(config,row,6)){
				strcpy(cfg[row].actuators[2], PQgetvalue(config, row , 6));
				cfg[row].total_act++;
				(*NUM_ACT)++;
			} 
		}
	}
	return NUM_ROOMS;
}

void db_clean(PGconn *conn){

	PQexec(conn, "DELETE FROM state;");
	PQexec(conn, "DELETE FROM measurements;");
	PQexec(conn, "DELETE FROM rules;");
	PQexec(conn, "DELETE FROM configuration;");
	PQexec(conn, "DELETE FROM sensor;");
	PQexec(conn, "DELETE FROM actuator;");
	PQexec(conn, "DELETE FROM room;");

}

void insert_room(PGconn *conn, struct Configuration *cfg, int NUM_ROOMS){

    char query[MAX_MSG_SIZE];
	//PQexec(conn, "TRUNCATE Table room CASCADE");
		
	for (int r = 0; r < NUM_ROOMS; r++){
		sprintf(query, "INSERT INTO room (room_name, moteid) VALUES ('%s', %d);", cfg[r].room, (r+1));
		//printf("%s\n", query);
		PQexec(conn, query);
		
	}
}

void insert_config (PGconn *conn, struct Configuration *cfg, int NUM_ROOMS){
	char query[300], aux[300];

	//PQexec (conn, "TRUNCATE Table configuration CASCADE");
	for (int r=0; r<NUM_ROOMS; r++){
		strcpy(query, "INSERT INTO Configuration (room_name, sensor1,");

		if(cfg[r].sensors[1][0]=='\0'){ 				// 1 SENSOR
			strcat(query," actuator1");
			if(cfg[r].actuators[1][0]=='\0'){ 			// 1 ATUADOR
				sprintf(aux, ") VALUES ('%s', '%s', '%s');",cfg[r].room, cfg[r].sensors[0],cfg[r].actuators[0]);
				strcat(query, aux);
			} 
			else if (cfg[r].actuators[2][0]=='\0'){		// 2 ATUADORES
				sprintf(aux, ", actuator2) VALUES ('%s', '%s', '%s', '%s');",cfg[r].room, cfg[r].sensors[0], cfg[r].actuators[0], cfg[r].actuators[1]);
				strcat(query, aux);
			}
			else{										// 3 ATUADORES
				sprintf(aux, ", actuator2, actuator3) VALUES ('%s', '%s', '%s', '%s', '%s');",cfg[r].room, cfg[r].sensors[0], cfg[r].actuators[0], cfg[r].actuators[1], cfg[r].actuators[2]);
				strcat(query, aux);
			}  
		}
		else if (cfg[r].sensors[2][0]=='\0'){ 			// 2 SENSORES
			strcat(query," sensor2, actuator1");
			if(cfg[r].actuators[1][0]=='\0'){			// 1 ATUADOR
				sprintf(aux, ") VALUES ('%s', '%s', '%s', '%s')",cfg[r].room, cfg[r].sensors[0], cfg[r].sensors[1], cfg[r].actuators[0]);
				strcat(query, aux);
			} 
			else if (cfg[r].actuators[2][0]=='\0'){		// 2 ATUADORES
				sprintf(aux, ", actuator2) VALUES ('%s', '%s', '%s', '%s', '%s')",cfg[r].room, cfg[r].sensors[0], cfg[r].sensors[1], cfg[r].actuators[0], cfg[r].actuators[1]);
				strcat(query, aux);
			}
			else{										// 3 ATUADORES
				sprintf(aux, ", actuator2, actuator3) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')",cfg[r].room, cfg[r].sensors[0], cfg[r].sensors[1], cfg[r].actuators[0],cfg[r].actuators[1],cfg[r].actuators[2]);
				strcat(query, aux);
			}  
		}
		else{ 											// 3 SENSORES
			strcat(query," sensor2, sensor3, actuator1");
			if(cfg[r].actuators[1][0]=='\0'){			// 1 ATUADOR
				sprintf(aux, ") VALUES ('%s', '%s', '%s', '%s', '%s')",cfg[r].room, cfg[r].sensors[0], cfg[r].sensors[1], cfg[r].sensors[2], cfg[r].actuators[0]);
				strcat(query, aux);
			} 
			else if (cfg[r].actuators[2][0]=='\0'){		// 2 ATUADORES
				sprintf(aux, ", actuator2) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')",cfg[r].room, cfg[r].sensors[0], cfg[r].sensors[1], cfg[r].sensors[2], cfg[r].actuators[0], cfg[r].actuators[1]);
				strcat(query, aux);
			}
			else{										// 3 ATUADORES
				sprintf(aux, ", actuator2, actuator3) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s')",cfg[r].room, cfg[r].sensors[0], cfg[r].sensors[1], cfg[r].sensors[2], cfg[r].actuators[0], cfg[r].actuators[1],cfg[r].actuators[2]);
				strcat(query, aux);
			}  
		}
		//printf("%s\n", query);
		PQexec (conn, query);
   }
}

void insert_sensor(PGconn *conn, struct Configuration *cfg, int NUM_ROOMS, int NUM_SENS){
   
    char query[MAX_MSG_SIZE];

	//PQexec(conn, "TRUNCATE Table sensor CASCADE");
		
	for (int r = 0; r < NUM_ROOMS; r++){

		for (int s = 0; s < cfg[r].total_sens; s++){

			sprintf(query, "INSERT INTO sensor (sensorid, room_name) VALUES ('%s', '%s');", cfg[r].sensors[s], cfg[r].room);
			//printf("%s\n", query);
			PQexec(conn, query);
		}
	}
	
}

void insert_actuator(PGconn *conn, struct Configuration *cfg, int NUM_ROOMS, int NUM_ACT){
   
    char query[MAX_MSG_SIZE];

	//PQexec(conn, "TRUNCATE Table actuator CASCADE");
			
	for (int r = 0; r < NUM_ROOMS; r++){

		for (int a = 0; a < cfg[r].total_act; a++)
		{
			sprintf(query, "INSERT INTO actuator (actuatorid, room_name) VALUES ('%s', '%s');", cfg[r].actuators[a], cfg[r].room);
			//printf("%s\n", query);
			PQexec(conn, query); 
		}
	}
}

void insert_rules(PGconn *conn,struct Configuration *cfg, struct ROOM_Rules *set, int NUM_ROOMS, int NUM_RULES){
   
    char query[512];
	int n=0;

	//PQexec(conn, "TRUNCATE Table rules CASCADE");

	while (n < NUM_RULES){

		for (int r = 0; r < NUM_ROOMS; r++){

			for (int k = 0; k < set[r].total_rules; k++)
			{
				if (set[r].rule[k].signal1 == '='){
					sprintf(query, "INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1) VALUES (%d, '%s', '%s', '%c', %.2f);", n, cfg[r].room, set[r].rule[k].sensor1, set[r].rule[k].signal1, set[r].rule[k].value1);
					//printf("%s\n", query);
					PQexec(conn, query); 
				}
				else if(set[r].rule[k].logic == 'X'){
					if(set[r].rule[k].actuator2[0] == 'N'){
						sprintf(query, "INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1) VALUES (%d, '%s', '%s', '%c', %.2f, '%s', '%d');", n, cfg[r].room, set[r].rule[k].sensor1, set[r].rule[k].signal1, set[r].rule[k].value1, set[r].rule[k].actuator1, set[r].rule[k].state1);
						//printf("%s\n", query);
						PQexec(conn, query); 
					}
					else{
						sprintf(query, "INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1, actuator2, state2) VALUES (%d, '%s', '%s', '%c', %.2f, '%s', '%d', '%s', '%d');", n, cfg[r].room, set[r].rule[k].sensor1, set[r].rule[k].signal1, set[r].rule[k].value1, set[r].rule[k].actuator1, set[r].rule[k].state1, set[r].rule[k].actuator2, set[r].rule[k].state2);
						//printf("%s\n", query);
						PQexec(conn, query); 
					}
				}else{
					if(set[r].rule[k].actuator2[0] == 'N'){
						sprintf(query, "INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, logic, sensor2, condition2, parameter2, actuator1, state1) VALUES (%d, '%s', '%s', '%c',  %.2f, '%c' ,'%s', '%c', %.2f, '%s', '%d');", n, cfg[r].room, set[r].rule[k].sensor1, set[r].rule[k].signal1, set[r].rule[k].value1, set[r].rule[k].logic, set[r].rule[k].sensor2, set[r].rule[k].signal2, set[r].rule[k].value2, set[r].rule[k].actuator1, set[r].rule[k].state1);
						//printf("%s\n", query);
						PQexec(conn, query); 
					}
					else{
						sprintf(query, "INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, logic, sensor2, condition2, parameter2, actuator1, state1, actuator2, state2) VALUES (%d, '%s', '%s', '%c',  %.2f, '%c' ,'%s', '%c',  %.2f, '%s', '%d', '%s', '%d');", n, cfg[r].room, set[r].rule[k].sensor1, set[r].rule[k].signal1, set[r].rule[k].value1, set[r].rule[k].logic, set[r].rule[k].sensor2, set[r].rule[k].signal2, set[r].rule[k].value2, set[r].rule[k].actuator1, set[r].rule[k].state1, set[r].rule[k].actuator2, set[r].rule[k].state2);
						//printf("%s\n", query);
						PQexec(conn, query); 
					}
				}
				n++;
			}
		}
		
	}	
	
}

void update_measurments(PGconn *conn, struct Configuration *cfg, struct Measurements *sens, int NUM_ROOMS){

	char query[MAX_MSG_SIZE];

	for (int r = 0; r < NUM_ROOMS; r++){

		for(int s = 0; s < cfg[r].total_sens; s++){

			if((cfg[r].sensors[s][0]=='T')){
				sprintf(query, "INSERT INTO measurements (date_time, value, sensorid) VALUES (CURRENT_TIMESTAMP, %f, '%s')", sens[r].temperature , cfg[r].sensors[s]);
				PQexec(conn, query);
				//printf("%s\n", query);
			}
			else if((cfg[r].sensors[s][0]=='L')){
				sprintf(query, "INSERT INTO measurements (date_time, value, sensorid) VALUES (CURRENT_TIMESTAMP, %f, '%s')", sens[r].light , cfg[r].sensors[s]);
				PQexec(conn, query);
				//printf("%s\n", query);
			}
			else if ((cfg[r].sensors[s][0]=='H')){
				sprintf(query, "INSERT INTO measurements (date_time, value, sensorid) VALUES (CURRENT_TIMESTAMP, %f, '%s')", sens[r].humidity , cfg[r].sensors[s]);
				PQexec(conn, query);
				//printf("%s\n", query);
			}
		}
	} 
}

int on_off(char *str){

	if(str[0]=='t') return 1;
	else return 0;
}

void update_state(PGconn *conn, struct Configuration *cfg, struct Actuators *act, int NUM_ROOMS){

	char query[MAX_MSG_SIZE], state_query[MAX_MSG_SIZE];
	int curr_row, prev_row;
	
	PGresult *stateTB;
	
	for (int r = 0; r < NUM_ROOMS; r++)
	{	
		for(int s=0; s < cfg[r].total_act; s++){

			strcpy(state_query,"SELECT * FROM state WHERE actuatorid = '");
			strcat(state_query, cfg[r].actuators[s]);
			strcat(state_query, "'");

			//printf("f_query: %s\n", state_query);
			stateTB = PQexec(conn, state_query);
			state_query[0]='\0';

			curr_row = PQntuples(stateTB);
			prev_row = curr_row-1;
			//printf("CURR ROW: %d\n",curr_row);

			//printf("STATE: %d\n", on_off(PQgetvalue(stateTB, 0, 1)));
				
			if((cfg[r].actuators[s][0]=='H') ){
				
				if((curr_row == 0 || (on_off(PQgetvalue(stateTB, prev_row, 1))!=act[r].heater  && !strcmp(PQgetvalue(stateTB, prev_row, 2), cfg[r].actuators[s]))) ){
					sprintf(query, "INSERT INTO state (date_time, state, actuatorid) VALUES (CURRENT_TIMESTAMP, '%d', '%s')", act[r].heater , cfg[r].actuators[s]);
					PQexec(conn, query);
					printf("%s\n", query);
				}
			}

			else if((cfg[r].actuators[s][0]=='D') ){

				if((curr_row == 0 || (on_off(PQgetvalue(stateTB, prev_row, 1))!=act[r].dehum  && !strcmp(PQgetvalue(stateTB, prev_row, 2), cfg[r].actuators[s]))) ){
					sprintf(query, "INSERT INTO state (date_time, state, actuatorid) VALUES (CURRENT_TIMESTAMP, '%d', '%s')", act[r].dehum , cfg[r].actuators[s]);
					PQexec(conn, query);
					printf("%s\n", query);
				}
			}

			else if ((cfg[r].actuators[s][0]=='B') ){

				if( (curr_row == 0 || (on_off(PQgetvalue(stateTB, prev_row, 1))!=act[r].blinds  && !strcmp(PQgetvalue(stateTB, prev_row, 2), cfg[r].actuators[s]))) ){
					sprintf(query, "INSERT INTO state (date_time, state, actuatorid) VALUES (CURRENT_TIMESTAMP, '%d', '%s')", act[r].blinds , cfg[r].actuators[s]);
					PQexec(conn, query);
					printf("%s\n", query);
				}
			}

			else if((cfg[r].actuators[s][0]=='L')  ){

				if ((curr_row == 0 || (on_off(PQgetvalue(stateTB, prev_row, 1))!=act[r].light  && !strcmp(PQgetvalue(stateTB, prev_row, 2), cfg[r].actuators[s]))) ){
					sprintf(query, "INSERT INTO state (date_time, state, actuatorid) VALUES (CURRENT_TIMESTAMP, '%d', '%s')", act[r].light , cfg[r].actuators[s]);
					PQexec(conn, query);
					printf("%s\n", query);
				}	
			}

			else if ((cfg[r].actuators[s][0]=='S') ){
				if ((curr_row == 0 || (on_off(PQgetvalue(stateTB, prev_row, 1))!=act[r].sprink  && !strcmp(PQgetvalue(stateTB, prev_row, 2), cfg[r].actuators[s]))) ){
					sprintf(query, "INSERT INTO state (date_time, state, actuatorid) VALUES (CURRENT_TIMESTAMP, '%d', '%s')", act[r].sprink , cfg[r].actuators[s]);
					PQexec(conn, query);
					printf("%s\n", query);
				}
			}
		}
	} 
}

int main(){	
	int NUM_ROOMS=0, NUM_SENS=0, NUM_ACT=0, NUM_RULES=0, count=0;
	char received_msg[MAX_MSG_SIZE];

	struct Measurements sensors[MAX_ROOMS];
	struct Actuators actuators[MAX_ROOMS];
	struct Configuration cfg[MAX_ROOMS];
	struct ROOM_Rules settings[MAX_ROOMS];

	PGconn *conn; 
	const char *dbconn;
	dbconn = "host = 'db.fe.up.pt' dbname = 'sinf1920a36' user = 'sinf1920a36' password = 'DHIZTXcf'";

    conn = PQconnectdb(dbconn);
	PQexec(conn,"SET search_path to has2");

	if (!conn){
		printf("libpq error: PQconnectdb returned NULL. \n\n");
		PQfinish(conn);
		exit(1);
	}
	else if (PQstatus(conn) != CONNECTION_OK){
		printf("Connection to DB failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}
    
	FILE *R1 = fopen("/tmp/ttyV10", "r");
		if (R1==NULL) {fputs ("SOCAT1 CHANNEL ERROR!\n",stderr); exit (1);}
	FILE *R2 = fopen("/tmp/ttyV20", "r");
		if (R2==NULL) {fputs ("SOCAT2 CHANNEL ERROR!\n",stderr); exit (1);} 
	
	for(int j=0; j<MAX_ROOMS; j++){
		settings[j].total_rules = 0;
		actuators[j].heater=-1;
		actuators[j].cooler=-1;
		actuators[j].light=-1;
		actuators[j].blinds=-1;
		actuators[j].dehum=-1;
		actuators[j].humid=-1;
		actuators[j].sprink=-1;
		for (int i = 0; i < MAX_SENSORS; i++) cfg[j].sensors[i][0] = '\0';
		for (int i = 0; i < MAX_ACTUATORS; i++) cfg[j].actuators[i][0] = '\0';
		cfg[j].total_act = 0;
		cfg[j].total_sens = 0;
	}

	// ## READ FROM DATABASE ## //
	if(0 /*PRINT DB CONFIG & RULES*/){
		NUM_ROOMS = getdb_config(conn, cfg, &NUM_SENS, &NUM_ACT);
		printf("##DB: CONFIG##\n");
		for(int i=0; i<NUM_ROOMS;i++){
			printf("ROOM: %s: ", cfg[i].room);
			for(int k=0; k<cfg[i].total_sens; k++){
				printf(" %s ", cfg[i].sensors[k]);
			}
			printf(" - ");
			for(int k=0; k<cfg[i].total_act; k++){
				printf(" %s ", cfg[i].actuators[k]);
			}
			printf("\n");
		}
		
		NUM_RULES = getdb_rules(conn, cfg, settings, NUM_ROOMS);
		printf("##RULES##\n");
		int rule=0;
		for(int i=0; i<NUM_ROOMS;i++){
			for (int j=0; j<settings[i].total_rules; j++){
			printf("Room: %c[%d] #RULEid:[%d]",cfg[i].room[0], i+1, rule);
			printf("\t%s%c%.0f \t %c \t %s %c %.0f\t%s:%d,%s:%d\n", settings[i].rule[j].sensor1, settings[i].rule[j].signal1, settings[i].rule[j].value1, settings[i].rule[j].logic, settings[i].rule[j].sensor2, settings[i].rule[j].signal2, settings[i].rule[j].value2, settings[i].rule[j].actuator1, settings[i].rule[j].state1, settings[i].rule[j].actuator2, settings[i].rule[j].state2);
			rule++;
			}
		}
	}

	// ## READ FROM TEXT FILES ## //
	NUM_ROOMS = getConfig(cfg);
	NUM_RULES = getRules(cfg, settings, NUM_ROOMS);

	// ## INSERT INTO DATABASE ## //
	db_clean(conn);
	insert_room(conn, cfg, NUM_ROOMS);
	insert_config(conn, cfg, NUM_ROOMS);
	insert_sensor(conn, cfg, NUM_ROOMS, NUM_SENS);
	insert_actuator(conn, cfg, NUM_ROOMS, NUM_ACT);
	insert_rules(conn, cfg, settings, NUM_ROOMS, NUM_RULES);
	
	while(1){
		
		//READ SENSORS ROUTINE
		if (fgets(received_msg,MAX_MSG_SIZE, R1) != NULL) // 3 MOTES
		readSensors(sensors, NUM_ROOMS, received_msg);

		if (NUM_ROOMS > 3 && fgets(received_msg,MAX_MSG_SIZE, R2) != NULL) // 1 MOTE (CONTROLADA)
		readSensors(sensors, NUM_ROOMS, received_msg);

		checkRules(cfg, settings, sensors, actuators, NUM_ROOMS);
		
		if (count >= 2){
			update_measurments(conn, cfg, sensors, NUM_ROOMS);
			update_state(conn, cfg, actuators, NUM_ROOMS);
			count = 0;
		}
		
		//writeRGBMatrix(actuators);

		//HASController(sensors, actuators, NUM_ROOMS);

		count++;

		if (0){
			for (int i = 0; i<NUM_ROOMS; i++){
				printf("ROOM:[%d] - Light:[%.2f]Lux - Temperature:[%.2f]ºC - Humidity:[%.2f]%% Current:[%.2f]\n\n", sensors[i].mote_id, sensors[i].light, sensors[i].temperature, sensors[i].humidity, sensors[i].current);
				printf("ATUADORES> Heat:%d - Cooler:%d - LIGHT:%d - DEHUMD:%d -HUMID:%d- BLINDS:%d - WATERING:%d\n\n", actuators[i].heater, actuators[i].cooler, actuators[i].light, actuators[i].dehum, actuators[i].humid, actuators[i].blinds, actuators[i].sprink);
			}
		}			
	}

	PQfinish(conn);
	return 0;
}


