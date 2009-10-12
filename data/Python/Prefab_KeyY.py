#KeyY Prefab Script
#------------------

import GraLL2

def create(self):
    #If 'm_numKeyY' never set, set to 0. Otherwise, increment.
    GraLL2.controller.locals.setdefault("m_numKeyY", 0)
    GraLL2.controller.m_numKeyY += 1
