SET search_path to has;

/*Number of sensors available in every room.*/

SELECT 
    room.room_name as "room", 
    COUNT(rules) as "rules"

FROM room
    JOIN rules
    ON room.room_name = rules.room_name

GROUP BY room.room_name
