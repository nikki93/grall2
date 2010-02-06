/*
=========================================
Crate.cpp
=========================================
*/

#define __CRATE_CPP__

#include "Objects/Obstacles/Crate.h"

#define MAX_CRATE_MATERIALS 6
#define CRATE_MOVE_SPEED 2

//--- NGF events ----------------------------------------------------------------
Crate::Crate(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mMoving(false),
      mExploded(false)
{
    addFlag("Crate");
    addFlag("Switcher");

    Ogre::String script = mProperties.getValue("script", 0, "");

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

    //Create the Physics stuff. The mesh is a normal box, except in the bottom it's bevelled.
    Ogre::Entity *colMesh = GlbVar.ogreSmgr->createEntity(mOgreName + "_Collision", "Collision_Crate.mesh");
    BtOgre::StaticMeshToShapeConverter converter(colMesh);
    mShape = converter.createConvex();
    GlbVar.ogreSmgr->destroyEntity(colMesh);

    mShape->setMargin(0);
    btScalar mass = 7;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);

    mBody = new btRigidBody(info);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    initBody( DimensionManager::LIFTABLE
            );

    //To allow Gravity, but still constraint on XZ plane, we use slider.
    mCastShape = new btBoxShape(btVector3(0.475,0.65,0.475));

    btDefaultMotionState *fixedState = new btDefaultMotionState(btTransform(BtOgre::Convert::toBullet(rot), BtOgre::Convert::toBullet(pos + Ogre::Vector3(0,20,0))));
    mFixedBody = new btRigidBody(0, fixedState, mCastShape);
    mFixedBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mFixedBody->setActivationState(DISABLE_DEACTIVATION);
    GlbVar.phyWorld->addRigidBody(mFixedBody, mDimensions 
            | DimensionManager::NO_DIM_CHECK 
            | DimensionManager::NO_MOVING_CHECK 
            | DimensionManager::NO_BULLET_HIT 
            | DimensionManager::NO_CRATE_CHECK 
            | DimensionManager::INVISIBLE
            , mDimensions);

    mConstraint = new btSliderConstraint(*mBody, *mFixedBody, btTransform(btQuaternion(btVector3(0,0,1),Ogre::Math::PI/2.0)), btTransform(btQuaternion(btVector3(0,0,1),Ogre::Math::PI/2.0)), false);
    mConstraint->setLowerLinLimit(1); //Free linear.
    mConstraint->setUpperLinLimit(0);
    mConstraint->setLowerAngLimit(0); //Locked angular.
    mConstraint->setUpperAngLimit(0);
    //mConstraint->setRestitutionOrthoLin(3);

    mBody->setAngularFactor(btVector3(0,0,0));
    mBody->setLinearFactor(btVector3(0,1,0));

    //GlbVar.phyWorld->addConstraint(mConstraint, true);

    //Height deficiency, for some variety in Crates.
    mHeight = 1.5 - heightDef;
    if (heightDef != 0)
    {
        Ogre::Real scale = mHeight / 1.5;
        mNode->setScale(Ogre::Vector3(1,scale,1));
        mShape->setLocalScaling(btVector3(1,scale,1));
        mCastShape->setLocalScaling(btVector3(1,scale,1));
    }

    //Crate materials, again for variety.
    int n = Ogre::Math::Floor(Ogre::Math::RangeRandom(1, MAX_CRATE_MATERIALS + 0.99));
    mEntity->setMaterialName("Objects/Crate" + Ogre::StringConverter::toString(n));

    //Sound.
    mSound = GlbVar.soundMgr->createSound(mOgreName + "_Sound", "CrateMove.ogg", true, false);
    mNode->attachObject(mSound);
    mSound->setReferenceDistance(1.2);
    mSound->setGain(2.1);
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

    GlbVar.soundMgr->destroySound(mSound);

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
    if (mMoving)
    {
        btTransform oldTrans, bodyTrans;
        mFixedBody->getMotionState()->getWorldTransform(oldTrans);
        mBody->getMotionState()->getWorldTransform(bodyTrans);

        Ogre::Real speed = CRATE_MOVE_SPEED * evt.timeSinceLastFrame;
        Ogre::Vector3 currPos = BtOgre::Convert::toOgre(oldTrans.getOrigin());

        if (currPos.squaredDistance(mTarget) < speed*speed)
        {
            //If next move'll take us overboard, just jump to the target.
            mFixedBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(mTarget)));
            mMoving = false;
        }
        else
        {
            //Else move toward target.
            btVector3 vel = BtOgre::Convert::toBullet((mTarget - currPos).normalisedCopy() * speed);
            mFixedBody->getMotionState()->setWorldTransform(btTransform(oldTrans.getRotation(), oldTrans.getOrigin() + vel));
        }

        //Update the body itself. Y = Y of body X,Z = X,Z of fixed body.
        mFixedBody->getMotionState()->getWorldTransform(oldTrans);
        mBody->setWorldTransform(btTransform(oldTrans.getRotation(), btVector3(oldTrans.getOrigin().x(), bodyTrans.getOrigin().y(), oldTrans.getOrigin().z())));

        if (!mSound->isPlaying())
            mSound->play();
    }
    else if (mSound->isPlaying())
        mSound->stop();

    //We might fall of.
    checkFell();
    
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
    switch (msg.code)
    {
        case MSG_EXPLODE:
            explode();
            NGF_NO_REPLY();
    }
    
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Crate::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //Only if not moving, pushed py Player, and standing on something, do we move.
    if (!mMoving && other->hasFlag("Player")
			&& !isPlaceFree(Ogre::Vector3(0,GlbVar.gravMgr->getSign() * -0.25,0), true))
    {
        Ogre::Vector3 playerPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETPOSITION));
        btTransform myTrans; mBody->getMotionState()->getWorldTransform(myTrans);
        Ogre::Vector3 myPos = BtOgre::Convert::toOgre(myTrans.getOrigin());

        //We have to go the other w, not toward player, so subtract this way.
        Ogre::Vector3 push = myPos - playerPos;

        //Check that we got hit on side and not up or (lol?) below. :-)
        if (Ogre::Math::Abs(push.y) < (mHeight / 2.0))
        {
            Ogre::Vector3 dir = Ogre::Vector3(0, 0, Ogre::Math::Sign(push.z));
            if (Ogre::Math::Abs(push.x) > Ogre::Math::Abs(push.z))
                dir = Ogre::Vector3(Ogre::Math::Sign(push.x), 0, 0);

            Ogre::Vector3 force = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(other, NGF_MESSAGE(MSG_GETCRATEFORCE));

            if (dir.dotProduct(force) > 2)
            {
                makeMove(dir, false);
                btVector3 currVel = mBody->getLinearVelocity();
                currVel.setY(0);
                mBody->setLinearVelocity(currVel);
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
void Crate::makeMove(const Ogre::Vector3 &dir, bool fix)
{
    //If we're already moving, we continue.
    if (mMoving)
        return;

    Ogre::Vector3 newDir = dir;

    //If x dir greater, move on x, else on z.
    if (fix)
        if (Ogre::Math::Abs(dir.x) > Ogre::Math::Abs(dir.z))
            newDir = Ogre::Vector3(Ogre::Math::Sign(dir.x), 0, 0);
        else
            newDir = Ogre::Vector3(0, 0, Ogre::Math::Sign(dir.z));

    //If free, move.
    if (isPlaceFree(newDir))
    {
        mBody->activate();
        mMoving = true;

        //Calculate new target by adding direction.
        btTransform fixedTrans;
        mFixedBody->getMotionState()->getWorldTransform(fixedTrans);
        btVector3 fixedPos = fixedTrans.getOrigin();
        //btVector3 myPos = myTrans.getOrigin();
        mTarget = BtOgre::Convert::toOgre(fixedPos) + newDir;
    }
}
//-------------------------------------------------------------------------------
void Crate::explode()
{
    if (mExploded)
        return;

    //FX!
    Util::createExplosion(mNode->getPosition());

    //Us no more. :-(
    GlbVar.goMgr->requestDestroy(getID());
    mExploded = true;
}
//-------------------------------------------------------------------------------
bool Crate::isPlaceFree(const Ogre::Vector3 &dir, bool ignorePlayer)
{
    //The cast result callback.
    struct CrateCheckResult : public btDynamicsWorld::ConvexResultCallback
    {
        btCollisionObject *mIgnore;
        int mDimension;
        bool mHit, mIgnorePlayer;

        CrateCheckResult(btCollisionObject *ignore, int dimension, bool ignorePlayer)
            : mIgnore(ignore),
            mDimension(dimension),
            mHit(false),
            mIgnorePlayer(ignorePlayer)
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
                && !(proxy0->m_collisionFilterGroup & DimensionManager::NO_CRATE_CHECK)
                && !(mIgnorePlayer && proxy0->m_collisionFilterGroup & DimensionManager::PLAYER)
                && (proxy0->m_collisionFilterGroup & mDimension);
                //&& !(((btCollisionObject*) proxy0->m_clientObject)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    };

    //Where to cast from, where to cast to, etc.
    btTransform myTrans;
    mBody->getMotionState()->getWorldTransform(myTrans);
    btVector3 pos1 = myTrans.getOrigin();
    btVector3 pos2 = myTrans.getOrigin() + BtOgre::Convert::toBullet(dir);
    btQuaternion rot = myTrans.getRotation();
    btTransform trans1(rot, pos1);
    btTransform trans2(rot, pos2);

    //Do the cast.
    CrateCheckResult res(mBody, mDimensions, ignorePlayer);
    GlbVar.phyWorld->convexSweepTest(mCastShape, trans1, trans2, res);

    //If not hit, return true.
    return !(res.mHit);
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
    NGF_PY_METHOD_IMPL(explode)
    {
        explode();
        NGF_PY_RETURN();
    }
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
