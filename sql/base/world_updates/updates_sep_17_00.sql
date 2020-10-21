-- Remove Greenstone Village from disabled map
DELETE FROM `disables` WHERE `sourceType` = 2 AND `entry` = 1024;

-- Enable all spawns on map 1024 (Greenstone Village)
UPDATE `creature` SET `spawnMask` = 4096 WHERE `map` = 1024;

-- Enable all gameobjects on map 1024 (Greenstone Village)
UPDATE `gameobject` SET `spawnMask` = 4096 WHERE `map` = 1024;