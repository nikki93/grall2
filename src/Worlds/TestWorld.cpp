/*
=========================================
TestWorld.cpp
=========================================
*/

#include "Worlds/TestWorld.h"
#include "Objects/Main/GraLL2GameObject.h"

//-------------------------------------------------------------------------------
void TestWorld::init()
{
    Ogre::LogManager::getSingleton().logMessage("TestWorld init!");

    GlbVar.ogreCamera->setPosition(15,15,15);
    GlbVar.ogreCamera->lookAt(Ogre::Vector3::ZERO);

    //Test object.
    GlbVar.goMgr->createObject<GraLL2GameObject>(Ogre::Vector3(0,10,0), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create("dimension1", "yes").addProperty("dimension2", "no"), "obj1");
    GlbVar.goMgr->createObject<GraLL2GameObject>(Ogre::Vector3(0,15,0), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create("dimension1", "yes").addProperty("dimension2", "no"), "obj2");
    GlbVar.goMgr->createObject<GraLL2GameObject>(Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, NGF::PropertyList::create("dimension1", "no").addProperty("dimension2", "yes"), "main");
}
//-------------------------------------------------------------------------------
void TestWorld::tick(const Ogre::FrameEvent &evt)
{
    PyRun_SimpleString
        (
         "obj = Ngf.getObject(\"main\")\n"
         "obj.setLinearVelocity(Ngf.Vector3.ZERO)"
        );
}
//-------------------------------------------------------------------------------
void TestWorld::stop()
{
    GlbVar.goMgr->destroyAll();
}
//-------------------------------------------------------------------------------

