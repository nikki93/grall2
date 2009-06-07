/*
=========================================
Crate.cpp
=========================================
*/

#define __CRATE_CPP__

#include "Objects/Obstacles/Crate.h"

#define MAX_CRATE_MATERIALS 1
#define CRATE_MOVE_SPEED 4

//--- NGF events ----------------------------------------------------------------
Crate::Crate(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mMoving(false),
      mDistanceMoved(0),
      mNextDirection(Ogre::Vector3::ZERO)
{
    addFlag("Crate");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Read properties.
    Ogre::Real heightDef = Ogre::StringConverter::parseReal(mProperties.getValue("heightDeficiency", 0, "0"));

    if(!(mProperties.getValue("NGF_SERIALISED", 0, "no") == "yes"))
        pos.y -= heightDef * 0.5; //If deficient in height, move down by half the deficiency because midpoint is local origin.

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Crate.mesh");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    mShape = new btBoxShape(btVector3(0.5,0.75,0.5));
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);

    mBody = new btRigidBody(0, state, mShape);
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    initBody();

    //Height deficiency, for some variety in Crates.
    mSize = 1.5 - heightDef;
    if (heightDef != 0)
    {
        Ogre::Real scale = mSize / 1.5;
        mNode->setScale(Ogre::Vector3(1,scale,1));
        mShape->setLocalScaling(btVector3(1,scale,1));
    }

    //Crate materials, again for variety.
    int n = Ogre::Math::Floor(Ogre::Math::RangeRandom(1, MAX_CRATE_MATERIALS + 0.99));
    mEntity->setMaterialName("Objects/Crate" + Ogre::StringConverter::toString(n));

    //For convex casts.
    mCastShape = new btBoxShape(*mShape);
    mCastShape->setLocalScaling(btVector3(0.85,0.85,0.85));
}
//-------------------------------------------------------------------------------
void Crate::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Crate::~Crate()
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
void Crate::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //If gotta move, move.
    
    btTransform oldTrans;
    mBody->getMotionState()->getWorldTransform(oldTrans);

    if (mMoving)
    {
        Ogre::Real speed = CRATE_MOVE_SPEED * evt.timeSinceLastFrame;
        btVector3 vel;

        if ((mDistanceMoved + speed) > 1)
        {
            //If next move'll take us overboard, just jump to the target.
            vel = BtOgre::Convert::toBullet(mNextDirection) * (1 - mDistanceMoved); //Whatever's left in that direction is what we have to go by.

            mMoving = false;
            mDistanceMoved = 0;
            mNextDirection = 0;
        }
        else
        {
            //Else move toward target.
            vel = BtOgre::Convert::toBullet(mNextDirection) * speed;
            mDistanceMoved += speed;
        }

        //Apply velocity.
        mBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), oldTrans.getOrigin() + vel));
    }
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Crate::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Crate::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Crate::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!mMoving && other->hasFlag("Player"))
    {
        Ogre::Vector3 playerPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETPOSITION));
        btTransform myTrans; mBody->getMotionState()->getWorldTransform(myTrans);
        Ogre::Vector3 myPos = BtOgre::Convert::toOgre(myTrans.getOrigin());

        //We have to go the other way, not toward player, so subtract this way.
        Ogre::Vector3 push = myPos - playerPos;

        //Check that we got hit on side and not up or (lol?) below. :-)
        if (push.y < (mSize / 2.0))
        {
            //If x push greater, move on x, else on z.
            if (Ogre::Math::Abs(push.x) > Ogre::Math::Abs(push.z))
                mNextDirection = Ogre::Vector3(Ogre::Math::Sign(push.x), 0, 0);
            else
                mNextDirection = Ogre::Vector3(0, 0, Ogre::Math::Sign(push.z));
        }

        //Now, check that the next place is free.

        //The cast result callback.
        struct CrateCheckResult : public btDynamicsWorld::ConvexResultCallback
        {
            btCollisionObject *mIgnore;
            int mDimension;
            bool mHit;

            CrateCheckResult(btCollisionObject *ignore, int dimension)
                : mIgnore(ignore),
                mDimension(dimension),
                mHit(false)
            {
            }

            btScalar addSingleResult(btDynamicsWorld::LocalConvexResult &convexResult, bool)
            {
                //btCollisionObject *obj = convexResult.m_hitCollisionObject;
                //Don't record non-physics objects (Trigger etc.).
                //mHit = !(obj->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
                mHit = true;

                return convexResult.m_hitFraction;
            }

            bool needsCollision(btBroadphaseProxy* proxy0) const
            {
                //If it's us, is the Player, or isn't in our dimension, we don't care.
                return ((btCollisionObject*) proxy0->m_clientObject != mIgnore) 
                    && (proxy0->m_collisionFilterGroup & mDimension)
                    && !(((btCollisionObject*) proxy0->m_clientObject)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
            }
        };

        //Where to cast from, where to cast to, etc.
        btVector3 pos1 = myTrans.getOrigin();
        btVector3 pos2 = myTrans.getOrigin() + BtOgre::Convert::toBullet(mNextDirection);
        btQuaternion rot = mBody->getWorldTransform().getRotation();
        btTransform trans1(rot, pos1);
        btTransform trans2(rot, pos2);

        //Do the cast.
        CrateCheckResult res(mBody, mDimensions);
        GlbVar.phyWorld->convexSweepTest(mCastShape, trans1, trans2, res);

        //If not hit, move!
        mMoving = !res.mHit;
    }

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Crate)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
