/*
=========================================
Player.cpp
=========================================
*/

#define __PLAYER_CPP__

#include "Objects/Main/Player.h"
#include "Objects/Misc/CameraHandler.h"

//--- NGF events ----------------------------------------------------------------
Player::Player(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mUnderControl(true)
{
    addFlag("Player");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Player.mesh");
    mEntity->setMaterialName("Objects/Player");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createSphere();
    btScalar mass = 3;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);

    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);
    info.m_friction = Ogre::Math::POS_INFINITY;
    info.m_linearDamping = 0.1;
    info.m_angularDamping = 0.6;

    mBody = new btRigidBody(info);
    mBody->setActivationState(DISABLE_DEACTIVATION);
    initBody();

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

    mControlNode->setPosition(mNode->getPosition());

    if (mBody->getAngularVelocity().length2() < 777)
    {
        Ogre::Vector3 torque = Ogre::Vector3::ZERO;
        torque.x =  (isKeyDown(OIS::KC_DOWN) || isKeyDown(OIS::KC_S)) - (isKeyDown(OIS::KC_UP) || isKeyDown(OIS::KC_W));
        torque.z =  (isKeyDown(OIS::KC_LEFT) || isKeyDown(OIS::KC_A)) - (isKeyDown(OIS::KC_RIGHT) || isKeyDown(OIS::KC_D));
        torque *= 9;

        mBody->applyTorque(BtOgre::Convert::toBullet(mControlNode->getOrientation() * torque));
    }

    OIS::MouseState ms = getMouseState();
    mControlNode->yaw(Ogre::Degree(-ms.X.rel * 0.2));

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
            if (GlbVar.paused)
                break;

            switch (msg.getParam<OIS::KeyCode>(0))
            {
                //Dimension switch!
                case OIS::KC_SPACE:
                    GlbVar.dimMgr->switchDimension();
                    setDimension(GlbVar.dimMgr->getCurrentDimension());
                    break;
            }

            break;
    }

    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Player::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    //(Much) Less friction if not ground hit.
    Ogre::Vector3 hitPos = BtOgre::Convert::toOgre(contact.getPositionWorldOnA());
    if (mNode->getPosition().y - hitPos.y < 0.4)
       contact.m_combinedFriction = 7;
    
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
    if (GlbVar.currCameraHandler)
    {
        GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETCAMERASTATE, int(CameraHandler::CS_NONE)));
    }
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Player)
{
    NGF_PY_METHOD_IMPL(getControlOrientation)
    {
        NGF_PY_RETURN(mControlNode->getOrientation());
    }
    NGF_PY_METHOD_IMPL(setControl)
    {
        mUnderControl = py::extract<bool>(args[0]);
        NGF_PY_RETURN();
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
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
//-------------------------------------------------------------------------------
