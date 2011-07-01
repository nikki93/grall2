#SpeedBoost Prefab Script
#------------------------

import GraLL2
import Ngf

def pickedUp(self):
    #Multiply his rotational speed by whatever multiplier we've stored.
    v = GraLL2.player.getLinearVelocity() * float(self.getProperty("multiplier", 0, "3"))

    #Remove y component if needed.
    if (parseBool(self.getProperty("removeY", 0, "no"))):
        v.y = 0

    GraLL2.player.setLinearVelocity(v)
