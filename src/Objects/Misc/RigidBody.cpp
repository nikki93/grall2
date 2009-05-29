/*
=========================================
RigidBody.cpp
=========================================
*/

#define __RIGIDBODY_CPP__

#include "Objects/Misc/RigidBody.h"

//--- NGF events ----------------------------------------------------------------
RigidBody::RigidBody(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("RigidBody");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Read physics properties.
    mp_Mass = Ogre::StringConverter::parseReal(properties.getValue("mass", 0, "5"));
    mp_Friction = Ogre::StringConverter::parseReal(properties.getValue("friction", 0, "0.5"));
    mp_Restitution = Ogre::StringConverter::parseReal(properties.getValue("restitution", 0, "0"));
    mp_LinearDamping = Ogre::StringConverter::parseReal(properties.getValue("linearDamping", 0, "0"));
    mp_AngularDamping = Ogre::StringConverter::parseReal(properties.getValue("angularDamping", 0, "0"));

    //Create the Ogre stuff.
    mEntity = createBrushEntity();
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createConvex();
    btScalar mass = mp_Mass;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);

    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);
    info.m_friction = mp_Friction;
    info.m_restitution = mp_Restitution;
    info.m_linearDamping = mp_LinearDamping;
    info.m_angularDamping = mp_AngularDamping;

    mBody = new btRigidBody(info);
    initBody();
}
//-------------------------------------------------------------------------------
void RigidBody::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
RigidBody::~RigidBody()
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
void RigidBody::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void RigidBody::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply RigidBody::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void RigidBody::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(RigidBody)
{
    NGF_PY_PROPERTY_IMPL(mass, mp_Mass, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(friction, mp_Friction, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(restitution, mp_Restitution, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(linearDamping, mp_LinearDamping, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(angularDamping, mp_AngularDamping, Ogre::Real);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
