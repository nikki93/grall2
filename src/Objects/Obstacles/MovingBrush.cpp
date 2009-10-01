/*
=========================================
MovingBrush.cpp
=========================================
*/

#define __MOVINGBRUSH_CPP__

#include <LinearMath/btGeometryUtil.h>

#include "Objects/Obstacles/MovingBrush.h"

#define CAST_SHAPE_SHRINK 0.1

//--- NGF events ----------------------------------------------------------------
MovingBrush::MovingBrush(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTimer(-1),
      mLastFrameTime(0.1)
{
    addFlag("MovingBrush");

    //Save the director event.
    NGF_PY_SAVE_EVENT(director);
    NGF_PY_SAVE_EVENT(point);

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Get properties.
    mEnabled = Ogre::StringConverter::parseBool(properties.getValue("enabled", 0, "yes"));
    mVelocity = rot * Ogre::Vector3(0,0,-Ogre::StringConverter::parseReal(properties.getValue("speed", 0, "2")));
    mFollowDirectors = Ogre::StringConverter::parseBool(properties.getValue("followDirectors", 0, "yes"));

    //Create the Ogre stuff.
    mEntity = createBrushEntity();
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createConvex();
    mShape->setMargin(0); //Bad, but we gotta.

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape);
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    initBody(DimensionManager::MOVINGBRUSH);

    //Make smaller shape for cast.
    //Get vertices.
    btAlignedObjectArray<btVector3> offsetVerts;
    btVector3 *iter = mShape->getUnscaledPoints(); 
    for (int i = 0; i < mShape->getNumPoints(); ++i, ++iter)
        offsetVerts.push_back(*iter);

    //Push 'em in by 0.1;
    btAlignedObjectArray<btVector3> offsetPlanes;
    btGeometryUtil::getPlaneEquationsFromVertices(offsetVerts, offsetPlanes);
    int sz = offsetPlanes.size();
    for (int i=0 ; i<sz ; ++i) 
        offsetPlanes[i][3] += CAST_SHAPE_SHRINK;
    offsetVerts.clear();
    btGeometryUtil::getVerticesFromPlaneEquations(offsetPlanes, offsetVerts);

    //Fill the shape with the new points.
    mCastShape = new btConvexHullShape();
    for (int i = 0; i < offsetVerts.size() ; ++i) 
        mCastShape->addPoint(offsetVerts[i]);
}
//-------------------------------------------------------------------------------
void MovingBrush::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
MovingBrush::~MovingBrush()
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
void MovingBrush::unpausedTick(const Ogre::FrameEvent &evt)
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
        Ogre::Real sqSpeed = (mVelocity.squaredLength() + 0.02) * evt.timeSinceLastFrame * evt.timeSinceLastFrame;

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
                NGF_PY_CALL_EVENT(point, currPoint);
                if (!mPoints.empty())
                    mVelocity = (mPoints.front() - currPoint).normalisedCopy() * mVelocity.length();
            }
        }

        //Calculate new position.
        btVector3 currVel = BtOgre::Convert::toBullet(mVelocity) * evt.timeSinceLastFrame;
        btVector3 newPos = prevPos + currVel;

        //The cast result callback. Also checks for Director hits.
        struct MovingBrushCheckResult : public btDynamicsWorld::ConvexResultCallback
        {
            btCollisionObject *mIgnore;
            int mDimension;
            bool mHit;
            std::set<NGF::GameObject*> mDirectorsHit;
            bool mYCast;

            MovingBrushCheckResult(btCollisionObject *ignore, int dimension, bool yCast)
                : mIgnore(ignore),
                  mDimension(dimension),
                  mHit(false),
                  mYCast(yCast)
            {
            }

            btScalar addSingleResult(btDynamicsWorld::LocalConvexResult &convexResult, bool)
            {
                //If it's a Director, we save it and pass, otherwise we're hit.
                btCollisionObject *obj = convexResult.m_hitCollisionObject;
                if (obj->getBroadphaseHandle()->m_collisionFilterGroup & DimensionManager::DIRECTOR)
                    mDirectorsHit.insert(NGF::Bullet::fromBulletObject(obj));
                else
                    mHit = true;

                return 1;
            }

            bool needsCollision(btBroadphaseProxy* proxy0) const
            {
                bool y = mYCast;
                y = mYCast;
                if ((btCollisionObject*) proxy0->m_clientObject == mIgnore)
                    return false;

                //If it's us, or isn't in our dimension, we don't care. 
                return ((btCollisionObject*) proxy0->m_clientObject != mIgnore) //Not us.
                    && !(proxy0->m_collisionFilterGroup & DimensionManager::NO_MOVING_CHECK) //Doesn't want to be tested.
                    && (proxy0->m_collisionFilterGroup & mDimension) //It's in the other dimension.
                    && !(mYCast && (proxy0->m_collisionFilterGroup & DimensionManager::LIFTABLE)) //If moving up, forget liftables.
                    && !(proxy0->m_collisionFilterGroup & DimensionManager::MOVINGBRUSH);
                    //&& ((proxy0->m_collisionFilterGroup & DimensionManager::DIRECTOR) //Directors allowed.
                            //|| !(((btCollisionObject*) proxy0->m_clientObject)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE));
            }
        };

        //Where to cast from, where to cast to, etc.
        btVector3 normVel = currVel.normalized();
        btVector3 pos1 = prevPos;
        btVector3 castDisp = currVel + (normVel * CAST_SHAPE_SHRINK);
        btVector3 pos2 = prevPos + castDisp;
        btQuaternion rot = mBody->getWorldTransform().getRotation();
        btTransform trans1(rot, pos1);
        btTransform trans2(rot, pos2);

        //Do the cast.
        MovingBrushCheckResult res(mBody, mDimensions, castDisp.y() > 0);
        GlbVar.phyWorld->convexSweepTest(mCastShape, trans1, trans2, res);

        //If hit Director, get directed. Use timer to avoid getting stuck to Director.
        if (mTimer < 0)
        {
            for (std::set<GameObject*>::iterator iter = res.mDirectorsHit.begin(); iter != res.mDirectorsHit.end(); ++iter)
            {
                NGF::GameObject *other = *iter;
                Ogre::Vector3 otherPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETPOSITION));
                Ogre::Real sqDist = (otherPos - BtOgre::Convert::toOgre(prevPos)).squaredLength() - 0.001;

                if (sqDist < sqSpeed)
                {
                    mBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(otherPos)));
                    jumped = true;

                    //-1 means keep speed, only change direction.
                    Ogre::Quaternion dir = GlbVar.goMgr->sendMessageWithReply<Ogre::Quaternion>(other, NGF_MESSAGE(MSG_GETDIRECTION));
                    Ogre::Real speed = GlbVar.goMgr->sendMessageWithReply<Ogre::Real>(other, NGF_MESSAGE(MSG_GETSPEED));

                    if (speed == -1)
                        mVelocity = mVelocity.getRotationTo(dir * Ogre::Vector3::NEGATIVE_UNIT_Z) * mVelocity;
                    else
                    {
                        //Rotate it by the amount required to rotate it to face a velocity in that direction.
                        mVelocity = dir * Ogre::Vector3(0,0,-speed);
                    }

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

        //Do the actual movement.
        if (!jumped)
        {
            if (res.mHit)
            {
                mVelocity = -mVelocity;
                currVel = -currVel;
                newPos = prevPos + currVel;
            }

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
void MovingBrush::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply MovingBrush::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void MovingBrush::collide(NGF::GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
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

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(MovingBrush)
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
