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
    mSpeed = Ogre::StringConverter::parseReal(mProperties.getValue("speed", 0, "77"));

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Bullet.mesh");
    mEntity->setMaterialName("Objects/BulletR");
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

    //The cast result callback. Also checks for Director hits.
    struct BulletCheckResult : public btDynamicsWorld::ConvexResultCallback
    {
        btCollisionObject *mIgnore;
        int mDimension;
        bool mHit;
        bool mHitPlayer;

        BulletCheckResult(btCollisionObject *ignore, int dimension)
            : mIgnore(ignore),
              mDimension(dimension),
              mHit(false),
              mHitPlayer(false)
        {
        }

        btScalar addSingleResult(btDynamicsWorld::LocalConvexResult &convexResult, bool)
        {
            mHit = true;

            //We hit something good!
            if (convexResult.m_hitCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup & DimensionManager::BULLET_SENSITIVE)
            {
                NGF::GameObject *obj = NGF::Bullet::fromBulletObject(convexResult.m_hitCollisionObject);
                if (obj)
                    GlbVar.goMgr->sendMessage(obj, NGF_MESSAGE(MSG_BULLETHIT));
            }

            return 1;
        }

        bool needsCollision(btBroadphaseProxy* proxy0) const
        {
            //If it's us, or isn't in our dimension, we don't care. 
            return ((btCollisionObject*) proxy0->m_clientObject != mIgnore) 
                && !(proxy0->m_collisionFilterGroup & DimensionManager::NO_BULLET_HIT)
                && (proxy0->m_collisionFilterGroup & mDimension);
        }
    };

    //Where to cast from, where to cast to, etc.
    btVector3 pos1 = oldPos;
    btVector3 pos2 = newPos;
    btQuaternion rot = mBody->getWorldTransform().getRotation();
    btTransform trans1(rot, pos1);
    btTransform trans2(rot, pos2);

    //Do the cast.
    BulletCheckResult res(mBody, mDimensions);
    GlbVar.phyWorld->convexSweepTest(mShape, trans1, trans2, res);

    if (res.mHit)
        explode();
    else
        mBody->getMotionState()->setWorldTransform(btTransform(oldRot, oldPos + vel));

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

    //FX (just light).
    GlbVar.goMgr->createObject("Light", mNode->getPosition(), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("lightType", "point")
            ("colour", "1 0.6 0")
            ("specular", "0.1 0.1 0.1")
            ("attenuation", "10 0.6 0.2 0.10")
            ("time", "0.7")
            ("fadeOutTime", "0.5")
            );

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
