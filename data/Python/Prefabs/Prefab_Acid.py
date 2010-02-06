#Acid Prefab Script
#------------------

import GraLL2

def init(self):
    #The mesh is a brush representing the acid region.
    self.createBrushMesh()
    self.setMaterial("Objects/Acid")
    self.createBody(GraLL2.CollisionShape.Convex,
                    GraLL2.BodyType.Static | GraLL2.BodyType.No_Contact,
                    GraLL2.CollisionFlags.No_Dim_Check |
                    GraLL2.CollisionFlags.No_Crate_Check |
                    GraLL2.CollisionFlags.No_Moving_Check |
                    GraLL2.CollisionFlags.No_Bullet_Hit
                   )

    self.m_unTouched = True

def collide(self, other):
    if (self.m_unTouched and other.hasFlag("Player")):
        self.m_unTouched = False

        #No explosion, leave 'corpse'.
        other.die(False, True)
