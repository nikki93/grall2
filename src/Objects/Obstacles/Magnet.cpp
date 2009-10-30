/*
=========================================
Magnet.cpp
=========================================
*/

#define __MAGNET_CPP__

#include "Objects/Obstacles/Magnet.h"

//--- NGF events ----------------------------------------------------------------
Magnet::Magnet(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("Magnet");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Get properties.
    mForce = Ogre::StringConverter::parseReal(mProperties.getValue("force", 0, "35"));
    mForce *= mProperties.getValue("mode", 0, "attraction") != "repulsion" ? 1 : -1; //If repulsion, negative.
    mEnabled = Ogre::StringConverter::parseBool(mProperties.getValue("enabled", 0, "yes"));
    mRadius = Ogre::StringConverter::parseReal(mProperties.getValue("radius", 0, "10"));

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Magnet.mesh");
    mEntity->setMaterialName("Objects/Bomb");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createSphere();

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody();

    //Create the magnetic field zone.
    mFieldShape = new btSphereShape(1);
    mFieldShape->setLocalScaling(btVector3(mRadius, mRadius, mRadius));

    mFieldObject = new btPairCachingGhostObject();
    mFieldObject->setCollisionShape(mFieldShape);
    mFieldObject->setCollisionFlags(mFieldObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    GlbVar.phyWorld->addCollisionObject(mFieldObject
            , DimensionManager::NO_DIM_CHECK 
            | DimensionManager::NO_CRATE_CHECK
            | DimensionManager::NO_MOVING_CHECK
            | DimensionManager::NO_BULLET_HIT
            | DimensionManager::INVISIBLE
            | mDimensions,
            mDimensions
            );

    updateFieldPos();
}
//-------------------------------------------------------------------------------
void Magnet::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Magnet::~Magnet()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    GlbVar.phyWorld->removeCollisionObject(mFieldObject);
    delete mFieldObject;
    delete mFieldShape;
    destroyBody();
    delete mShape;

    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Magnet::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    if (mEnabled)
    {
        mEntity->setMaterialName(mForce > 0 ? "Objects/MagnetAttr" : "Objects/MagnetRepul");
        updateFieldPos();
        mFieldShape->setLocalScaling(btVector3(mRadius, mRadius, mRadius));

        //Tell objects about magnet.
        btManifoldArray manifoldArray;
        btBroadphasePairArray& pairArray = mFieldObject->getOverlappingPairCache()->getOverlappingPairArray();
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
                btCollisionObject *obj = (btCollisionObject*) ((manifold->getBody0() == mFieldObject) ? manifold->getBody1() : manifold->getBody0());

                if (obj == mBody) //It's us!
                    break;

                short int flags = obj->getBroadphaseHandle()->m_collisionFilterGroup;
                if (!(flags & DimensionManager::MAGNETIC)) //It doesn't want magnet events!
                    break;

                for (int p = 0; p < manifold->getNumContacts(); p++)
                {
                    const btManifoldPoint&pt = manifold->getContactPoint(p);

                    if (pt.getDistance() < 0.f)
                    {
                        NGF::GameObject *o = NGF::Bullet::fromBulletObject(obj);
                        GlbVar.goMgr->sendMessage(o, NGF_MESSAGE(MSG_MAGNET, mNode->getPosition(), mForce, mRadius));
                    }
                }
            }
        }
    }
    else
        mEntity->setMaterialName("Objects/MagnetDis");

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Magnet::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Magnet::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Magnet::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Magnet)
{
    NGF_PY_PROPERTY_IMPL(force, mForce, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(radius, mRadius, Ogre::Real);
    NGF_PY_PROPERTY_IMPL(enabled, mEnabled, bool);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
