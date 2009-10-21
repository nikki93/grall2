#Trap Prefab Script
#------------------

import GraLL2
import Ngf

def init(self):
    #Create a Pad mesh, set the Trap material, and make a Box physics body.
    self.createMesh("Template_Pad.mesh")
    self.setMaterial("Objects/Trap")
    self.createBody(GraLL2.CollisionShape.Box, GraLL2.BodyType.Static,
                    GraLL2.CollisionFlags.Static)

    #We get the time from the properties.
    self.m_bombTime = float(self.getProperty("time", 0, "0.56"))

    #Not touched yet.
    self.m_unTouched = True

def collide(self, other):
    #If not touched yet, and other is Player and above us, we start waiting for
    #bomb-time
    if (self.m_unTouched and other.hasFlag("Player")):
        if (other.getPosition().y > (self.getPosition().y + 0.55)):
            self.m_unTouched = False
            self.setAlarm(self.m_bombTime, 0)

def alarm(self, n):
    if (n == 0):
        #Bombs away! :D
        Ngf.createObject("Bomb", "", self.getPosition() +
                         Ngf.Vector3(0,0.75,0), Ngf.Quaternion.IDENTITY, {})
