#MessageTrigger Prefab Script
#----------------------------

def create(self):
    self.m_done = False

def collide(self, other):
    if ((not self.m_done) and other.hasFlag("Player")):
        message = self.getProperty("message", 0, "Hmm... It seems there should have been a message here, but it was\nleft out. Contact the creator of the level. :P")
        GraLL2.showMessage(message.replace("\\n", "\n"), float(self.getProperty("time", 0, "5")))

        self.m_done = True
        Ngf.destroyObject(self)
