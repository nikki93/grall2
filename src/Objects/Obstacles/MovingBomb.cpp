/*
=========================================
MovingBomb.cpp
=========================================
*/

#define __MOVINGBOMB_CPP__

#include <LinearMath/btGeometryUtil.h>

#include "Objects/Obstacles/MovingBomb.h"

#include "Objects/Misc/Light.h"
#include "Objects/Misc/ParticleEffect.h"

#define CAST_SHAPE_SHRINK 0.1

//--- NGF events ----------------------------------------------------------------
MovingBomb::MovingBomb(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTimer(-1),
      mLastFrameTime(0.1),
      mFollowDirectors(true),
      mExploded(false)
{
    addFlag("MovingBomb");
    addFlag("Explosive");

    //Save the director event.
    NGF_PY_SAVE_EVENT(director);

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Get properties.
    mEnabled = Ogre::StringConverter::parseBool(properties.getValue("enabled", 0, "yes"));
    mVelocity = rot * Ogre::Vector3(0,0,-Ogre::StringConverter::parseReal(properties.getValue("speed", 0, "2")));

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "MovingBomb.mesh");
    mEntity->setMaterialName("Objects/MovingBomb");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = new btSphereShape(0.45);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape);
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions | DimensionManager::NO_DIM_CHECK, mDimensions);
    setBulletObject(mBody);

    //Make smaller shape for cast.
    mCastShape = new btSphereShape(converter.getRadius() - CAST_SHAPE_SHRINK);
}
//-------------------------------------------------------------------------------
void MovingBomb::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
MovingBomb::~MovingBomb()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    delete mCastShape;
    destroyBody();
    delete mShape;

    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void MovingBomb::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);
    bool jumped = false;

    if (mEnabled)
    {
        //Get old place (current place actually).
        btTransform oldTrans;
        mBody->getMotionState()->getWorldTransform(oldTrans);
        btVector3 prevPos = oldTrans.getOrigin();

        //Squared speed.
        Ogre::Real sqSpeed = mVelocity.squaredLength() * evt.timeSinceLastFrame;

        //If we're near the next point on our list then we take it off our list and start moving to the next one (if it exists).
        //This way, you don't _need_ a point-list. You could do it using Directors, or even bouncing between walls, but when they're
        //there, it nicely blends in.
        if (!mPoints.empty())
        {
            Ogre::Vector3 currPoint = mPoints.front();
            Ogre::Real sqDist = (currPoint - BtOgre::Convert::toOgre(prevPos)).squaredLength();
            if (sqDist < sqSpeed)
            {
                mBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(currPoint)));
                jumped = true;
                mPoints.pop_front();
                if (!mPoints.empty())
                    mVelocity = (mPoints.front() - currPoint).normalisedCopy() * mVelocity.length();
            }
        }

        //Calculate new position.
        btVector3 currVel = BtOgre::Convert::toBullet(mVelocity) * evt.timeSinceLastFrame;
        btVector3 newPos = prevPos + currVel;

        //The cast result callback. Also checks for Director hits.
        struct MovingBombCheckResult : public btDynamicsWorld::ConvexResultCallback
        {
            btCollisionObject *mIgnore;
            int mDimension;
            bool mHit;
            std::set<NGF::GameObject*> mDirectorsHit;

            MovingBombCheckResult(btCollisionObject *ignore, int dimension)
                : mIgnore(ignore),
                  mDimension(dimension),
                  mHit(false)
            {
            }

            btScalar addSingleResult(btDynamicsWorld::LocalConvexResult &convexResult, bool)
            {
                //If it's a Director, we save it and pass, otherwise we're hit.
                btCollisionObject *obj = convexResult.m_hitCollisionObject;
                if (obj->getBroadphaseHandle()->m_collisionFilterGroup & DimensionManager::DIRECTOR)
                    mDirectorsHit.insert(NGF::Bullet::fromBulletObject(obj));
                else
                {
                    NGF::GameObject *gobj = NGF::Bullet::fromBulletObject(obj);

                    if (!(gobj->hasFlag("Player")))
                        mHit = true;
                }

                return convexResult.m_hitFraction;
            }

            bool needsCollision(btBroadphaseProxy* proxy0) const
            {
                //If it's us, or isn't in our dimension, we don't care. 
                return ((btCollisionObject*) proxy0->m_clientObject != mIgnore) 
                    && (proxy0->m_collisionFilterGroup & mDimension)
                    && ((proxy0->m_collisionFilterGroup & DimensionManager::DIRECTOR) 
                            || !(((btCollisionObject*) proxy0->m_clientObject)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
            }
        };

        //Where to cast from, where to cast to, etc.
        btVector3 normVel = currVel.normalized();
        btVector3 pos1 = prevPos;
        btVector3 castDisp = currVel + (normVel * (CAST_SHAPE_SHRINK + 0.05));
        castDisp.setY(0);
        btVector3 pos2 = prevPos + castDisp;
        btQuaternion rot = mBody->getWorldTransform().getRotation();
        btTransform trans1(rot, pos1);
        btTransform trans2(rot, pos2);

        //Do the cast.
        MovingBombCheckResult res(mBody, mDimensions);
        GlbVar.phyWorld->convexSweepTest(mCastShape, trans1, trans2, res);

        //If hit Director, get directed. Use timer to avoid getting stuck to Director.
        if (mTimer < 0)
        {
            for (std::set<GameObject*>::iterator iter = res.mDirectorsHit.begin(); iter != res.mDirectorsHit.end(); ++iter)
            {
                NGF::GameObject *other = *iter;
                Ogre::Vector3 otherPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETPOSITION));
                Ogre::Real sqDist = (otherPos - BtOgre::Convert::toOgre(prevPos)).squaredLength();

                if (sqDist < sqSpeed)
                {
                    mBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(otherPos)));
                    jumped = true;
                    mVelocity = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETVELOCITY));

                    //Call the Python director event (seperate from collision event so that we can be notifed exactly when 'directed').
                    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
                    NGF_PY_CALL_EVENT(director, oth->getConnector());

                    //Wait for next time, to avoid sticking to this one.
                    mTimer = 1/(mVelocity.length());
                }
            }
        }
        else
        {
            mTimer -= evt.timeSinceLastFrame;
        }

        //If free, continue, otherwise turn.
        if (res.mHit)
        {
            mVelocity = -mVelocity;
            currVel = -currVel;
            newPos = prevPos + currVel;
        }

        //Do the actual movement.
        if (!jumped)
        {
            oldTrans.setOrigin(newPos);
            mBody->getMotionState()->setWorldTransform(oldTrans);
            mNode->translate(mVelocity * evt.timeSinceLastFrame);
        }
    }

    //Save frame time.
    mLastFrameTime = evt.timeSinceLastFrame;

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void MovingBomb::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply MovingBomb::receiveMessage(NGF::Message msg)
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
void MovingBomb::collide(NGF::GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //The PythonGameObject.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);

    //If Director, get directed.
    /*
    if (mFollowDirectors && (mTimer < 0) && other->hasFlag("Director"))
    {
        Ogre::Vector3 otherPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETPOSITION));
        Ogre::Real sqSpeed = mVelocity.length() * mLastFrameTime * 1.7;
        Ogre::Real sqDist = otherPos.squaredDistance(BtOgre::Convert::toOgre(mBody->getWorldTransform().getOrigin()));
        if (sqDist < sqSpeed)
        {
            mBody->getMotionState()->setWorldTransform(btTransform(mBody->getWorldTransform().getRotation(), BtOgre::Convert::toBullet(otherPos)));
            mVelocity = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETVELOCITY));

            //Call the Python director event (seperate from collision event so that we can be notifed exactly when 'directed').
            NGF_PY_CALL_EVENT(director, oth->getConnector());

            //Wait for next time, to avoid sticking to this one.
            mTimer = 1/(mVelocity.length());
        }
    }
    */

    //Python collision event.
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void MovingBomb::explode()
{
    if (mExploded)
        return;

    //FX!
    GlbVar.goMgr->createObject<Light>(mNode->getPosition(), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("lightType", "point")
            ("colour", "1 0.6 0")
            ("specular", "0.1 0.1 0.1")
            ("attenuation", "10 0.6 0.2 0.1")
            ("time", "1.6")
            ("fadeOutTime", "0.75")
            );

    GlbVar.goMgr->createObject<ParticleEffect>(mNode->getPosition(), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("template", "ParticleFX/Explosion")
            ("time", "2")
            );

    //Us no more. :-(
    GlbVar.goMgr->requestDestroy(getID());
    mExploded = true;
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(MovingBomb)
{
    //setPosition
    NGF_PY_METHOD_IMPL(setPosition)
    {
        Ogre::Vector3 vec = py::extract<Ogre::Vector3>(args[0]);

        btTransform oldTrans;
        mBody->getMotionState()->getWorldTransform(oldTrans);
        mBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(vec)));

        NGF_PY_RETURN();
    }
    //setOrientation
    NGF_PY_METHOD_IMPL(setOrientation)
    {
        Ogre::Quaternion rot = py::extract<Ogre::Quaternion>(args[0]);

        btTransform oldTrans;
        mBody->getMotionState()->getWorldTransform(oldTrans);
        mBody->getMotionState()->setWorldTransform(btTransform(BtOgre::Convert::toBullet(rot), oldTrans.getOrigin()));

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(addPoint)
    {
        Ogre::Vector3 pos = py::extract<Ogre::Vector3>(args[0]);
        mPoints.push_back(pos);

        NGF_PY_RETURN();
    }

    NGF_PY_PROPERTY_IMPL(enabled, mEnabled, bool);
    NGF_PY_PROPERTY_IMPL(velocity, mVelocity, Ogre::Vector3);
    NGF_PY_PROPERTY_IMPL(followDirectors, mFollowDirectors, bool);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
