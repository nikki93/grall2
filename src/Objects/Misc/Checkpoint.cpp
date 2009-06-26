/*
=========================================
Checkpoint.cpp
=========================================
*/

#define __CHECKPOINT_CPP__

#include "Objects/Misc/Checkpoint.h"

//--- NGF events ----------------------------------------------------------------
Checkpoint::Checkpoint(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mEnabled(true)
{
    addFlag("Checkpoint");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Checkpoint.mesh");
    mEntity->setMaterialName("Objects/Checkpoint");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createTrimesh();

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody();
}
//-------------------------------------------------------------------------------
void Checkpoint::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Checkpoint::~Checkpoint()
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
void Checkpoint::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Checkpoint::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Checkpoint::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Checkpoint::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    //Check he's within some limits, we're a 'gate-like' object.
    Ogre::Vector3 playerPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETPOSITION));
    Ogre::Vector3 playerDisp = playerPos - mNode->getPosition();

    Ogre::Quaternion invRot = Ogre::Vector3::UNIT_X.getRotationTo(mNode->getOrientation() * Ogre::Vector3::UNIT_X);
    playerDisp = invRot * playerDisp;

    if (Ogre::Math::Abs(playerDisp.x) < 0.5 && Ogre::Math::Abs(playerDisp.z) <= 0.2)
    {
        //Save the game. Disable before saving though, so we're loaded disabled.
        bool enabledPrev = mEnabled;
        mEnabled = false;

        if (enabledPrev)
        {
            NGF::Serialisation::Serialiser::save(USER_PREFIX + "Saves/" + GlbVar.levelName);
            mEntity->setMaterialName("Objects/Checkpoint_R");
        }
    }

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Checkpoint)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
