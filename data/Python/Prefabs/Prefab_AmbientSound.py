#AmbientSound Prefab Script
#--------------------------

def init(self):
    #Get the properties.
    soundFile = self.getProperty("soundFile", 0, "Default.wav")
    loop = parseBool(self.getProperty("loop", 0, "1"))
    stream = parseBool(self.getProperty("stream", 0, "1"))

    #Create sound.
    self.v_sound = GraLL2.createSound(soundFile, loop, stream)
    self.v_sound.setPosition(self.getPosition())

    #Set sound properties.
    self.v_sound.setDistanceValues(
        float(self.getProperty("attenuation", 0, "20")),
        float(self.getProperty("attenuation", 1, "4")),
        float(self.getProperty("attenuation", 2, "6"))
    )
    self.v_sound.setGain(
        float(self.getProperty("gain", 0, "1"))
    )

    #Sound test.

def create(self):
    #Play!
    if (parseBool(self.getProperty("initPlay", 0, "1"))):
        self.v_sound.play()

def destroy(self):
    GraLL2.destroySound(self.v_sound)

#Return dict to save.
def save(self):
    values = {
        "playing" : self.v_sound.isPlaying(),
        "offset" : self.v_sound.getSecondOffset(),
        "gain" : self.v_sound.getGain()
    }

    return values

#Get the saved dict, use it.
def load(self, values):
    if (values["playing"]):
        self.v_sound.play()
    else:
        self.v_sound.stop()

    self.v_sound.setSecondOffset(values["offset"])
    self.v_sound.setGain(values["gain"])

    return True
