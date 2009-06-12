/*
=========================================
TestWorld.cpp
=========================================
*/

#include "Objects/Misc/RigidBody.h"

#include "Worlds/TestWorld.h"

//-------------------------------------------------------------------------------
void TestWorld::init()
{
    Ogre::LogManager::getSingleton().logMessage("TestWorld init!");

    loadLevel("TestLevel");

    Ogre::String msg =
        "Welcome to GraLL2!\n\n"

        "This game is the sequel to the original GraLL, with a lot of new stuff, including, but not limited to:-\n\n"

        " - Python scripting\n"
        " - Dimension system\n"
        " - Better Physics\n"
        " - Proper save/load\n"
        " - And much, much more :-)\n\n"

        "We hope you have fun, thanks for playing!";

    showMessage(msg, 10);

    //Test Player.
    /*
    GlbVar.goMgr->createObject<Player>(Ogre::Vector3(0.2,10,0), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("dimension1", "yes")
            ("dimension2", "no"));

    //Test StaticBrush. We create a plane mesh for it.
    Ogre::MeshManager::getSingleton().createPlane("Plane.mesh", "General", Ogre::Plane(Ogre::Vector3::UNIT_Y, 0), 100, 100, 10, 10, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

    GlbVar.goMgr->createObject<StaticBrush>(Ogre::Vector3(0.2,5,0), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("brushMeshFile", "Plane.mesh")
            ("dimension1", "yes")
            ("dimension2", "no"));
            */

    /*
    Ogre::Entity *plane = GlbVar.ogreSmgr->createEntity("groundEntity", "Plane.mesh");
    plane->setMaterialName("Player/TEXFACE/Player.png");
    GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode("groundNode")->attachObject(plane);
    
    mTestShape = new btStaticPlaneShape(btVector3(0,1,0),1);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
    mTestBody = new btRigidBody(0, groundMotionState, mTestShape);
    int groundDims = DimensionManager::DIM_1; //Dimensions the ground is in.
    GlbVar.phyWorld->addRigidBody(mTestBody, groundDims, groundDims);
    */
}
//-------------------------------------------------------------------------------
void TestWorld::tick(const Ogre::FrameEvent &evt)
{
}
//-------------------------------------------------------------------------------
void TestWorld::stop()
{
    //delete mTestBody->getMotionState();
    //delete mTestBody;
    //delete mTestShape;
    clearLevel();
}
//-------------------------------------------------------------------------------

