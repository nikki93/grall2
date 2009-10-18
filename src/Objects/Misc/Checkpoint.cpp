/*
=========================================
Checkpoint.cpp
=========================================
*/

#define __CHECKPOINT_CPP__

#include "Objects/Misc/Checkpoint.h"

#include "Objects/Misc/Light.h"

#define BLUE_LIGHT 0, 0.2, 1
#define RED_LIGHT 1, 0.2, 0

//--- NGF events ----------------------------------------------------------------
Checkpoint::Checkpoint(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mEnabled(true),
      mLight(0)
{
    addFlag("Checkpoint");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Checkpoint.mesh");
    mEntity->setMaterialName("Objects/Checkpoint_B");
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
    //Create the light.
    if (mProperties.getValue("NGF_SERIALISED", 0, "no") != "yes")
    {
        Ogre::Vector3 offset = Ogre::Vector3(0,0.99,0);
        mLight = GlbVar.goMgr->createObject<Light>(mNode->getPosition() + offset, Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
                ("lightType", "point")
                ("colour", "0 0.8 1")
                ("specular", "0.5 0.5 0.5")
                ("attenuation", "14 0.6 0.2 0.4")
                );
        mLight->setPersistent(true);
    }

    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Checkpoint::~Checkpoint()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //Destroy the light.
    if (mLight)
        GlbVar.goMgr->destroyObject(mLight->getID());

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
    if (!other)
        return;

    if (other->hasFlag("Player"))
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

            //Change light colour.
            setLightColour(false);

            if (enabledPrev)
            {
                Util::saveCheckpoint();
                mEntity->setMaterialName("Objects/Checkpoint_R");
            }
        }
    }

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void Checkpoint::setLightColour(bool blue)
{
    if (mLight)
    {
        Ogre::ColourValue colour = blue ? Ogre::ColourValue(BLUE_LIGHT) : Ogre::ColourValue(RED_LIGHT);
        GlbVar.goMgr->sendMessage(mLight, NGF_MESSAGE(MSG_SETDIFFUSECOLOUR, colour));
    }
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Checkpoint)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
