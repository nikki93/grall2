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
            #If it was given, show the message, with the given time.
            message = message.replace("\\n", "\n")
            time = float(self.getProperty("time", 0, "5"))
        else:
            #Otherwise, get the message from the messageDict. If the messageKey
            #isn't given either, shows the 'default' message.
            pair = messageDict[self.getProperty("messageKey", 0, "default")]
            message = pair[0]
            time = pair[1]

        GraLL2.showMessage(message, time)

        #Don't to it again. :P
        self.m_done = True
        Ngf.destroyObject(self)
