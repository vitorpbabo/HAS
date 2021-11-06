/*******************************************************************************
   HAS Database - Version FINAL
   Description: Creates Tables on the database - DDL
   DB Server: PostgreSql
   Author: Vitor Babo
********************************************************************************/
SET search_path to has;
/*******************************************************************************
   Create Tables
********************************************************************************/

CREATE TABLE Room
(
    room_name VARCHAR(20) NOT NULL PRIMARY KEY,
    moteid INT NOT NULL 
);

CREATE TABLE Configuration
(
    room_name VARCHAR(20) NOT NULL PRIMARY KEY,
    sensor1 VARCHAR(20) NOT NULL,
    sensor2 VARCHAR(20),
    sensor3 VARCHAR(20),
    actuator1 VARCHAR(20) NOT NULL,
    actuator2 VARCHAR(20),
    actuator3 VARCHAR(20)
);

CREATE TABLE Rules
(
    ruleid INTEGER NOT NULL PRIMARY KEY,
    sensor1 VARCHAR(20) NOT NULL,
    condition1 CHAR(1) NOT NULL,
    parameter1 NUMERIC(6,2) NOT NULL,
    logic CHAR(3),
    sensor2 VARCHAR(20),
    condition2 CHAR(3),
    parameter2 NUMERIC(6,2),
    actuator1 VARCHAR(20),
    state1 BOOL,
    actuator2 VARCHAR(20),
    state2 BOOL,
    room_name VARCHAR(20) NOT NULL
);

CREATE TABLE Actuator
(
    actuatorid VARCHAR(20) NOT NULL PRIMARY KEY,
    room_name VARCHAR(20) NOT NULL
);

CREATE TABLE State
(
    date_time TIMESTAMP PRIMARY KEY,
    state BOOL,
    actuatorid VARCHAR(20) NOT NULL
);

CREATE TABLE Sensor
(
    sensorid VARCHAR(20) NOT NULL PRIMARY KEY,
    room_name VARCHAR(20) NOT NULL

);

CREATE TABLE Measurements
(
    date_time TIMESTAMP PRIMARY KEY,
    value NUMERIC(6,2),
    sensorid VARCHAR(20) NOT NULL

);

/*******************************************************************************
   Create Foreign Keys
********************************************************************************/

ALTER TABLE Configuration 
  ADD CONSTRAINT fk_Configuration_room_name 
  FOREIGN KEY (room_name) 
  REFERENCES Room(room_name) 
  ON UPDATE CASCADE;

ALTER TABLE Rules 
  ADD CONSTRAINT fk_Rules_room_name
  FOREIGN KEY (room_name) 
  REFERENCES Configuration(room_name) 
  ON UPDATE CASCADE;

ALTER TABLE Actuator 
  ADD CONSTRAINT fk_Actuator_room_name
  FOREIGN KEY (room_name) 
  REFERENCES Room(room_name) 
  ON UPDATE CASCADE;

ALTER TABLE  State
  ADD CONSTRAINT fk_State_actuatorid
  FOREIGN KEY (actuatorid) 
  REFERENCES Actuator(actuatorid) 
  ON UPDATE CASCADE;

ALTER TABLE Sensor 
  ADD CONSTRAINT fk_Sensor_room_name
  FOREIGN KEY (room_name) 
  REFERENCES Room(room_name) 
  ON UPDATE CASCADE;

ALTER TABLE  Measurements
  ADD CONSTRAINT fk_Measurements_sensorid
  FOREIGN KEY (sensorid) 
  REFERENCES Sensor(sensorid) 
  ON UPDATE CASCADE;


