#Acid Prefab Script
#------------------

def create(self):
    self.createMesh("Acid.mesh")
    self.setMaterial("Objects/Acid")
    self.createBoxShape(Ngf.Vector3(1,0.1,1))
    self.createBody(GraLL2.CollisionShape.Manual,
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
        other.die(False, True)
