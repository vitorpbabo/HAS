SET search_path to has;

/*Consumption of energy (kWh) in a room B, within a time duration C. */

SELECT 
    ((220/*V*/ * value/*I*/)/1000 /*KW*/) AS energy, 
    date_time AS timestamp

FROM 
    measurements
    INNER JOIN sensor ON measurements.sensorid = sensor.sensorid
    
WHERE SUBSTR(measurements.sensorid,1,4)='TEMP'/*USING TEMP FOR CURRENT FROM ALL ACTUATORS*/ 
AND sensor.room_name='ROOM1'
AND date_time BETWEEN (SELECT MIN(date_time) FROM measurements) AND (SELECT MAX(date_time) FROM measurements)