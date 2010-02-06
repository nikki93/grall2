#Door Prefab Script
#------------------

import GraLL2

#--- Methods ------

def open(self):
    #Make brush move forward, and play sound.
    self.p_forward = True
    self.v_sound.play()
    self.m_opened = True
    self.m_moving = True

def close(self):
    #Make brush go back, play sound.
    self.p_forward = False
    self.v_sound.play()

#--- Events -------

def init(self):
    #Sound we play when moving.
    self.v_sound = GraLL2.createSound("DoorOpen.wav", True, False)
    self.attachSound(self.v_sound)
    self.v_sound.setReferenceDistance(1.2)

    #Methods.
    self.open = open
    self.close = close

def create(self):
    #Not opened yet!
    self.m_opened = False

    #We're not moving yet!
    self.m_moving = False

    #Save some properties.
    self.m_repeat = parseBool(self.getProperty("repeat", 0, "1"))
    self.m_autoClose = parseBool(self.getProperty("autoClose", 0, "yes"))
    self.m_condition = self.getProperty("condition", 0, "True")

def destroy(self):
    GraLL2.destroySound(self.v_sound)

def collide(self, other):
    #If we're already moving, then nothing to check. If we're repeating, then do it. If we're not 
    #repeating, this must be first time. Also, the other object must be a 'Doorer'.
    if ((not self.m_moving) and (self.m_repeat or (not self.m_opened)) and other.hasFlag("Doorer")):
        #Everything else is fine, check the door condition itself now.
        if (eval(self.m_condition)):
            self.open()

def point(self, n):
    self.v_sound.stop()

    if (n == 0):
        #We hit the start point, so we've stopped moving.
        self.m_moving = False

    elif ((n == 1)):
        #If we're auto-closing, we close after a small delay after
        #reaching end point.
        if (self.m_autoClose):
            self.setAlarm(1, 0)

def alarm(self, n):
    #Delay is over, close.
    self.close()

