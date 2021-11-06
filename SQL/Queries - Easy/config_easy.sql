SET search_path to has;

/*Considering only two sensor motes and two rooms, 
where each sensor mote is present in a different room,
change the location of both sensor motes, so each room will be
attributed with the sensor mote that was located in the other room.*/


WITH BACKUP1 AS 
(
    SELECT room_name
    
    FROM sensor

    WHERE sensorid = 'TEMP1' 
),

/*1st REWRITE*/

REWRITE AS 
(
    UPDATE sensor 
    
    SET room_name = (SELECT room_name FROM sensor WHERE sensorid = 'TEMP2')
    
    WHERE sensorid = 'TEMP1'
)

/*2nd REWRITE*/

UPDATE sensor 

SET room_name = (SELECT room_name FROM BACKUP1) 

WHERE sensorid = 'TEMP2';

SELECT room_name AS "Room", sensorid AS "Sensor"
    FROM sensor
WHERE sensorid = 'TEMP1' OR sensorid='TEMP2'