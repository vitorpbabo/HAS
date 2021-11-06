SET search_path to has;

/*Number of sensors available in every room.*/

WITH c1 AS(

    SELECT room_name, COUNT(*) as n_mote
    FROM room
    
    GROUP BY room_name

), c2 AS(

    SELECT
    sensor.room_name AS room, 
    COUNT(sensor.sensorid) AS n_sens

    FROM sensor

    GROUP BY room_name

), c3 AS(

    SELECT
    actuator.room_name AS room, 
    COUNT(actuator.actuatorid) AS n_act

    FROM actuator

    GROUP BY room_name
)

SELECT c2.room as "Room", c1.n_mote as "Mote Count", c2.n_sens as "Sensor Count", c3.n_act as "Actuator Count"

FROM c1
    INNER JOIN c2 ON c1.room_name = c2.room
    INNER JOIN c3 ON c1.room_name = c3.room