/*
=========================================
Pickup.cpp
=========================================
*/

#define __PICKUP_CPP__

#include "Objects/Obstacles/Pickup.h"

//--- NGF events ----------------------------------------------------------------
Pickup::Pickup(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTime(0)
{
    addFlag("Pickup");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Store properties.
    mPickupType = mProperties.getValue("pickupType", 0, "KeyR");
    mSpin = Ogre::Math::DegreesToRadians(Ogre::StringConverter::parseReal(mProperties.getValue("spin", 0, "0")));
    mBob = Ogre::StringConverter::parseReal(mProperties.getValue("bob", 0, "0"));

    //Create the Ogre stuff. Pickups can be brushes too. :-)
    if (mProperties.getValue("brushMeshFile", 0, "n") == "n")
    {
        Ogre::String meshFile = mProperties.getValue("meshFile", 0, "Mesh_Key.mesh");
        Ogre::String material = mProperties.getValue("material", 0, "Objects/KeyR");
        mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, meshFile);
        mEntity->setMaterialName(material);
    }
    else
    {
        mEntity = createBrushEntity();
    }
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createConvex();

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody( DimensionManager::NO_DIM_CHECK
            | DimensionManager::NO_BULLET_HIT
            );
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}
//-------------------------------------------------------------------------------
void Pickup::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Pickup::~Pickup()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    destroyBody();
    delete mShape;

    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Pickup::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Get trans.
    btTransform trans;
    mBody->getMotionState()->getWorldTransform(trans);

    //Spin!
    btQuaternion rot(btVector3(0,1,0), mSpin * evt.timeSinceLastFrame);

    //Bob!
    mTime += evt.timeSinceLastFrame * 3;
    btVector3 disp = btVector3(0,Ogre::Math::Sin(mTime) * mBob * 0.02,0);

    //Set trans.
    trans *= btTransform(rot, disp);
    mBody->setWorldTransform(trans);
    mBody->getMotionState()->setWorldTransform(trans);

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Pickup::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Pickup::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_GETPICKUPTYPE:
            NGF_SEND_REPLY(mPickupType);
        case MSG_PICKEDUP:
            //We've been picked up! Now we become 'NONE' so that no more pickups are registered,
            //just in case Bullet sends a few callbacks before destruction.
            mPickupType = "NONE";
            GlbVar.goMgr->requestDestroy(getID());
            NGF_NO_REPLY();
    }
    
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Pickup::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Pickup)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
