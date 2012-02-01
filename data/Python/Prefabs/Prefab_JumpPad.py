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

    #We get the jump impulse from the properties.
    self.m_impulse = float(self.getProperty("impulse", 0, "15")) * 0.5

    #Create sound.
    self.v_sound = GraLL2.createSound("JumpPad.wav", False, False)
    self.attachSound(self.v_sound)
    self.v_sound.setReferenceDistance(1.2)
    self.v_sound.setGain(7)

def create(self):
    self.m_canJump = True

def collide(self, other):
    if (self.m_canJump and other.hasFlag("Jumper")):
        #Play sound.
        self.v_sound.stop()
        self.v_sound.play()

        #Jump!
        other.applyCentralImpulse(Ngf.Vector3(0,self.m_impulse,0))

        #Don't jump too frequently.
        self.m_canJump = False
        self.setAlarm(0.5, 0)

def alarm(self, n):
    self.m_canJump = True
