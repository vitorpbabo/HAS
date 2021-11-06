SET search_path to has;

/*Modify the reference value of a sensor of type A in a given rule*/

UPDATE rules

SET parameter1 = 27

WHERE ruleid=0 AND SUBSTR(sensor1,1,4)='TEMP';

SELECT 
    ruleid AS "Rule", 
    room_name AS "Room",  
    parameter1 AS "Reference Value", 
    sensor1 AS "Sensor"

    FROM rules

WHERE ruleid=0
