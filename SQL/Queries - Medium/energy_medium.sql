SET search_path to has;

/*Energy cost (in €) in all rooms, within a time duration C. */

WITH c1 as( /*TIME DURATION C - CURRENTLY STORED APROX:  '2020-05-29 21:27:30' to '2020-05-29 21:31:30' ~3min*/
 
    SELECT  ((  
                DATE_PART('day',    (SELECT MAX(date_time) FROM measurements)::timestamp - (SELECT MIN(date_time) FROM measurements)::timestamp) * 24 + 
                DATE_PART('hour',   (SELECT MAX(date_time) FROM measurements)::timestamp - (SELECT MIN(date_time) FROM measurements)::timestamp)) * 60 +
                DATE_PART('minute', (SELECT MAX(date_time) FROM measurements)::timestamp - (SELECT MIN(date_time) FROM measurements)::timestamp)) * 60 +
                DATE_PART('second', (SELECT MAX(date_time) FROM measurements)::timestamp - (SELECT MIN(date_time) FROM measurements)::timestamp) as delta_t /*seconds*/,

    sensor.room_name AS room

    FROM 
    sensor
    INNER JOIN measurements ON sensor.sensorid = measurements.sensorid 

    GROUP BY sensor.room_name 

), c2 as(   /*AVG CURRENT*/

    SELECT
    AVG(measurements.value)/5 as current, /*5 = CURRENT CORRECTION FACTOR*/
    sensor.room_name AS room

    FROM 
    sensor
    INNER JOIN measurements ON sensor.sensorid = measurements.sensorid 
    
    WHERE SUBSTR(measurements.sensorid,1,4)='TEMP'/*USING TEMP FOR CURRENT FROM ALL ACTUATORS*/
    AND date_time BETWEEN (SELECT MIN(date_time) FROM measurements) AND (SELECT MAX(date_time) FROM measurements)

    GROUP BY sensor.room_name
)

SELECT c1.room as "room", ( (220/*V*/ * c2.current/*I*/)/1000 /*Power in KW*/ * c1.delta_t/3600/*horus*/ * 0.1544/*Cost per hour*/ ) as "cost"

FROM c1 INNER JOIN c2 ON  c1.room = c2.room