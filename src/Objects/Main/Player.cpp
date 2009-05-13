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
      mCameraHandler(0)
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
    mControlNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName + "-controlnode", pos, Ogre::Quaternion::IDENTITY);

    //Player can't be in a dimension that's not being displayed. :P
    setDimension(GlbVar.dimMgr->getCurrentDimension());

    //Camera stuff.
    if (mProperties.getValue("captureCamera", 0, "yes") != "no")
    {
        mCameraHandler = GlbVar.goMgr->createObject<CameraHandler>(pos, rot);

        //Camera is attached to the control node to prevent nausea.
        GlbVar.goMgr->sendMessage(mCameraHandler, NGF_MESSAGE(MSG_SETTARGET, mControlNode));
        //GlbVar.goMgr->sendMessage(mCameraHandler, NGF_MESSAGE(MSG_SETOFFSET, Ogre::Vector3(0,4.5,8)));
        GlbVar.goMgr->sendMessage(mCameraHandler, NGF_MESSAGE(MSG_SETSMOOTHINGFACTOR, Ogre::Real(4)));
        GlbVar.goMgr->sendMessage(mCameraHandler, NGF_MESSAGE(MSG_SETCAMERASTATE, int(CameraHandler::CS_THIRDPERSON)));
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

               /*
    const Ogre::Real force = 100;

    if (isKeyDown(OIS::KC_I))
        mBody->applyCentralForce(btVector3(-force,0,0));
    if (isKeyDown(OIS::KC_K))
        mBody->applyCentralForce(btVector3(force,0,0));
    if (isKeyDown(OIS::KC_J))
        mBody->applyCentralForce(btVector3(0,0,force));
    if (isKeyDown(OIS::KC_L))
        mBody->applyCentralForce(btVector3(0,0,-force));
        */
    
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

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Player)
{
}
*/
