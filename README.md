Controls:
W,A,S,D to move
Left mouse button click for basic attack, shooting a dagger in the direction character is facing, cooldown 0.25s
Right mouse button for charged ability attack, as ability is charged it will spawn multiple daggers around the character (up to 8), when input is released, spawned daggers will release and shoot out from around the player, if input is released within 10% of max charge either before or after, it will double the number of projectiles, max charge time is 5 seconds

Changes:
I have used Gameplay Ability system to implement basic attack and charged attack abilities with cooldowns and charged ability blocking basic attack from being used while charged ability is being charged
Charged attack spawns visual representation of the attack as it is being charged (daggers around the player set up in equal increments depending on the charge time), implemented using AbilityTask
Implemented health attribute, using Attribute set
Implemented projectiles which apply damage, using gameplay effects
Implemented HUD displaying cooldown of the abilities as they are used, using Gameplay tags, and gameplay effects
Implemented stationary NPC enemies which can be killed
Changed input changing navigation to WASD from mouse clicks and made character rotate towards the mouse cursor instead of direction of movement
