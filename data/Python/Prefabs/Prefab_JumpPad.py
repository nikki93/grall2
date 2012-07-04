#JumpPad Prefab Script
#---------------------

import Ngf
import GraLL2

def init(self):
    #Create a Pad mesh, set the JumpPad material, and make a Box physics body.
    self.createMesh("Template_Pad.mesh")
    self.setMaterial("Objects/JumpPad")
    self.createBody(GraLL2.CollisionShape.Box, GraLL2.BodyType.Static,
                    GraLL2.CollisionFlags.Static)

    #Create Player detection cast.
    unitY = self.getOrientation() * Ngf.Vector3.UNIT_Y
    self.createBoxCast(Ngf.Vector3(0.3, 0.1, 0.3), 1.2, 
            self.getPosition() + 0.35*unitY,
            self.getPosition() + 0.45*unitY)

    #We get the jump impulse from the properties.
    self.m_impulse = float(self.getProperty("impulse", 0, "15")) * 0.5

    #Create sound.
    self.v_sound = GraLL2.createSound("JumpPad.wav", False, False)
    self.attachSound(self.v_sound)
    self.v_sound.setReferenceDistance(1.2)
    self.v_sound.setGain(7)

def cast(self):
     #Play sound.
     self.v_sound.stop()
     self.v_sound.play()

     #Jump!
     v = GraLL2.player.getLinearVelocity()
     if (GraLL2.isUp):
         GraLL2.player.setLinearVelocity(Ngf.Vector3(v.x, self.m_impulse, v.z))
     else:
         GraLL2.player.setLinearVelocity(Ngf.Vector3(v.x, -self.m_impulse, v.z))

