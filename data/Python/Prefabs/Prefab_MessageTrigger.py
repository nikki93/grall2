#MessageTrigger Prefab Script
#----------------------------

def create(self):
    self.m_done = False

def collide(self, other):
    if ((not self.m_done) and other.hasFlag("Player")):
        message = self.getProperty("message", 0, "<message>")

        if (message != "<message>"):
            message = message.replace("\\n", "\n")
        else:
            message = messageDict[self.getProperty("messageKey", 0, "default")]

        GraLL2.showMessage(message.replace("\\n", "\n"), float(self.getProperty("time", 0, "5")))

        self.m_done = True
        Ngf.destroyObject(self)
