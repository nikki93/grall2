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
      mMoving(false)
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
    btScalar mass = 100;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);
    info.m_friction = 0;

    mBody = new btRigidBody(info);
    initBody();

    //To allow Gravity, but still constraint on XZ plane, we use slider.
    mCastShape = new btBoxShape(btVector3(0.475,0.70,0.475));

    btDefaultMotionState *fixedState = new btDefaultMotionState(btTransform(BtOgre::Convert::toBullet(rot), BtOgre::Convert::toBullet(pos + Ogre::Vector3(0,5,0))));
    mFixedBody = new btRigidBody(0, fixedState, mCastShape);
    mFixedBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mFixedBody->setActivationState(DISABLE_DEACTIVATION);
    GlbVar.phyWorld->addRigidBody(mFixedBody, mDimensions | DimensionManager::NO_DIM_CHECK, mDimensions);

    mConstraint = new btSliderConstraint(*mBody, *mFixedBody, btTransform(btQuaternion(btVector3(0,0,1),Ogre::Math::PI/2.0)), btTransform(btQuaternion(btVector3(0,0,1),Ogre::Math::PI/2.0)), false);
    mConstraint->setLowerLinLimit(1); //Free linear.
    mConstraint->setUpperLinLimit(0);
    mConstraint->setLowerAngLimit(0); //Locked angular.
    mConstraint->setUpperAngLimit(0);
    //mConstraint->setRestitutionOrthoLin(3);

    GlbVar.phyWorld->addConstraint(mConstraint, true);

    //Height deficiency, for some variety in Crates.
    mHeight = 1.5 - heightDef;
    if (heightDef != 0)
    {
        Ogre::Real scale = mHeight / 1.5;
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
        Ogre::Vector3 currPos = BtOgre::Convert::toOgre(oldTrans.getOrigin());

        if (currPos.squaredDistance(mTarget) < speed*speed)
        {
            //If next move'll take us overboard, just jump to the target.
            mFixedBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(mTarget)));
            btTransform trans = mBody->getWorldTransform();
            mBody->setWorldTransform(btTransform(oldTrans.getRotation(), btVector3(mTarget.x, trans.getOrigin().y(), mTarget.z)));
            mMoving = false;
        }
        else
        {
            //Else move toward target.
            btVector3 vel = BtOgre::Convert::toBullet(Ogre::Vector3(speed,0,0).getRotationTo(mTarget - currPos) * Ogre::Vector3(speed,0,0));
            mFixedBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), oldTrans.getOrigin() + vel));
        }
    }

    /*
    mFixedBody->getMotionState()->getWorldTransform(oldTrans);
    mFixedBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), btVector3(oldTrans.getOrigin().x(), 
                    mBody->getWorldTransform().getOrigin().y(), oldTrans.getOrigin().z())));
    */

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
        if (Ogre::Math::Abs(push.y) < (mHeight / 2.0))
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
            //If it's us, or isn't in our dimension, we don't care.
            return ((btCollisionObject*) proxy0->m_clientObject != mIgnore) 
                && !(proxy0->m_collisionFilterGroup & DimensionManager::GREENBOMB) //So we can blast 'em green ones!
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
    GlbVar.phyWorld->convexSweepTest(mCastShape, trans1, trans2, res);

    //If not hit, move!
    if (!res.mHit)
    {
        mBody->activate();
        mMoving = true;

        //Calculate new target by adding direction. Use fixed body's X and Z values to add to to maintain grid-stability.
        btTransform fixedTrans;
        mFixedBody->getMotionState()->getWorldTransform(fixedTrans);
        btVector3 fixedPos = fixedTrans.getOrigin();
        btVector3 myPos = myTrans.getOrigin();
        //mTarget = Ogre::Vector3(fixedPos.x(), fixedPos.y(), fixedPos.z()) + newDir;
        mTarget = BtOgre::Convert::toOgre(fixedPos) + newDir;
        //GlbVar.console->print("Current fixed position: " + Ogre::StringConverter::toString(BtOgre::Convert::toOgre(fixedPos)) + ", New target: " + Ogre::StringConverter::toString(mTarget) + "\n");
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
