#SpeedBoost Prefab Script
#------------------------

import GraLL2

def pickedUp(self):
    #Multiply his rotational speed by whatever multiplier we've stored.
    GraLL2.player.setAngularVelocity(
        GraLL2.player.getAngularVelocity() * 
            float(self.getProperty("multiplier", 0, "2"))
    )
