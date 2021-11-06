SET search_path to has;

/*Number of state changes of every actuator of type D,
which is installed in every room, within a time duration C.*/

WITH c1 AS(

    SELECT 
    COUNT(state)-1 AS changes,
    actuator.room_name as room

    FROM state
        INNER JOIN actuator ON state.actuatorid = actuator.actuatorid

    WHERE SUBSTR(state.actuatorid,1,4)='HEAT'
    AND state.date_time BETWEEN (SELECT MIN(date_time) FROM state) AND (SELECT MAX(date_time) FROM state)

    GROUP by actuator.room_name
)

SELECT c1.room as "room", c1.changes AS "change"
    FROM c1