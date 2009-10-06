/*
=========================================
Empty.cpp
=========================================
*/

#define __EMPTY_CPP__

#include "Objects/Misc/Empty.h"

//--- NGF events ----------------------------------------------------------------
Empty::Empty(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mEntity(0),
      mShape(0),
      mPos(pos),
      mRot(rot)
{
    addFlag("Empty");

    Ogre::String script = mProperties.getValue("script", 0, "");

    mNode = 0;
    mBody = 0;

    //Python init event.
    NGF_PY_CALL_EVENT(init);
}
//-------------------------------------------------------------------------------
void Empty::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Empty::~Empty()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    destroyBody();
    if (mShape)
        delete mShape;

    if (mNode && mEntity)
    {
        mNode->detachAllObjects();
        GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
    }
}
//-------------------------------------------------------------------------------
void Empty::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Empty::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Empty::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Empty::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void Empty::createBody(Ogre::String type, bool kinematic, int flags)
{
    if (!(mNode && mEntity))
        return;

    BtOgre::StaticMeshToShapeConverter converter(mEntity);

    if (type == "convex")
        mShape = converter.createConvex();
    else if (type == "trimesh")
        mShape = converter.createTrimesh();
    else if (type == "box")
        mShape = converter.createBox();
    else if (type == "sphere")
        mShape = converter.createSphere();
    else if (type == "cylinderY")
        mShape = new btCylinderShape(BtOgre::Convert::toBullet(converter.getSize() * 0.5));
    else if (type == "cylinderZ")
        mShape = new btCylinderShapeZ(BtOgre::Convert::toBullet(converter.getSize() * 0.5));
    else if (type == "cylinderX")
        mShape = new btCylinderShapeX(BtOgre::Convert::toBullet(converter.getSize() * 0.5));
    else
        mShape = converter.createConvex();

    btScalar mass = mp_Mass;
    btVector3 inertia;

    if (mp_Mass)
        mShape->calculateLocalInertia(mass, inertia);
    else
        inertia = btVector3(0,0,0);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);

    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);
    info.m_friction = mp_Friction;
    info.m_restitution = mp_Restitution;
    info.m_linearDamping = mp_LinearDamping;
    info.m_angularDamping = mp_AngularDamping;

    mBody = new btRigidBody(info);
    mBody->setActivationState(DISABLE_DEACTIVATION);

    if (kinematic)
        mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

    initBody(flags);
}
//-------------------------------------------------------------------------------
void Empty::createMesh(Ogre::String meshName)
{
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, meshName);
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, mPos, mRot);
    mNode->attachObject(mEntity);
}
//-------------------------------------------------------------------------------
void Empty::createBrushMesh()
{
    mEntity = createBrushEntity();
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, mPos, mRot);
    mNode->attachObject(mEntity);
}
//-------------------------------------------------------------------------------
void Empty::setMaterial(Ogre::String matName)
{
    mEntity->setMaterialName(matName);
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Empty)
{
    NGF_PY_METHOD_IMPL(createBody)
    {
        Ogre::String type = py::extract<Ogre::String>(args[0]);
        bool kinematic = py::extract<bool>(args[1]);
        int flags = py::extract<int>(args[2]);
        createBody(type, kinematic, flags);

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(createMesh)
    {
        Ogre::String meshName = py::extract<Ogre::String>(args[0]);
        createMesh(meshName);

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(createBrushMesh)
    {
        createBrushMesh();

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(setMaterial)
    {
        Ogre::String material = py::extract<Ogre::String>(args[0]);
        setMaterial(material);

        NGF_PY_RETURN();
    }

    NGF_PY_METHOD_IMPL(getPosition)
    {
        if (mNode)
            NGF_PY_RETURN(mNode->getPosition());
        else
            NGF_PY_RETURN(mPos);
    }
    NGF_PY_METHOD_IMPL(getOrientation)
    {
        if (mNode)
            NGF_PY_RETURN(mNode->getOrientation());
        else
            NGF_PY_RETURN(mRot);
    }

    NGF_PY_PROPERTY_IMPL(mass, mp_Mass, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(friction, mp_Friction, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(restitution, mp_Restitution, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(linearDamping, mp_LinearDamping, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(angularDamping, mp_AngularDamping, Ogre::Real);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
//-------------------------------------------------------------------------------
