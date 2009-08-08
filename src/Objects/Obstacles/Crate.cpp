/*
=========================================
Crate.cpp
=========================================
*/

#define __CRATE_CPP__

#include "Objects/Obstacles/Crate.h"

#define MAX_CRATE_MATERIALS 3
#define CRATE_MOVE_SPEED 2

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
    mShape = new btBoxShape(btVector3(0.475,0.75,0.475));
    btScalar mass = 10;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(mass, state, mShape, inertia);
    initBody();

    //To allow Gravity, but still constraint on XZ plane, we use slider.
    mShape2 = new btBoxShape(*mShape);
    mShape2->setLocalScaling(btVector3(0.85,0.85,0.85));

    btDefaultMotionState *fixedState = new btDefaultMotionState(btTransform(BtOgre::Convert::toBullet(rot), BtOgre::Convert::toBullet(pos + Ogre::Vector3(0,20,0))));
    mFixedBody = new btRigidBody(0, fixedState, mShape2);
    mFixedBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mFixedBody->setActivationState(DISABLE_DEACTIVATION);
    GlbVar.phyWorld->addRigidBody(mFixedBody, mDimensions | DimensionManager::NO_DIM_CHECK, mDimensions);

    mConstraint = new btSliderConstraint(*mBody, *mFixedBody, btTransform(btQuaternion(btVector3(0,0,1),Ogre::Math::PI/2.0)), btTransform(btQuaternion(btVector3(0,0,1),Ogre::Math::PI/2.0)), false);
    mConstraint->setLowerLinLimit(1); //Free linear.
    mConstraint->setUpperLinLimit(0);
    mConstraint->setLowerAngLimit(0); //Locked angular.
    mConstraint->setUpperAngLimit(0);
    mConstraint->setDampingDirLin(0.5);
    mConstraint->setDampingLimLin(1);
    mConstraint->setRestitutionOrthoLin(0);

    GlbVar.phyWorld->addConstraint(mConstraint, true);

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
    GlbVar.phyWorld->removeConstraint(mConstraint);
    delete mConstraint;
    GlbVar.phyWorld->removeRigidBody(mFixedBody);
    delete mFixedBody;
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
    mFixedBody->getMotionState()->getWorldTransform(oldTrans);

    if (mMoving)
    {
        Ogre::Real speed = CRATE_MOVE_SPEED * evt.timeSinceLastFrame;

        //Zero out vertical jump.
        /*
        btVector3 currVel = mBody->getLinearVelocity();
        currVel.setY(0);
        mBody->setLinearVelocity(currVel);
        */

        //Damping.
        /*
        if (mDistanceMoved > 0.8)
        {
            speed *= ((1.01 - mDistanceMoved) / 0.2);
        }
        */

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
        mFixedBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), oldTrans.getOrigin() + vel));
    }

    //If fell off, die.
    if (mBody->getWorldTransform().getOrigin().y() < -20)
        GlbVar.goMgr->requestDestroy(getID());
    
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
    if (!other)
        return;

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
            makeMove(push);
            btVector3 currVel = mBody->getLinearVelocity();
            currVel.setY(0);
            mBody->setLinearVelocity(currVel);
        }
    }

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void Crate::makeMove(const Ogre::Vector3 &dir)
{
    //If we're already moving, we continue.
    if (mMoving)
        return;

    //If x dir greater, move on x, else on z.
    Ogre::Vector3 newDir;
    if (Ogre::Math::Abs(dir.x) > Ogre::Math::Abs(dir.z))
        newDir = Ogre::Vector3(Ogre::Math::Sign(dir.x), 0, 0);
    else
        newDir = Ogre::Vector3(0, 0, Ogre::Math::Sign(dir.z));

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
    btTransform myTrans;
    mBody->getMotionState()->getWorldTransform(myTrans);
    btVector3 pos1 = myTrans.getOrigin();
    btVector3 pos2 = myTrans.getOrigin() + BtOgre::Convert::toBullet(newDir);
    btQuaternion rot = myTrans.getRotation();
    btTransform trans1(rot, pos1);
    btTransform trans2(rot, pos2);

    //Do the cast.
    CrateCheckResult res(mBody, mDimensions);
    GlbVar.phyWorld->convexSweepTest(mShape2, trans1, trans2, res);

    //If not hit, move!
    if (!res.mHit)
    {
        mBody->activate();
        mMoving = true;
        mNextDirection = newDir;
    }
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Crate)
{
    NGF_PY_METHOD_IMPL(move)
    {
        if (!mMoving)
        {
            Ogre::Vector3 push = py::extract<Ogre::Vector3>(args[0]);
            makeMove(push);
        }

        NGF_PY_RETURN();
    }
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
