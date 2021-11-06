#ifndef _interfaces_h_
#define _interfaces_h_
#endif

#define GREEN 	"[0,254,0]"
#define RED 	"[254,0,0]"
#define BLACK 	"[0,0,0]"

#define MAX_MSG_SIZE 256
#define MAX_WORD_SZ 32
#define MAX_ROOMS 5
#define MAX_SENSORS 3
#define MAX_ACTUATORS 3
#define MAX_RULES 20
#define NUMBER_CELLS 25
#define VREF 1.5

struct Rules{
	char sensor1[MAX_WORD_SZ];
	char sensor2[MAX_WORD_SZ];
	char actuator1[MAX_WORD_SZ];
	char actuator2[MAX_WORD_SZ];
	char signal1;
	char signal2;
	char logic;
	float value1;
	float value2;
	int state1;
	int state2;
};

struct ROOM_Rules{
	struct Rules rule[MAX_RULES];
	int total_rules;
};

struct Configuration{
	char sensors[MAX_SENSORS][MAX_WORD_SZ];
	char actuators[MAX_ACTUATORS][MAX_WORD_SZ];
	char room[MAX_WORD_SZ];
	int total_act;
	int total_sens;
};

struct Measurements{
	float light;
	float humidity;
	float temperature;
	float voltage;
	float current;
	int mote_id;
};

struct Actuators{
	int heater;
	int cooler;
	int light;
	int dehum;
	int humid;
	int blinds;
	int sprink;
};

long byte2long(char *byte1, char *byte2);
float value2light(long light);
float value2hum(long hum);
float value2temp(long temp);
float value2curr(long curr);
float value2voltage(long volt);
float value2humcomp(long temp, long hum, long value);

void readSensors(struct Measurements *sensors, int n_rooms, char* received_msg);
void checkRules(struct Configuration *cfg, struct ROOM_Rules *settings,  struct Measurements *sensors, struct Actuators *actuators, int rooms);
void writeRGBMatrix(struct Actuators *actuators);
void HASController(struct Measurements *sensors, struct Actuators *actuators,int rooms);

int getConfig(struct Configuration *cfg /*returns number of rooms*/);
int getRules(struct Configuration *cfg, struct ROOM_Rules *set, int rooms  /*returns number of rules*/);

int getRoom(struct Configuration *cfg, int rooms, char* compare);
void getThresholds(struct Configuration *cfg, struct Rules *rule, int room, char *str, int section);
void getActuatores(struct Configuration *cfg, struct Rules *rule, int room, char *str);
float getSensorVal (char *settings, struct Measurements *sensors, struct Configuration *cfg, int rooms);