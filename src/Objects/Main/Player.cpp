/*
=========================================
Player.cpp
=========================================
*/

#define __PLAYER_CPP__

#include "Objects/Main/Player.h"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#include "Objects/Misc/CameraHandler.h"
#include "Objects/Misc/Light.h"
#include "Objects/Misc/ParticleEffect.h"

#define MAT_AMBIENT 0.9
#define MAT_DIFFUSE 0.9
#define MAT_SPECULAR 0.3

//Makes sure the ghost object stays with us.
class PlayerMotionState : public BtOgre::RigidBodyState
{
    protected:
        btPairCachingGhostObject *mGhostObject;

    public:
        PlayerMotionState(Ogre::SceneNode *node, const btTransform &transform, const btTransform &offset, btPairCachingGhostObject *ghost)
            : BtOgre::RigidBodyState(node, transform, offset),
              mGhostObject(ghost)
        {
            mGhostObject->setWorldTransform(transform);
        }

        PlayerMotionState(Ogre::SceneNode *node, btPairCachingGhostObject *ghost)
            : BtOgre::RigidBodyState(node),
              mGhostObject(ghost)
        {
            mGhostObject->setWorldTransform(mTransform);
        }

        virtual void setWorldTransform(const btTransform &in) 
        {
            mGhostObject->setWorldTransform(in);
            BtOgre::RigidBodyState::setWorldTransform(in); //Let base do it's stuff.
        }
};

//--- NGF events ----------------------------------------------------------------
Player::Player(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mUnderControl(true),
      mDead(false),
      mLight(0)
{
    addFlag("Player");
    addFlag("Switcher");
    addFlag("Doorer");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Do away with the previous fellow (if any).
    if (GlbVar.player)
        GlbVar.goMgr->destroyObject(GlbVar.player->getID());
    GlbVar.player = this;

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Player.mesh");
    mEntity->setMaterialName("Objects/Player");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Read in properties.
    mMinHeight = Ogre::StringConverter::parseReal(mProperties.getValue("minHeight", 0, "-4"));

    //Create the ghost object for dimension tests (we do it before the actual body so that
    //we can pass it to the PlayerMotionState).
    mGhostObject = new btPairCachingGhostObject();

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createSphere();
    btScalar mass = 2;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);
    PlayerMotionState *state = new PlayerMotionState(mNode, mGhostObject);

    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);
    info.m_friction = Ogre::Math::POS_INFINITY;
    info.m_linearDamping = 0.1;
    info.m_angularDamping = 0.6;

    mBody = new btRigidBody(info);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    initBody( DimensionManager::PLAYER
            | DimensionManager::LIFTABLE
            );
    setBulletObject(mBody);

    //Configure the GhostObject.
    mGhostShape = new btSphereShape(converter.getRadius() - 0.01);
    mGhostObject->setCollisionShape(mGhostShape);
    mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    GlbVar.phyWorld->addCollisionObject(mGhostObject
            , DimensionManager::NO_DIM_CHECK 
            | DimensionManager::NO_CRATE_CHECK
            | DimensionManager::NO_MOVING_CHECK
            | DimensionManager::NO_BULLET_HIT
            | DimensionManager::INVISIBLE
            | DimensionManager::DIM_BOTH, 
            DimensionManager::DIM_BOTH
            );

    //Control node is used for controlling the Player. He rotates in all kinds of crazy ways, but we need to know where we're headed.
    mControlNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName + "-controlnode", pos, rot);

    //Player can't be in a dimension that's not being displayed. :P
    setDimension(GlbVar.dimMgr->getCurrentDimension());
}
//-------------------------------------------------------------------------------
void Player::postLoad()
{
    //Do stuff we have to do when not deserialising.
    if (!(mProperties.getValue("NGF_SERIALISED", 0, "no") == "yes"))
    {
        if (!(mProperties.getValue("captureCameraHandler", 0, "yes") == "no"))
            captureCameraHandler();
        mLight = GlbVar.goMgr->createObject<Light>(mNode->getPosition(), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
                ("lightType", "point")
                ("colour", "0 0.3 0.75")
                ("specular", "0.1 0.1 0.1")
                ("attenuation", "10 0.6 0.4 0.6")
                );
        mLight->setPersistent(true); //We'll manage his destruction.
    }

    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Player::~Player()
{
    GlbVar.player = 0;

    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //Fix FOV.
    GlbVar.ogreCamera->setFOVy(Ogre::Degree(45));

    //Destroy the light.
    if (mLight)
        GlbVar.goMgr->destroyObject(mLight->getID());

    //We only clear up stuff that we did.
    GlbVar.phyWorld->removeCollisionObject(mGhostObject);
    delete mGhostObject;
    delete mGhostShape;
    loseCameraHandler();
    destroyBody();
    delete mShape;

    GlbVar.ogreSmgr->destroySceneNode(mControlNode);
    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Player::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Allow stuff to catch up.
    mControlNode->setPosition(mNode->getPosition());

    //Controls.
    if (mBody->getAngularVelocity().length2() < 600)
    {
        Ogre::Vector3 torque = Ogre::Vector3::ZERO;
        torque.x =  isKeyDown(GlbVar.settings.controls.keys["backward"]) - isKeyDown(GlbVar.settings.controls.keys["forward"]);
        torque.z =  isKeyDown(GlbVar.settings.controls.keys["left"]) - isKeyDown(GlbVar.settings.controls.keys["right"]);
        torque *= 6;

        mBody->applyTorque(BtOgre::Convert::toBullet(mControlNode->getOrientation() * torque));
    }

    OIS::MouseState ms = getMouseState();
    mControlNode->yaw(Ogre::Degree(-ms.X.rel * GlbVar.settings.controls.turningSensitivity * 0.4));

    //Fallage.
    Ogre::Real currHeight = mBody->getWorldTransform().getOrigin().getY();
    const Ogre::Real fadeHeight = 2.5;
    if (currHeight < (mMinHeight + fadeHeight))
    {
        Ogre::MaterialPtr mat = mEntity->getSubEntity(0)->getMaterial();
        Ogre::Pass *passAmb = mat->getTechnique(0)->getPass(0);
        Ogre::Pass *passIter = mat->getTechnique(0)->getPass(1);

        if (currHeight < mMinHeight)
        {
            //We fell off! Aaaaah! D: Fix materials, and then die.
            passAmb->setAmbient(Ogre::ColourValue(MAT_AMBIENT,MAT_AMBIENT,MAT_AMBIENT));
            passIter->setDiffuse(Ogre::ColourValue(MAT_DIFFUSE,MAT_DIFFUSE,MAT_DIFFUSE));
            passIter->setSpecular(Ogre::ColourValue(MAT_SPECULAR,MAT_SPECULAR,MAT_SPECULAR));

            die(false);
        }
        else
        {
            //Fade out. We're (supposed to be) the only ones using our material. If you use the Player texture for a Brush
            //and see weird 'world-darkenings' when the Player falls of, we hope you've learnt your lesson.
            Ogre::Real factor = (currHeight - mMinHeight) / fadeHeight;
            Ogre::Real amb =  factor * MAT_AMBIENT;
            Ogre::Real diff = factor * MAT_DIFFUSE;
            Ogre::Real spec = factor * MAT_SPECULAR;

            passAmb->setAmbient(Ogre::ColourValue(amb,amb,amb));
            passIter->setDiffuse(Ogre::ColourValue(diff,diff,diff));
            passIter->setSpecular(Ogre::ColourValue(spec,spec,spec));
        }
    }

    //Move light.
    GlbVar.goMgr->sendMessage(mLight, NGF_MESSAGE(MSG_SETPOSITION, mNode->getPosition()));

    //Player can't be in a dimension that's not being displayed. :P
    setDimension(GlbVar.dimMgr->getCurrentDimension());

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Player::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Player::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_KEYPRESSED:
            if (GlbVar.paused || GlbVar.console->isVisible() || !mUnderControl)
                break;

            {
                OIS::KeyCode key = msg.getParam<OIS::KeyCode>(0);

                if (key == GlbVar.settings.controls.keys["dimensionSwitch"])
                    switchDimension();
                else if (key == GlbVar.settings.controls.keys["selfDestruct"])
                    die(true);
            }
            NGF_NO_REPLY();

        case MSG_GETPOSITION:
            NGF_SEND_REPLY(BtOgre::Convert::toOgre(mBody->getWorldTransform().getOrigin()));

        case MSG_TELEPORT:
            {
                Ogre::Vector3 target = msg.getParam<Ogre::Vector3>(0);
                GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_TELEPORT, target));
                btTransform oldTrans = mBody->getWorldTransform();
                mBody->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(target)));
            }
            NGF_NO_REPLY();

        case MSG_BULLETHIT:
            die(true);
            NGF_NO_REPLY();

        case MSG_CAPTURECAMERAHANDLER:
            captureCameraHandler();
            NGF_NO_REPLY();
    }

    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Player::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //(Much) Less friction if not ground hit.
    Ogre::Vector3 hitPos = BtOgre::Convert::toOgre(contact.getPositionWorldOnA());
    if (mNode->getPosition().y - hitPos.y < (mShape->getRadius() - 0.1))
       contact.m_combinedFriction = 0;

    //Do the GameObject type checks.
    if (other->hasFlag("Explosive"))
    {
        die(true);
        GlbVar.goMgr->sendMessage(other, NGF_MESSAGE(MSG_EXPLODE));
    }
    else if (other->hasFlag("Dangerous"))
    {
        die(false);
    }
    else if (other->hasFlag("Ice"))
    {
        contact.m_combinedFriction = GlbVar.goMgr->sendMessageWithReply<Ogre::Real>(other, NGF_MESSAGE(MSG_GETFRICTIONCOEFF));
    }
    else if (other->hasFlag("Pickup"))
    {
        Ogre::String pickupType = GlbVar.goMgr->sendMessageWithReply<Ogre::String>(other, NGF_MESSAGE(MSG_GETPICKUPTYPE));
        if (pickupType != "NONE")
        {
            ++mPickups[pickupType];
        GlbVar.goMgr->sendMessage(other, NGF_MESSAGE(MSG_PICKEDUP));
        }
    }
    
    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void Player::captureCameraHandler()
{
    //If it isn't already there we create it.
    if (!GlbVar.currCameraHandler)
    {
        GlbVar.currCameraHandler = GlbVar.goMgr->createObject<CameraHandler>(mControlNode->getPosition() + (mControlNode->getOrientation() * Ogre::Vector3(0,9,16)), mControlNode->getOrientation());
        GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETSMOOTHINGFACTOR, Ogre::Real(4)));
    }

    GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETTARGET, mControlNode));
    GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETCAMERASTATE, int(CameraHandler::CS_THIRDPERSON)));
}
//-------------------------------------------------------------------------------
void Player::loseCameraHandler()
{
    //If camera lookin' at us, tell him not to.
    if (GlbVar.currCameraHandler && (GlbVar.goMgr->sendMessageWithReply<Ogre::SceneNode*>(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_GETTARGET)) == mNode))
        GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETCAMERASTATE, int(CameraHandler::CS_NONE)));
}
//-------------------------------------------------------------------------------
void Player::switchDimension()
{
    //If the other dimension isn't free at our place, then abort.

    /* v1: Do a cast.
    //Collects only the results that matter.
    struct PlayerDimensionCheckResult : public btDynamicsWorld::ConvexResultCallback
    {
        btCollisionObject *mIgnore;
        int mOppDimension;
        std::set<btCollisionObject*> mHits;
        btVector3 mFrom, mTo;
        bool mHit;

        PlayerDimensionCheckResult(btCollisionObject *ignore, int oppDimension, btVector3 from, btVector3 to)
            : mIgnore(ignore),
            mOppDimension(oppDimension),
            mFrom(from),
            mTo(to)
        {
        }

        btScalar addSingleResult(btDynamicsWorld::LocalConvexResult &convexResult, bool)
        {
            btCollisionObject *obj = convexResult.m_hitCollisionObject;
            btCollisionShape *shape = obj->getCollisionShape();

            //If triangle mesh, check if 'inside'.
            if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
            {
                //(do inside-check here)
                mHit = true;
                mHits.insert(obj);
            }
            else
            {
                mHit = true;
                mHits.insert(obj);
            }

            return convexResult.m_hitFraction;
        }

        bool needsCollision(btBroadphaseProxy* proxy0) const
        {
            //If it's the Player, doesn't want dimension checking or isn't in the other dimension, we don't care.
            return ((btCollisionObject*) proxy0->m_clientObject != mIgnore) 
                && (!(proxy0->m_collisionFilterGroup & DimensionManager::NO_DIM_CHECK))
                && (proxy0->m_collisionFilterGroup & mOppDimension);
        }
    };

    //Where to cast from, where to cast to, etc.
    btVector3 pos1 = mBody->getWorldTransform().getOrigin() - btVector3(0,0.01,0);
    btVector3 pos2 = btVector3(pos1.x(),9999,pos1.z());
    btQuaternion rot = mBody->getWorldTransform().getRotation();
    btTransform trans1(rot, pos1);
    btTransform trans2(rot, pos2);

    //Do the cast.
    PlayerDimensionCheckResult res(mBody, mDimensions ^ DimensionManager::DIM_SWITCH, pos1, pos2);
    GlbVar.phyWorld->convexSweepTest(mGhostShape, trans1, trans2, res);

    //If no hits, then switch.
    if (!res.mHit)
    {
        GlbVar.dimMgr->switchDimension();
        setDimension(mDimensions ^ DimensionManager::DIM_SWITCH);
    }
    */

    /* v2: Broadphase ghost test.
    GlbVar.phyWorld->getDispatcher()->dispatchAllCollisionPairs(mGhostObject->getOverlappingPairCache(), GlbVar.phyWorld->getDispatchInfo(), GlbVar.phyWorld->getDispatcher());

    for (int i = 0; i < mGhostObject->getNumOverlappingObjects(); ++i)
    {
        btCollisionObject *obj = mGhostObject->getOverlappingObject(i);

        if (obj == mBody) //It's us!
            continue;

        short int flags = obj->getBroadphaseHandle()->m_collisionFilterGroup;

        if (flags & DimensionManager::NO_DIM_CHECK)
            continue;
        if (flags & (mDimensions ^ DimensionManager::DIM_SWITCH))
            return;
    }
    */

    /* v3: Broadphase, narrowphase ghost check + ray even/odd test for trimeshes */
    //First, do ghost test. Works for convex, and for trimesh-intersection.
    btManifoldArray manifoldArray;
    btBroadphasePairArray& pairArray = mGhostObject->getOverlappingPairCache()->getOverlappingPairArray();
    int numPairs = pairArray.size();

    for (int i = 0; i < numPairs; i++)
    {
        manifoldArray.clear();

        const btBroadphasePair& pair = pairArray[i];

        btBroadphasePair* collisionPair = GlbVar.phyWorld->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
        if (!collisionPair)
            continue;

        if (collisionPair->m_algorithm)
            collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

        for (int j = 0; j < manifoldArray.size(); j++)
        {
            btPersistentManifold* manifold = manifoldArray[j];
            btCollisionObject *obj = (btCollisionObject*) ((manifold->getBody0() == mGhostObject) ? manifold->getBody1() : manifold->getBody0());

            if (obj == mBody) //It's us!
                break;

            short int flags = obj->getBroadphaseHandle()->m_collisionFilterGroup;
            if (flags & DimensionManager::NO_DIM_CHECK) //It doesn't want dimension test!
                break;

            for (int p = 0; p < manifold->getNumContacts(); p++)
            {
                const btManifoldPoint&pt = manifold->getContactPoint(p);

                if (pt.getDistance() < 0.f)
                {
                    if (flags & (mDimensions ^ DimensionManager::DIM_SWITCH)) //It's in the opposite dimension, abort!
                        return;
                }
            }
        }
    }

    //Now, do trimesh-volume check. Use raycast, and then even-odd test.
    //http://en.wikipedia.org/wiki/Point_in_polygon
    struct PlayerTrimeshFind : public btDynamicsWorld::RayResultCallback
    {
        std::map<btCollisionObject*, bool> mHitMap;
        int mOppDimension;

        PlayerTrimeshFind(int oppDimension)
            : mOppDimension(oppDimension)
        {
        }

        btScalar addSingleResult(btDynamicsWorld::LocalRayResult &rayResult, bool)
        {
            btCollisionObject *obj = rayResult.m_collisionObject;
            mHitMap[obj] = !mHitMap[obj]; //Odd number of nots on 'false' (default bool constructor) is true, opposite for even.

            return 1; //Return 1 to get all hits.
        }

       bool needsCollision(btBroadphaseProxy* proxy0) const
        {
            //No non-dimension-check, only opposite dimension, only trimeshes (no need to ignore self, we're not trimesh anyway
            //so that test does that automatically).
            return (!(proxy0->m_collisionFilterGroup & DimensionManager::NO_DIM_CHECK))
                && (proxy0->m_collisionFilterGroup & mOppDimension) //Not really needed (we don't intersect with things in this dimension), but anyway.
                && ((btCollisionObject*)(proxy0->m_clientObject))->getCollisionShape()->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE;
        }
    } res(mDimensions ^ DimensionManager::DIM_SWITCH);

    btVector3 pos1 = mBody->getWorldTransform().getOrigin();
    btVector3 pos2 = pos1 + btVector3(0,100,0); //100 is big enough I guess. :P (cast upward)
    GlbVar.phyWorld->rayTest(pos1, pos2, res);

    for (std::map<btCollisionObject*, bool>::iterator iter = res.mHitMap.begin(); iter != res.mHitMap.end(); ++iter)
        if (iter->second) //The final bool for each trimesh is whether hits are odd (read previous comment).
            return;

    //All clear, switch!
    GlbVar.dimMgr->switchDimension();
    setDimension(mDimensions ^ DimensionManager::DIM_SWITCH);
}
//-------------------------------------------------------------------------------
void Player::die(bool explode)
{
    //We lost the level.
    loseLevel();

    //We're not going through this twice!
    if (mDead)
        return;

    //Explosions!
    if (explode)
        Util::createExplosion(mNode->getPosition());

    //And of course, we don't exist anymore. :-(
    GlbVar.goMgr->requestDestroy(getID());
    mDead = true;
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Player)
{
    NGF_PY_METHOD_IMPL(getControlOrientation)
    {
        NGF_PY_RETURN(mControlNode->getOrientation());
    }
    NGF_PY_METHOD_IMPL(loseCameraHandler)
    {
        loseCameraHandler();
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(captureCameraHandler)
    {
        captureCameraHandler();
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(switchDimension)
    {
        switchDimension();
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(numPickup)
    {
        Ogre::String type = py::extract<Ogre::String>(args[0]);
        NGF_PY_RETURN(mPickups[type]);
    }
    NGF_PY_METHOD_IMPL(hasPickup)
    {
        Ogre::String type = py::extract<Ogre::String>(args[0]);
        NGF_PY_RETURN(mPickups[type] > 0);
    }
    NGF_PY_METHOD_IMPL(incPickup)
    {
        Ogre::String type = py::extract<Ogre::String>(args[0]);
        ++mPickups[type];
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(decPickup)
    {
        Ogre::String type = py::extract<Ogre::String>(args[0]);
        if (mPickups[type] > 0)
            --mPickups[type];
        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(hasDecPickup)
    {
        //Decrease number, and return if we have (doors etc.).
        Ogre::String type = py::extract<Ogre::String>(args[0]);
        NGF_PY_RETURN(mPickups[type] > 0 && mPickups[type]--);
    }

    NGF_PY_PROPERTY_IMPL(underControl, mUnderControl, bool)
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
//-------------------------------------------------------------------------------
