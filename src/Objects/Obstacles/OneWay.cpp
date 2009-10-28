/*
=========================================
OneWay.cpp
=========================================
*/

#define __ONEWAY_CPP__

#include "Objects/Obstacles/OneWay.h"

//--- NGF events ----------------------------------------------------------------
OneWay::OneWay(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("OneWay");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    
    if (mProperties.getValue("brushMeshFile", 0, "n") == "n")
    {
        mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "OneWay.mesh");

        BtOgre::StaticMeshToShapeConverter converter(mEntity);
        mShape = converter.createBox();
    }
    else
    {
        mEntity = createBrushEntity();

        BtOgre::StaticMeshToShapeConverter converter(mEntity);
        mShape = converter.createTrimesh();
        mShape->setMargin(0);
    }
    mEntity->setMaterialName("Objects/OneWay");

    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody( DimensionManager::NO_DIM_CHECK 
            | DimensionManager::NO_CRATE_CHECK
            | DimensionManager::NO_MOVING_CHECK
            | DimensionManager::NO_BULLET_HIT
            );
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}
//-------------------------------------------------------------------------------
void OneWay::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
OneWay::~OneWay()
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
void OneWay::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void OneWay::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply OneWay::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void OneWay::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    if (other->hasFlag("OneWayer"))
        GlbVar.goMgr->sendMessage(other, NGF_MESSAGE(MSG_ONEWAY, mNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z));

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(OneWay)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
