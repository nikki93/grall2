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
    self.m_impulse = float(self.getProperty("impulse", 0, "15"))

    #Create sound.
    self.v_sound = GraLL2.createSound("JumpPad.wav", False, False)
    self.attachSound(self.v_sound)
    self.v_sound.setReferenceDistance(1.2)
    self.v_sound.setGain(7)

def create(self):
    #Shouldn't we be using alarms for this?
    self.m_canJump = 0

def utick(self, elapsed):
    if (self.m_canJump > 0):
        self.m_canJump -= elapsed

def collide(self, other):
    if (self.m_canJump <= 0 and other.hasFlag("Player")):
        self.m_canJump = 0.5
        self.v_sound.stop()
        self.v_sound.play()
        other.applyCentralImpulse(Ngf.Vector3(0,self.m_impulse,0))
