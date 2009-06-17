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

//--- NGF events ----------------------------------------------------------------
Player::Player(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mUnderControl(true),
      mDead(false)
{
    addFlag("Player");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Player.mesh");
    mEntity->setMaterialName("Objects/Player");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Read in properties.
    mMinHeight = Ogre::StringConverter::parseReal(mProperties.getValue("minHeight", 0, "-4"));

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createSphere();
    btScalar mass = 2;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);

    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);
    info.m_friction = Ogre::Math::POS_INFINITY;
    info.m_linearDamping = 0.1;
    info.m_angularDamping = 0.6;

    mBody = new btRigidBody(info);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions | DimensionManager::PLAYER, mDimensions);
    setBulletObject(mBody);

    //Control node is used for controlling the Player. He rotates in all kinds of crazy ways, but we need to know where we're headed.
    mControlNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName + "-controlnode", pos, rot);

    //Player can't be in a dimension that's not being displayed. :P
    setDimension(GlbVar.dimMgr->getCurrentDimension());
}
//-------------------------------------------------------------------------------
void Player::postLoad()
{
    //Camera stuff. We check that we aren't unserialising, because the CamerHandler is automatically loade up if so.
    if (!(mProperties.getValue("NGF_SERIALISED", 0, "no") == "yes" || mProperties.getValue("captureCameraHandler", 0, "yes") == "no"))
    {
        captureCameraHandler();
    }

    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Player::~Player()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //Fix FOV.
    GlbVar.ogreCamera->setFOVy(Ogre::Degree(45));

    //We only clear up stuff that we did.
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

    //Allow the controlnode to catch up.
    mControlNode->setPosition(mNode->getPosition());

    //Controls.
    if (mBody->getAngularVelocity().length2() < 600)
    {
        Ogre::Vector3 torque = Ogre::Vector3::ZERO;
        torque.x =  (isKeyDown(OIS::KC_DOWN) || isKeyDown(OIS::KC_S)) - (isKeyDown(OIS::KC_UP) || isKeyDown(OIS::KC_W));
        torque.z =  (isKeyDown(OIS::KC_LEFT) || isKeyDown(OIS::KC_A)) - (isKeyDown(OIS::KC_RIGHT) || isKeyDown(OIS::KC_D));
        torque *= 6;

        mBody->applyTorque(BtOgre::Convert::toBullet(mControlNode->getOrientation() * torque));
    }

    OIS::MouseState ms = getMouseState();
    mControlNode->yaw(Ogre::Degree(-ms.X.rel * 0.2));

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

            switch (msg.getParam<OIS::KeyCode>(0))
            {
                //Dimension switch!
                case OIS::KC_SPACE:
                    switchDimensions();
                    break;
                case OIS::KC_U:
                    die(false);
                    break;
            }

            break;

        case MSG_GETPOSITION:
            NGF_SEND_REPLY(BtOgre::Convert::toOgre(mBody->getWorldTransform().getOrigin()));
    }

    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Player::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
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
void Player::switchDimensions()
{
    //If the other dimension isn't free at our place, then abort.

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

    //Copy shape for cast. Maybe we should do it in the beginning and keep a copy?
    btSphereShape shape(*mShape);
    shape.setLocalScaling(btVector3(0.85,0.85,0.85));

    //Where to cast from, where to cast to, etc.
    btVector3 pos1 = mBody->getWorldTransform().getOrigin() - btVector3(0,0.15,0);
    btVector3 pos2 = btVector3(pos1.x(),9999,pos1.z());
    btQuaternion rot = mBody->getWorldTransform().getRotation();
    btTransform trans1(rot, pos1);
    btTransform trans2(rot, pos2);

    //Do the cast.
    PlayerDimensionCheckResult res(mBody, mDimensions ^ DimensionManager::DIM_SWITCH, pos1, pos2);
    GlbVar.phyWorld->convexSweepTest(&shape, trans1, trans2, res);

    //If no hits, then switch.
    if (!res.mHit)
    {
        GlbVar.dimMgr->switchDimension();
        setDimension(GlbVar.dimMgr->getCurrentDimension());
    }
}
//-------------------------------------------------------------------------------
void Player::die(bool explode)
{
    //We're not going through this twice!
    if (mDead)
        return;

    //Deathcam! :-)
    if (!GlbVar.currCameraHandler)
    {
        GlbVar.currCameraHandler = GlbVar.goMgr->createObject<CameraHandler>(mControlNode->getPosition() + (mControlNode->getOrientation() * Ogre::Vector3(0,9,16)), mControlNode->getOrientation());
        GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETSMOOTHINGFACTOR, Ogre::Real(4)));
    }
    GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETTARGET, mControlNode));
    GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETCAMERASTATE, int(CameraHandler::CS_DEATH)));

    //Explosions!
    if (explode)
    {
        GlbVar.goMgr->createObject<Light>(mNode->getPosition(), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
                ("lightType", "point")
                ("colour", "1 0.6 0")
                ("specular", "0.1 0.1 0.1")
                ("attenuation", "10 0.6 0.1 0.05")
                ("time", "1.6")
                ("fadeOutTime", "0.75")
                );

        GlbVar.goMgr->createObject<ParticleEffect>(mNode->getPosition(), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
                ("template", "ParticleFX/Explosion")
                ("time", "2")
                );
    }

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

    NGF_PY_PROPERTY_IMPL(underControl, mUnderControl, bool)
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
//-------------------------------------------------------------------------------
