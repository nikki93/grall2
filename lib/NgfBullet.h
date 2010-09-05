/*
 * =====================================================================================
 *
 *       Filename:  NgfBullet.h
 *
 *    Description:  NGF-Bullet connection. Allows BulletGameObjects to be notified of
 *                  Bullet collision events.
 *
 *                  NOTE: Bullet's 'gContactAddedCallback' is overwritten for
 *                        callbacks.
 *
 *        Version:  1.0
 *        Created:  02/27/2009 11:40:53 PM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __NGF_BULLET_H__
#define __NGF_BULLET_H__

#include <Ogre.h> //Change this to only include specific headers when done.
#include <Ngf.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btManifoldResult.h>
#include <BulletCollision/NarrowPhaseCollision/btManifoldPoint.h>

namespace NGF { namespace Bullet {

class BulletGameObject;

/*
 * =====================================================================================
 * Free Functions
 * =====================================================================================
 */

//Register a BulletGameObject for collision. It is associated with the given btCollisionObject.
//The given BulletGameObject will be notified of any collision events involving the given
//btCollisionObject. You can also use 'setBulletObject' from within a BulletGameObject.
void registerBulletGameObject(BulletGameObject *object, btCollisionObject *physic);

//Returns the BulletGameObject associated with the given btCollisionObject. An association is
//made with the 'registerForCollision' function.
BulletGameObject* fromBulletObject(btCollisionObject *physic);

/*
 * =====================================================================================
 *        Class:  BulletGameObject
 *
 *  Description:  An interface for GameObject that can receive 'collide' events.
 * =====================================================================================
 */

class BulletGameObject : virtual public GameObject
{
    public:
        BulletGameObject()
            : GameObject(Ogre::Vector3(), Ogre::Quaternion(), ID(), PropertyList(), "")
        {
        }

        //Called on collision with a physics object.
        //
        //other: The GameObject collided with.
        //otherPhysicsObject: The btCollisionObject collided with.
        //contact: The mainfold contact point.
        virtual void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact) { }

        //Register self for collision event. We're associated with the given GameObject.
        inline void setBulletObject(btCollisionObject *physic) { registerBulletGameObject(this, physic); }
};

} //namespace Bullet

} //namespace NGF

#endif //ifndef __NGF_BULLET_H__
