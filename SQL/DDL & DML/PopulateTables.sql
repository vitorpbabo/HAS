/*******************************************************************************
   HAS Database - Version FINAL
   Description: Populates Tabels on the database - DML
   DB Server: PostgreSql
   Author: Tony Ferreira
********************************************************************************/

/*******************************************************************************
   Populate Tables EXAMPLES
********************************************************************************/

INSERT INTO Room (room_name, moteid) VALUES ('ROOM1', 1);
INSERT INTO Room (room_name, moteid) VALUES ('WC', 2);
INSERT INTO Room (room_name, moteid) VALUES ('TERRACE', 3);
INSERT INTO Room (room_name, moteid) VALUES ('LIVING_ROOM', 4);

INSERT INTO Actuator (actuatorid, room_name) VALUES ('HEAT1', true, 'ROOM1');
INSERT INTO Sensor (sensorid, room_name) VALUES ('TEMP1','ROOM1');

INSERT INTO Measurements (date_time, value, sensorid) VALUES (CURRENT_TIMESTAMP, 25.1, 1);
INSERT INTO Measurements (date_time, value, sensorid) VALUES (CURRENT_TIMESTAMP, 26, 1);
INSERT INTO Measurements (date_time, value, sensorid) VALUES (CURRENT_TIMESTAMP, 24.1, 1);
INSERT INTO Measurements (date_time, value, sensorid) VALUES (CURRENT_TIMESTAMP, 25.5, 1);

INSERT INTO Configuration (room_name, sensor1, sensor2, actuator1, actuator2) VALUES ('ROOM1', 'TEMP1', 'LIGHT1', 'HEAT1', 'BLINDS1');
INSERT INTO Configuration (room_name, sensor1, sensor2, actuator1, actuator2) VALUES ('WC', 'TEMP2', 'HUM2', 'HEAT2', 'DEHUMIDIFIER2');
INSERT INTO Configuration (room_name, sensor1, sensor2, sensor3, actuator1, actuator2) VALUES ('TERRACE', 'TEMP3', 'HUM3', 'LIGHT3', 'SPRINKLES3', 'LIGHT3');
INSERT INTO Configuration (room_name, sensor1, sensor2, actuator1, actuator2, actuator3) VALUES ('LIVING_ROOM', 'TEMP4', 'LIGHT4', 'HEAT4', 'LIGHT4', 'BLINDS4');

/*ROOM1*/
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1) VALUES (1, 'ROOM1', 'TEMP1', '=', 25);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1, actuator2, state2) VALUES (2, 'ROOM1', 'LIGHT1', '<', 100, 'BLINDS1', true, 'LIGHT1', true);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, logic, sensor2, condition2, parameter2, actuator1, state1, actuator2, state2) VALUES (3, 'ROOM1', 'LIGHT3', '>', 250, 'AND', 'LIGHT1', '<', 100, 'BLINDS1', false, 'LIGHT1', false);
/*WC*/
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1) VALUES (4, 'WC', 'TEMP2', '<', 22, 'HEAT2', false);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1) VALUES (5, 'WC', 'TEMP2', '>', 28, 'HEAT2', true);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1,logic, sensor2, condition2, parameter2, actuator1, state1) VALUES (6, 'WC', 'HUM2', '<', 60, 'AND', 'TEMP2', '<', 25, 'DEHUMIDIFIER2', false);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1) VALUES (7, 'WC', 'HUM2', '>', 70, 'DEHUMIDIFIER2', true);
/*TERRACE*/
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1,logic, sensor2, condition2, parameter2, actuator1, state1) VALUES (8, 'TERRACE', 'HUM3', '<', 40, 'OR', 'TEMP3', '>', 30, 'SPRINKLES3', true);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1) VALUES (9, 'TERRACE', 'HUM3', '>', 70, 'SPRINKLES3', false);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1) VALUES (10, 'TERRACE', 'LIGHT3', '<', 100, 'LIGHT3', true);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, actuator1, state1) VALUES (11, 'TERRACE', 'LIGHT3', '>', 150, 'LIGHT3', true);
/*LIVING_ROOM*/
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1) VALUES (12, 'LIVING_ROOM', 'TEMP4', '=', 22);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, logic, sensor2, condition2, parameter2, actuator1, state1, actuator2, state2) VALUES (13, 'LIVING_ROOM', 'LIGHT4', '<', 100, 'OR', 'LIGHT3', '<', 100, 'BLINDS4', true, 'LIGHT4', true);
INSERT INTO Rules (ruleid, room_name, sensor1, condition1, parameter1, logic, sensor2, condition2, parameter2, actuator1, state1, actuator2, state2) VALUES (14, 'LIVING_ROOM', 'LIGHT4', '>', 150, 'OR', 'LIGHT3', '>', 250, 'BLINDS4', false, 'LIGHT4', false);
