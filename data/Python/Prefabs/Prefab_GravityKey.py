#GravityKey Prefab Script
#------------------------

import GraLL2

def pickedUp(self):
    GraLL2.player.setGravitySwitchTime(
        float(self.getProperty("time", 0, "10"))
    )
