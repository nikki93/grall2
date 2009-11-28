#JumpPad Prefab Script
#---------------------

import GraLL2
import Ngf

def init(self):
    #Create a Pad mesh, set the JumpPad material, and make a Box physics body.
    self.createMesh("Template_Pad.mesh")
    self.setMaterial("Objects/JumpPad")
    self.createBody(GraLL2.CollisionShape.Box, GraLL2.BodyType.Static,
                    GraLL2.CollisionFlags.Static)

    #We get the jump impulse from the properties.
    self.m_impulse = float(self.getProperty("impulse", 0, "15"))

def collide(self, other):
    if (other.hasFlag("Player")):
        other.applyCentralImpulse(Ngf.Vector3(0,self.m_impulse,0))
