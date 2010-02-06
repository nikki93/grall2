/*
=========================================
Turret.cpp
=========================================
*/

#define __TURRET_CPP__

#include "Objects/Bots/Turret.h"

#include "Objects/Bots/Bullet.h"

#define SHOOT_OFFSET (Ogre::Vector3(0,0.89,0))
#define TOP_MOVE_TIME 1.0f
#define BULLET_TIME 0.14
#define MAX_BULLET_DEVIATION_ANGLE 0.08
#define SHOOT_SOUND_PITCH_RANGE 1.6, 2.2

#define SOUND_MOVE "TurretMove.wav"
#define SOUND_SHOOT "TurretShoot.ogg"

//'TURRET_TOP_HEIGHT', 'TURRET_FIRST_BULLET_TIME' in Turret.h.

//--- NGF events ----------------------------------------------------------------
Turret::Turret(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("Turret");                                      

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Read properties.
    bool enabled = Ogre::StringConverter::parseBool(mProperties.getValue("enabled", 0, "yes"));
    mRadius = Ogre::StringConverter::parseReal(mProperties.getValue("radius", 0, "20"));

    Ogre::Real initTime = Ogre::StringConverter::parseReal(mProperties.getValue("initTime", 0, "0"));
    mFireTime = Ogre::StringConverter::parseReal(mProperties.getValue("fireTime", 0, "7"));
    mRestTime = Ogre::StringConverter::parseReal(mProperties.getValue("restTime", 0, "5"));

    bool initScan = Ogre::StringConverter::parseBool(mProperties.getValue("initScan", 0, "no"));
    mAlwaysScan = Ogre::StringConverter::parseBool(mProperties.getValue("alwaysScan", 0, "no"));

    //Create the Ogre stuff.
    mBaseEntity = GlbVar.ogreSmgr->createEntity(mOgreName + "_base", "Mesh_TurretBase.mesh");
    mTopEntity = GlbVar.ogreSmgr->createEntity(mOgreName + "_top", "Mesh_TurretTop.mesh");
    mBaseEntity->setMaterialName("Objects/TurretBase");
    mTopEntity->setMaterialName("Objects/TurretTop");

    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mBaseNode = mNode->createChildSceneNode(mOgreName + "_base");
    mTopNode = mNode->createChildSceneNode(mOgreName + "_top", Ogre::Vector3(0,-TURRET_TOP_HEIGHT,0));

    mBaseNode->attachObject(mBaseEntity);
    mTopNode->attachObject(mTopEntity);

    //Create the Physics stuff.
    Ogre::Entity *colMesh = GlbVar.ogreSmgr->createEntity(mOgreName + "_collision", "Collision_Turret.mesh");
    BtOgre::StaticMeshToShapeConverter converter(colMesh);
    mShape = converter.createTrimesh();
    GlbVar.ogreSmgr->destroyEntity(colMesh);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape);

    initBody( DimensionManager::NO_BULLET_HIT
            );
    setBulletObject(mBody);

    //Create sounds.
    mShootSound = GlbVar.soundMgr->createSound(mOgreName + "_shootSound", SOUND_SHOOT);
    mNode->attachObject(mShootSound);
    mShootSound->setReferenceDistance(1.2);
    mShootSound->setGain(1.2);

    mMoveSound = GlbVar.soundMgr->createSound(mOgreName + "_moveSound", SOUND_MOVE);
    mNode->attachObject(mMoveSound);
    mMoveSound->setReferenceDistance(1.2);
    mMoveSound->setGain(1.7);

    //Initialise states.
    NGF_STATES_INIT();

    NGF_STATES_INIT_STATE(Fire);
    NGF_STATES_INIT_STATE(Rest);
    NGF_STATES_INIT_STATE(Scan);
    NGF_STATES_INIT_STATE(Disabled);
    NGF_STATES_INIT_STATE(MoveDown);
    NGF_STATES_INIT_STATE(MoveUp);

    //Start!
    if (initScan)
        scan();
    else if (initTime)
        rest(initTime);
    else
        fire(mFireTime);
    if (!enabled)
        disable();
}
//-------------------------------------------------------------------------------
void Turret::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Turret::~Turret()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    destroyBody();
    delete mShape;

    //Destroy sounds.
    GlbVar.soundMgr->destroySound(mShootSound);
    GlbVar.soundMgr->destroySound(mMoveSound);

    //Remove Ogre SceneNodes and Entities.
    mNode->detachAllObjects();
    mTopNode->detachAllObjects();
    mBaseNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyEntity(mTopEntity->getName());
    GlbVar.ogreSmgr->destroyEntity(mBaseEntity->getName());
    GlbVar.ogreSmgr->destroySceneNode(mBaseNode);
    GlbVar.ogreSmgr->destroySceneNode(mTopNode);
}
//-------------------------------------------------------------------------------
void Turret::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Update state.
    NGF_STATES_UPDATE();
    if (mCurrState)
        mCurrState->unpausedTick(evt);

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Turret::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Turret::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Turret::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Fire ----------------------------------------------------------------------
void Turret::Fire::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Time flies!
    mObj->mTime -= evt.timeSinceLastFrame;

    if (mObj->mTime > 0)
    {
        //If no Player (we did it!), then we stop after 1 second.
        if (!GlbVar.player && mObj->mTime > 1)
            mObj->mTime = 1;

        mObj->mBulletTime -= evt.timeSinceLastFrame;

        if (mObj->mBulletTime < 0)
        {
            mObj->fireSingleBullet();
            mObj->mBulletTime = BULLET_TIME + Ogre::Math::RangeRandom(-0.07,0.07);
        }
    }
    else
    {
        //If no time left switch to the next state (scan or rest, depends).
        if (mObj->mAlwaysScan)
            mObj->scan();
        else
            mObj->rest(mObj->mRestTime);
    }
}
//-------------------------------------------------------------------------------

//--- Rest ----------------------------------------------------------------------
void Turret::Rest::unpausedTick(const Ogre::FrameEvent &evt)
{
    if (mObj->mTime > 0)
        mObj->mTime -= evt.timeSinceLastFrame;
    else
        if (mObj->playerIsInRadius())
            mObj->fire(mObj->mFireTime);
}
//-------------------------------------------------------------------------------

//--- Scan ----------------------------------------------------------------------
void Turret::Scan::unpausedTick(const Ogre::FrameEvent &)
{
    if (mObj->playerIsInRadius() && mObj->doSingleScan())
        mObj->fire(mObj->mFireTime);
}
//-------------------------------------------------------------------------------

//--- MoveDown ------------------------------------------------------------------
void Turret::MoveDown::unpausedTick(const Ogre::FrameEvent &evt)
{
    Ogre::Real speed = (TURRET_TOP_HEIGHT / TOP_MOVE_TIME) * evt.timeSinceLastFrame;

    if (mObj->mTopNode->getPosition().y + TURRET_TOP_HEIGHT < speed)
    {
        mObj->mTopNode->setPosition(Ogre::Vector3(0,-TURRET_TOP_HEIGHT,0));
        NGF_STATES_CONTAINER_POP_STATE();
    }
    else
        mObj->mTopNode->translate(Ogre::Vector3(0,-speed,0));
}
//-------------------------------------------------------------------------------

//--- MoveUp ------------------------------------------------------------------
void Turret::MoveUp::unpausedTick(const Ogre::FrameEvent &evt)
{
    Ogre::Real speed = (TURRET_TOP_HEIGHT / TOP_MOVE_TIME) * evt.timeSinceLastFrame;

    if (-(mObj->mTopNode->getPosition().y) < speed)
    {
        mObj->mTopNode->setPosition(Ogre::Vector3::ZERO);
        NGF_STATES_CONTAINER_POP_STATE();
    }
    else
        mObj->mTopNode->translate(Ogre::Vector3(0,speed,0));
}
//-------------------------------------------------------------------------------

//--- State switches ------------------------------------------------------------
void Turret::fire(Ogre::Real time)
{
    NGF_STATES_CLEAR_STACK();
    mTime = time;
    NGF_STATES_PUSH_STATE(Fire);
}
//-------------------------------------------------------------------------------
void Turret::rest(Ogre::Real time)
{
    NGF_STATES_CLEAR_STACK();
    mTime = time;
    NGF_STATES_PUSH_STATE(Rest);
}
//-------------------------------------------------------------------------------
void Turret::scan()
{
    NGF_STATES_CLEAR_STACK();
    NGF_STATES_PUSH_STATE(Scan);
}
//-------------------------------------------------------------------------------
void Turret::disable()
{
    NGF_STATES_PUSH_STATE(Disabled);
}
//-------------------------------------------------------------------------------
void Turret::enable()
{
    if (NGF_STATES_CURRENT_STATE(Disabled))
        NGF_STATES_POP_STATE(); //Will resume whatever was the previous state.
}
//-------------------------------------------------------------------------------

//--- Some helper functions -----------------------------------------------------
void Turret::fireSingleBullet()
{
    if (GlbVar.player)
    {
        Ogre::Vector3 playerPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(GlbVar.player, NGF_MESSAGE(MSG_GETPOSITION));
        Ogre::Vector3 shootPos = mNode->getPosition() + SHOOT_OFFSET;

        //Get the direction, then deviate a bit or we'll get complaints that Turrets are too accurate.
        Ogre::Vector3 dir = (playerPos - shootPos).normalisedCopy();
        dir = dir.randomDeviant(Ogre::Radian(Ogre::Math::UnitRandom() * MAX_BULLET_DEVIATION_ANGLE));

        //We're gonna use 30 degree limit. sin is 0.5, cos is 0.866.
#define COS_MAX_ANG 0.866
#define SIN_MAX_ANG 0.5
        if (Ogre::Math::Abs(dir.y) > SIN_MAX_ANG)
        {
            //Make length of XZ part equal to cosine by scaling it down.
            Ogre::Real fact = Ogre::Math::Sqrt((dir.x * dir.x + dir.z * dir.z) 
                    / (COS_MAX_ANG * COS_MAX_ANG));
            dir.x /= fact; 
            dir.z /= fact;

            //Make y part the sine.
            dir.y = dir.y < 0 ? -SIN_MAX_ANG : SIN_MAX_ANG;

            //Well this shouldn't be needed actually (cos^2 + sin^2 = 1). Anyway.
            //dir.normalise();
        }

        Ogre::Quaternion bulletRot = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo(dir);
        Ogre::Vector3 bulletPos = shootPos + (bulletRot * Ogre::Vector3(0,0,-0.25)); //We make the bullet a little bit in it's direction.

        //Make the bullet.
        GlbVar.goMgr->createObject<Bullet>(bulletPos, bulletRot, NGF::PropertyList::create
                ("dimensions", Ogre::StringConverter::toString(mDimensions))
                );

        //Play the sound.
        mShootSound->stop();
        mShootSound->setPitch(Ogre::Math::RangeRandom(SHOOT_SOUND_PITCH_RANGE));
        mShootSound->play();
    }
}
//-------------------------------------------------------------------------------
bool Turret::doSingleScan()
{
    if (GlbVar.player)
    {
        Ogre::Vector3 playerPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(GlbVar.player, NGF_MESSAGE(MSG_GETPOSITION));

        struct ScanRayResult : public btDynamicsWorld::ClosestRayResultCallback
        {
            int mDimension;
            btCollisionObject *mIgnore;

            ScanRayResult(int dimension, btCollisionObject *ignore, const btVector3 &from, const btVector3 &to)
                : btCollisionWorld::ClosestRayResultCallback(from, to),
                  mDimension(dimension),
                  mIgnore(ignore)
            {
            }

            bool needsCollision(btBroadphaseProxy* proxy0) const
            {
                return ((btCollisionObject*) proxy0->m_clientObject != mIgnore)
                    && !(proxy0->m_collisionFilterGroup & DimensionManager::NO_BULLET_HIT)
                    && (proxy0->m_collisionFilterGroup & mDimension);
            }
        };

        btVector3 pos1 = BtOgre::Convert::toBullet(mNode->getPosition() + SHOOT_OFFSET);
        btVector3 pos2 = BtOgre::Convert::toBullet(playerPos);
        ScanRayResult res(mDimensions, mBody, pos1, pos2);

        GlbVar.phyWorld->rayTest(pos1, pos2, res);

        return res.m_collisionObject && (res.m_collisionObject->getBroadphaseHandle()->m_collisionFilterGroup & DimensionManager::PLAYER);
    }
    
    return false;
}
//-------------------------------------------------------------------------------
bool Turret::playerIsInRadius()
{
    if (GlbVar.player)
    {
        Ogre::Vector3 ourPos = mNode->getPosition();
        Ogre::Vector3 playerPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(GlbVar.player, NGF_MESSAGE(MSG_GETPOSITION));

        return ourPos.squaredDistance(playerPos) < mRadius * mRadius;
    }

    return false;
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Turret)
{
    NGF_PY_METHOD_IMPL(fire)
    {
        fire(mFireTime);
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(stop)
    {
        if (mAlwaysScan)
            scan();
        else
            rest(mRestTime);
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(enable)
    {
        enable();
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(disable)
    {
        disable();
        NGF_PY_RETURN();
    }

    NGF_PY_PROPERTY_IMPL(radius, mRadius, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(fireTime, mFireTime, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(restTime, mRestTime, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(alwaysScan, mAlwaysScan, bool);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
//-------------------------------------------------------------------------------
