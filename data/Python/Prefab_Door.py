#Door Prefab Script
#------------------

print "SCRIPT!"

def create(self):
	#Not opened yet!
	self.m_opened = False
	
	#We're not moving yet!
	self.m_moving = False
	
	#Save some properties.
	self.m_repeat = (self.getProperty("repeat", 0, "yes") == "yes")
	self.m_autoClose = (self.getProperty("autoClose", 0, "yes") == "yes")
	self.m_condition = self.getProperty("condition", 0, "True")
	
def collide(self, other):
	#If we're already moving, then nothing to check.
	#If we're repeating, then do it. If we're not repeating, this
	#must be first time. Also, the other object must be a 'Doorer'.
	#Lastly, we check for the 'condition' (evaluate Python string)
	#and if it's fine, then continue.
	if ((not self.m_moving) and (self.m_repeat or (not self.m_opened)) and other.hasFlag("Doorer")):
		if (eval(self.m_condition)):
			self.p_forward = True
			self.m_opened = True
			self.m_moving = True

def point(self, n):
	#We've hit the start point, so we've stopped moving.
	if (n == 0):
		self.m_moving = False
	#If we're auto-closing, we close after a small delay after
	#reaching end point.
	if (self.m_autoClose and (n == 1)):
		self.setAlarm(1, 0)
		
def alarm(self, n):
	#Delay is over, close.
	if (n == 0):
		self.p_forward = False
