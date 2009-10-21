/*
=========================================
Bullet.cpp
=========================================
*/

#define __BULLET_CPP__

#include "Objects/Bots/Bullet.h"

#include "Objects/Misc/Light.h"

#define MAX_LIFETIME 30

//--- NGF events ----------------------------------------------------------------
Bullet::Bullet(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mExploded(false),
      mLifetime(0)
{
    addFlag("Bullet");
    addFlag("Explosive");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Read properties.
    mSpeed = Ogre::StringConverter::parseReal(mProperties.getValue("speed", 0, "35"));

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Bullet.mesh");
    mEntity->setMaterialName("Objects/Bullet1");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = new btCylinderShapeZ(BtOgre::Convert::toBullet(converter.getSize() * 0.5));

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape);
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    //Player won't know about bullets in other dimension. >:-)
    initBody( DimensionManager::NO_DIM_CHECK 
            | DimensionManager::NO_CRATE_CHECK
            | DimensionManager::NO_MOVING_CHECK
            | DimensionManager::NO_BULLET_HIT
            | DimensionManager::INVISIBLE
            );
    setBulletObject(mBody);
}
//-------------------------------------------------------------------------------
void Bullet::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Bullet::~Bullet()
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
void Bullet::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Move!
    btTransform oldTrans;
    mBody->getMotionState()->getWorldTransform(oldTrans);
    btVector3 oldPos = oldTrans.getOrigin();
    btQuaternion oldRot = oldTrans.getRotation();

    btVector3 vel = quatRotate(oldRot, btVector3(0,0,-mSpeed * evt.timeSinceLastFrame));
    btVector3 newPos = oldPos + vel;

    //Do a raycast.
    struct ClickRayResult : public btDynamicsWorld::ClosestConvexResultCallback
    {
        btCollisionObject *mIgnore;
        int mDimension;

        ClickRayResult(int dimension, btCollisionObject *ignore, const btVector3 &from, const btVector3 &to)
            : btCollisionWorld::ClosestConvexResultCallback(from, to),
              mIgnore(ignore),
              mDimension(dimension)
        {
        }

        bool needsCollision(btBroadphaseProxy* proxy0) const
        {
            return ((btCollisionObject*) proxy0->m_clientObject != mIgnore)
                && !(proxy0->m_collisionFilterGroup & DimensionManager::NO_BULLET_HIT)
                && (proxy0->m_collisionFilterGroup & mDimension);
        }
    };

    btVector3 pos1 = oldPos;
    btVector3 pos2 = newPos;
    ClickRayResult res(mDimensions, mBody, pos1, pos2);

    GlbVar.phyWorld->convexSweepTest(mShape, btTransform(oldRot,oldPos), btTransform(oldRot,newPos), res);

    if (res.m_hitCollisionObject)
    {
        //If hit, explode, and tell the guy.
        explode();
        NGF::GameObject *obj = NGF::Bullet::fromBulletObject(res.m_hitCollisionObject);
        if (obj)
            GlbVar.goMgr->sendMessage(obj, NGF_MESSAGE(MSG_BULLETHIT));
    }
    else
    {
        //Otherwise keep going.
        mBody->getMotionState()->setWorldTransform(btTransform(oldRot, oldPos + vel));
    }

    //We can't stay forever. D:
    mLifetime += evt.timeSinceLastFrame;
    if (mLifetime > MAX_LIFETIME)
        explode();
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Bullet::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Bullet::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_EXPLODE:
            explode();
            NGF_NO_REPLY();
    }
    
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Bullet::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void Bullet::explode()
{
    if (mExploded)
        return;

    //Us no more. :-(
    GlbVar.goMgr->requestDestroy(getID());
    mExploded = true;
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Bullet)
{
    NGF_PY_METHOD_IMPL(explode)
    {
        explode();
        NGF_PY_RETURN();
    }
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
