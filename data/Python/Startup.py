#Run on game startup
#-------------------

#Parse a boolean value from a string.
def parseBool(str):
    return (str == '1') or (str == 'true') or (str == 'yes')

#Find how many yellow keys left.
def yellowKeysLeft():
    if (GraLL2.player):
        return GraLL2.player.numPickup("KeyY")
    else:
        return 7 # O_o

#Whether all yellow keys have been taken.
def yellowKeysTaken():
    if (GraLL2.player):
        return GraLL2.player.numPickup("KeyY") == 0
    else:
        return False

#Messages.
messageDict = {
"default":
("""Hmm... Seems there should have been a message here but it was left
out. Contact the creator of the level. :P""", 7),

"checkpoint":
("""Roll through a Checkpoint to save the game. The game can be resumed from
the last stored Checkpoint. You can't use the same Checkpoint twice.""", 10),

"bomb":
("""Don't touch the Bombs!""", 6),

"greenbomb":
("""Crates can be used to pass Green Bombs.""", 6),

"ice":
("""Ice is slippery.""", 5),

"oneway":
("""One Way screens allow you to pass through them only in one direction""", 7),

"trap":
("""Don't stay on the Traps ahead for too long!""", 7),

"door":
("""Doors can be opened.""", 6),

"colourKey":
("""Some Doors need Keys. You can see how many Keys you have in the bottom
left of the screen.""", 9),

"yellowKey":
("""Yellow Keys are special - you need to get all of them. You can see how
many Yellow Keys are left in the level in yellow to the bottom left of
the screen.""", 10),

"fallingFloor":
("""Some floors may fall.""", 5),

"movingPlatform":
("""Some floors move.""", 5),

"switch":
("""Switches make things happen. There are three types of switches, 'once'
switches, which can only be switched on once, 'press and hold' switches,
and 'toggle' switches.  
    
The switch here is a 'once' switch.""", 15),

"crateSwitch":
("""Crates can be used to activate Switches.""", 7),

"movingBomb":
("""Bombs can move too!""", 6)
}
