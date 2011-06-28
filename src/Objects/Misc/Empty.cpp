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
      mRot(rot),
      mp_Mass(5),
      mp_Friction(0.5),
      mp_Restitution(0),
      mp_LinearDamping(0),
      mp_AngularDamping(0)
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
void Empty::createBody(int shape, int bodyType, int flags)
{
    if (!(mNode && mEntity))
        return;

    //Create shape.
    switch (shape)
    {
        case PythonBodyFlags::MANUAL:
            break;

        case PythonBodyFlags::TRIMESH:
            {
                BtOgre::StaticMeshToShapeConverter converter(mEntity);
                mShape = converter.createTrimesh();
            }
            break;

        case PythonBodyFlags::BOX:
            {
                BtOgre::StaticMeshToShapeConverter converter(mEntity);
                mShape = converter.createBox();
            }
            break;

        case PythonBodyFlags::SPHERE:
            {
                BtOgre::StaticMeshToShapeConverter converter(mEntity);
                mShape = converter.createSphere();
            }
            break;

        case PythonBodyFlags::CYLINDERY:
            {
                BtOgre::StaticMeshToShapeConverter converter(mEntity);
                mShape = new btCylinderShape(BtOgre::Convert::toBullet(converter.getSize() * 0.5));
            }
            break;

        case PythonBodyFlags::CYLINDERZ:
            {
                BtOgre::StaticMeshToShapeConverter converter(mEntity);
                mShape = new btCylinderShapeZ(BtOgre::Convert::toBullet(converter.getSize() * 0.5));
            }
            break;

        case PythonBodyFlags::CYLINDERX:
            {
                BtOgre::StaticMeshToShapeConverter converter(mEntity);
                mShape = new btCylinderShapeX(BtOgre::Convert::toBullet(converter.getSize() * 0.5));
            }
            break;

        default:
            {
                BtOgre::StaticMeshToShapeConverter converter(mEntity);
                mShape = converter.createConvex();
                mShape->setMargin(0);
            }
            break;
    }

    //Calculate inertia, (or not, if static/kinematic).
    btVector3 inertia;
    switch (bodyType & (PythonBodyFlags::FREE | PythonBodyFlags::STATIC | PythonBodyFlags::STATIC))
    {
        case PythonBodyFlags::KINEMATIC:
        case PythonBodyFlags::STATIC:
            inertia = btVector3(0,0,0);
            mp_Mass = 0;
            break;

        default:
            mShape->calculateLocalInertia(mp_Mass, inertia);
            break;
    }

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);

    btRigidBody::btRigidBodyConstructionInfo info(mp_Mass, state, mShape, inertia);
    info.m_friction = mp_Friction;
    info.m_restitution = mp_Restitution;
    info.m_linearDamping = mp_LinearDamping;
    info.m_angularDamping = mp_AngularDamping;

    mBody = new btRigidBody(info);
    mBody->setActivationState(DISABLE_DEACTIVATION);

    if (bodyType == PythonBodyFlags::KINEMATIC)
        mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

    if (bodyType & PythonBodyFlags::NO_CONTACT)
        mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

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
    Ogre::String mesh = mProperties.getValue("brushMeshFile", 0, "Player.mesh");
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, mesh);
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, mPos, mRot);
    mNode->attachObject(mEntity);
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Empty)
{
    NGF_PY_METHOD_IMPL(createBody)
    {
        int shape = py::extract<int>(args[0]);
        int bodyType = py::extract<int>(args[1]);
        int flags = py::extract<int>(args[2]);
        createBody(shape, bodyType, flags);

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
    NGF_PY_METHOD_IMPL(setBrushMaterial)
    {
        setBrushMaterial();

        NGF_PY_RETURN();
    }

    NGF_PY_METHOD_IMPL(createBoxShape)
    {
        NGF_PY_METHOD_PARAMS_1(Ogre::Vector3, half);
        createBoxShape(half);

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
