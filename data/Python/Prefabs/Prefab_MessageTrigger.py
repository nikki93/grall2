#MessageTrigger Prefab Script
#----------------------------

import Ngf
import GraLL2

def create(self):
    self.m_done = False

def collide(self, other):
    if ((not self.m_done) and other.hasFlag("Player")):
        #Get the message property.
        message = self.getProperty("message", 0, "<message>")

        if (message != "<message>"):
            #If it was given, show it.
            GraLL2.showMessage(message.replace("\\n", "\n"),
                               float(self.getProperty("time", 0, "5")))
        else:
            #Otherwise show message from messageDict.
            showMessageFromDict(self.getProperty("messageKey", 0, "default"))

        #Don't to it again. :P
        self.m_done = True
        Ngf.destroyObject(self)
