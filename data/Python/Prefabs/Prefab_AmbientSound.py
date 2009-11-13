#AmbientSound Prefab Script
#--------------------------

def init(self):
    #Get the properties.
    soundFile = self.getProperty("soundFile", 0, "Default.wav")
    loop = parseBool(self.getProperty("loop", 0, "1"))
    stream = parseBool(self.getProperty("stream", 0, "1"))

    #Create sound.
    self.m_sound = GraLL2.createSound(soundFile, loop, stream)
    self.m_sound.setPosition(self.getPosition())

    #Set sound properties.
    self.m_sound.setDistanceValues(
        float(self.getProperty("attenuation", 0, "20")),
        float(self.getProperty("attenuation", 1, "4")),
        float(self.getProperty("attenuation", 2, "6"))
    )

def create(self):
    #Play!
    if (parseBool(self.getProperty("initPlay", 0, "1"))):
        self.m_sound.play()

def destroy(self):
    GraLL2.destroySound(self.m_sound)

#Return dict to save.
def save(self):
    values = {
        "playing" : self.m_sound.isPlaying(),
        "offset" : self.m_sound.getSecondOffset()
    }

    return values

#Get the saved dict, use it.
def load(self, values):
    if (values["playing"]):
        self.m_sound.play()
    else:
        self.m_sound.stop()

    self.m_sound.setSecondOffset(values["offset"])

    return True
