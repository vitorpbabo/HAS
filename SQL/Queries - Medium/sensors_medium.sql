SET search_path to has;

/*Average of all sensors of type A that are present in all rooms, within a time duration C.*/

SELECT 
    AVG(measurements.value) AS average,
    sensor.room_name AS room

FROM measurements
    INNER JOIN sensor ON measurements.sensorid = sensor.sensorid

WHERE SUBSTR(measurements.sensorid,1,4)='TEMP' 
AND date_time BETWEEN (SELECT MIN(date_time) FROM measurements) AND (SELECT MAX(date_time) FROM measurements)
GROUP by sensor.room_name