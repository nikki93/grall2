#KeyY Prefab Script
#------------------

import GraLL2

def create(self):
    #Increment count when created.
    GraLL2.player.incPickup("KeyY")

def pickedUp(self):
    #Decrement count when picked up. We decrement twice because the Player
    #already incremented it once (hacky, I know :( ).
    GraLL2.player.decPickup("KeyY")
    GraLL2.player.decPickup("KeyY")
