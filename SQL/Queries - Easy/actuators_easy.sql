SET search_path to has;

/*Current state of all actuators that are installed in every room. */

WITH c1 AS(

    SELECT DISTINCT ON (state.actuatorid)
    state.actuatorid AS actuator,
    state.state AS state,
    state.date_time AS timestamp,
    actuator.room_name AS room

    FROM state 
    INNER JOIN actuator ON state.actuatorid = actuator.actuatorid

    ORDER BY state.actuatorid, timestamp DESC
)

SELECT c1.actuator, c1.state, c1.room 
    FROM c1

