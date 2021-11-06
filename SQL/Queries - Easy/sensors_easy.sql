SET search_path to has;

/*Measurement history of one sensor of type A, which is present in room B, within a time duration C.*/

SELECT 
    DATE_TRUNC('second', measurements.date_time) AS timestamp,
    measurements.value AS measurement

FROM measurements
    INNER JOIN sensor ON measurements.sensorid = sensor.sensorid

WHERE SUBSTR(measurements.sensorid,1,4)='TEMP' 
AND sensor.room_name='ROOM1' 
AND date_time BETWEEN (SELECT MIN(date_time) FROM measurements) AND (SELECT MAX(date_time) FROM measurements)