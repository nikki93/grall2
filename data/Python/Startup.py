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
"""Hmm... Seems there should have been a message here but it was left
out. Contact the creator of the level. :P""",

"checkpoint":
"""Roll through a Checkpoint to save the game. The game can be resumed from
the last stored Checkpoint. The stored Checkpoint can be removed from the
'Level Select' menu.

You can't use the same Checkpoint twice.""",

"bomb":
"""Don't touch the Bombs!""",

"greenbomb":
"""Crates can be used to pass Green Bombs.""",

"ice":
"""Ice is slippery.""",

"oneway":
"""One Way screens allow you to pass through them only in one direction""",

"trap":
"""Don't stay on the Traps ahead for too long!""",

"door":
"""Doors can be opened.""",

"colourKey":
"""Some Doors need Keys. You can see how many Keys you have in the bottom
left of the screen.""",

"yellowKey":
"""Yellow Keys are special - you need to get all of them. You can see how
many Yellow Keys are left in the level in yellow to the bottom left of
the screen.""",

"fallingFloor":
"""Some floors may fall."""
}
