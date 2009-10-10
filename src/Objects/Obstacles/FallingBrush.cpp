/*
=========================================
FallingBrush.cpp
=========================================
*/

#define __FALLINGBRUSH_CPP__

#include "Objects/Obstacles/FallingBrush.h"

//--- NGF events ----------------------------------------------------------------
FallingBrush::FallingBrush(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTime(0),
      mFell(false)
{
    addFlag("FallingBrush");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Get properties.
    mFallTime = Ogre::StringConverter::parseReal(mProperties.getValue("time", 0, "0.5"));

    //Create the Ogre stuff.
    if (mProperties.getValue("brushMeshFile", 0, "n") == "n")
    {
        mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Template_Pad.mesh");
        mEntity->setMaterialName("Objects/SwitchOn");
    }
    else
    {
        mEntity = createBrushEntity();
    }
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);

    mShape = converter.createConvex();
    mShape->setMargin(0);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody( DimensionManager::STATIC
            );
}
//-------------------------------------------------------------------------------
void FallingBrush::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
FallingBrush::~FallingBrush()
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
void FallingBrush::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Time flies, and we fall (ok, that was bad).
    if (!mFell && mTime > 0)
    {
        mTime -= evt.timeSinceLastFrame;

        if (mTime <= 0)
            fall();
    }
    checkFell();
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void FallingBrush::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply FallingBrush::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void FallingBrush::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    if (!mFell && !mTime && other->hasFlag("Player"))
        mTime = mFallTime;

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void FallingBrush::fall()
{
    //We just remove the old (static) body and create a dynamic one in it's place.
    destroyBody();

    btScalar mass = 42;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(mass, state, mShape, inertia);
    initBody( DimensionManager::STATIC
            );

    mFell = true;
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(FallingBrush)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
