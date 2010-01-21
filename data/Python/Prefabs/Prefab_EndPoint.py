#EndPoint Prefab Script
#----------------------

import Ngf
import GraLL2

def init(self):
    self.createMesh("EndPoint.mesh")
    self.setMaterial("Objects/EndPoint")
    self.createBody(GraLL2.CollisionShape.CylinderY, 
                    GraLL2.BodyType.Static | GraLL2.BodyType.No_Contact,
                    GraLL2.CollisionFlags.No_Dim_Check |
                    GraLL2.CollisionFlags.No_Crate_Check |
                    GraLL2.CollisionFlags.No_Moving_Check |
                    GraLL2.CollisionFlags.No_Bullet_Hit
                   )

    self.m_unTouched = True

def create(self):
    #We do this in the 'create' event so that it's only done on .ngf load, and
    #not deserialisation.
    lightPos = self.getPosition() - Ngf.Vector3(0,0.8,0)
    Ngf.createObject("Light", "", lightPos, Ngf.Quaternion.IDENTITY, {
        "colour" : "0.7 0.4 0.02",
        "attenuation" : "10 0.8 0.2 0.09"
    })

def utick(self, elapsed):
    if (not self.m_unTouched):
        player = Ngf.getObject("player1")
        if (player):
            #Push him toward us.
            dir = self.getPosition() - player.getPosition()
            dir.y = 0
            dist = dir.squaredLength()
            if (dist > 0.04):
                player.applyCentralForce(dir * 15)
            else:
                player.applyCentralForce(dir * 7)

def collide(self, other):
    if (other.hasFlag("Player")):
        if (self.m_unTouched):
            #We win! :D
            self.m_unTouched = False
            GraLL2.winLevel(True)
