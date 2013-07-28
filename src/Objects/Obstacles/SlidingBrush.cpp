/*
=========================================
SlidingBrush.cpp
=========================================
*/

#define __SLIDINGBRUSH_CPP__

#include <LinearMath/btGeometryUtil.h>

#include "Objects/Obstacles/SlidingBrush.h"

#define CAST_SHAPE_SHRINK 0.1

//--- NGF events ----------------------------------------------------------------
SlidingBrush::SlidingBrush(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mCurrentPlace(0.5),
      mLastPlace(1),
      mForward(false)
{
    addFlag("SlidingBrush");

    //Save the director event.
    NGF_PY_SAVE_EVENT(point);

    //Python init event.
    NGF_PY_CALL_EVENT(init);
    
    //Get properties.
    mEnabled = Ogre::StringConverter::parseBool(mProperties.getValue("enabled", 0, "yes"));
    mSpeed = Ogre::StringConverter::parseReal(mProperties.getValue("speed", 0, "2"));

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
    initBody();

    //First point is here.
    mPoints.push_back(pos);

    //If we're a simple slider, we just move in one direction till a point and then return.
    Ogre::Real distance = Ogre::StringConverter::parseReal(mProperties.getValue("distance", 0, "n"));
    if (distance != 0.0f)
        mPoints.push_back(pos + (rot * Ogre::Vector3(0,0,-distance)));
    
    mIgnoreCollisions = Ogre::StringConverter::parseBool(mProperties.getValue("ignoreCollisions", 0, "no"));
    //mIgnoreCollisions = true;

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
void SlidingBrush::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
SlidingBrush::~SlidingBrush()
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
void SlidingBrush::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    if (mEnabled)
    {
        //Get old place (current place actually).
        btTransform oldTrans;
        mBody->getMotionState()->getWorldTransform(oldTrans);
        btVector3 prevPos = oldTrans.getOrigin();

        //Find next point position, and velocity.
        int currPlace = mForward ? mCurrentPlace + 0.5 : mCurrentPlace -0.5;
        Ogre::Vector3 currPoint = mPoints[currPlace];
        Ogre::Vector3 velocity = (currPoint - BtOgre::Convert::toOgre(prevPos)).normalisedCopy() * mSpeed;

        //Check if we're near next point.
        Ogre::Real sqSpeed = mSpeed * mSpeed * evt.timeSinceLastFrame * evt.timeSinceLastFrame;
        Ogre::Real sqDist = (currPoint - BtOgre::Convert::toOgre(prevPos)).squaredLength();

        if (sqDist < sqSpeed)
        {
            //If near, jump to it.
            mBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(currPoint)));
            mNode->setPosition(currPoint);

            //If next 'place' exists, get there..
            unsigned int nextPlace  = currPlace + (mForward ? 1 : -1);
            if (nextPlace >= 0 && nextPlace < mPoints.size())
                mCurrentPlace += mForward ? 1 : -1;

            //Call the Python 'point' event. Cut out repeated events using a 'last place' idea.
            //if (mLastPlace != currPlace)
                NGF_PY_CALL_EVENT(point, currPlace);
            mLastPlace = currPlace;
        }
        else
        {
            //Otherwise, usual velocity movement.
            bool canMove = true;
            btVector3 currVel = BtOgre::Convert::toBullet(velocity) * evt.timeSinceLastFrame;
            btVector3 newPos = prevPos + currVel;

            //If we're not ignoring collisions, check for 'em.
            if (!mIgnoreCollisions)
            {
                //The cast result callback.
                struct SlidingBrushCheckResult : public btDynamicsWorld::ConvexResultCallback
                {
                    btCollisionObject *mIgnore;
                    int mDimension;
                    bool mHit;
                    Ogre::Real mHitFraction;
                    bool mYCast;

                    SlidingBrushCheckResult(btCollisionObject *ignore, int dimension, bool yCast)
                        : mIgnore(ignore),
                        mDimension(dimension),
                        mHit(false),
                        mHitFraction(1),
                        mYCast(yCast)
                    {
                    }

                    btScalar addSingleResult(btDynamicsWorld::LocalConvexResult &convexResult, bool)
                    {
                        mHit = true;
                        mHitFraction = convexResult.m_hitFraction < mHitFraction ? convexResult.m_hitFraction : mHitFraction;
                        return convexResult.m_hitFraction;
                    }

                    bool needsCollision(btBroadphaseProxy* proxy0) const
                    {
                        return ((btCollisionObject*) proxy0->m_clientObject != mIgnore) //Shouldn't be us.
                            && (proxy0->m_collisionFilterGroup & mDimension) //Should be in our dimension.
                            && (!(proxy0->m_collisionFilterGroup & DimensionManager::STATIC)) //No need to check with static.
                            && !(mYCast && (proxy0->m_collisionFilterGroup & DimensionManager::LIFTABLE)) //If moving up, forget the liftables.
                            && !(((btCollisionObject*) proxy0->m_clientObject)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE); //If no contact response, ignore.
                    }
                };

                //Where to cast from, where to cast to, etc.
                btVector3 normVel = currVel.normalized();
                btVector3 pos1 = prevPos;
                btVector3 pos2 = prevPos + currVel + (normVel * CAST_SHAPE_SHRINK);
                btQuaternion rot = mBody->getWorldTransform().getRotation();
                btTransform trans1(rot, pos1);
                btTransform trans2(rot, pos2);

                //Do the cast.
                SlidingBrushCheckResult res(mBody, mDimensions, GlbVar.gravMgr->isUp() ? pos1.y() < pos2.y() : pos1.y() > pos2.y());
                GlbVar.phyWorld->convexSweepTest(mCastShape, trans1, trans2, res);

                //If hit, don't move.
                if (res.mHit)
                {
                    goto skip;
                }
            }

            if (canMove)
            {
                oldTrans.setOrigin(newPos);
                mBody->getMotionState()->setWorldTransform(oldTrans);
                mNode->setPosition(BtOgre::Convert::toOgre(newPos));
            }
        }
    }

    skip:
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void SlidingBrush::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply SlidingBrush::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_SETFORWARD:
            mForward = msg.getParam<bool>(0);
            NGF_NO_REPLY();

        case MSG_SETENABLED:
            mEnabled = msg.getParam<bool>(0);
            NGF_NO_REPLY();
    }
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void SlidingBrush::collide(NGF::GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //The PythonGameObject.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);

    //Python collision event.
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(SlidingBrush)
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
    NGF_PY_PROPERTY_IMPL(forward, mForward, bool);
    NGF_PY_PROPERTY_IMPL(speed, mSpeed, Ogre::Real);

    }
    //NGF_PY_END_IMPL_BASE(GraLL2GameObject)
        }
    }

    if (mPythonEvents.has_key(NGF_name))
    {
        return (NGF::Python::PythonManager::getSingleton().getMainNamespace()["callFunc"]) 
            (mPythonEvents, NGF_name, mConnector, args);   
    }

    return GraLL2GameObject::pythonMethod(NGF_name, args);
}
