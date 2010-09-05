/*
 * =====================================================================================
 *
 *       Filename:  NgfBullet.cpp
 *
 *    Description:  NGF-Bullet implementation
 *
 *        Version:  1.0
 *        Created:  03/31/2009 01:43:14 PM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "ngfplugins/NgfBullet.h"

namespace NGF { namespace Bullet {

    //Forward declaration of the callback.
    static bool _contactAdded(btManifoldPoint& cp, const btCollisionObject* colObj0, 
            int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1);

/*
 * =====================================================================================
 * Free Functions
 * =====================================================================================
 */

    void registerBulletGameObject(BulletGameObject *object, btCollisionObject *physic)
    {
        gContactAddedCallback = _contactAdded;
        physic->setUserPointer(object);
        physic->setCollisionFlags(physic->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    }
    //----------------------------------------------------------------------------------   
    BulletGameObject* fromBulletObject(btCollisionObject *physic)
    {
        return (BulletGameObject*) (physic->getUserPointer());
    }
    //----------------------------------------------------------------------------------   
    bool _contactAdded(btManifoldPoint& cp, const btCollisionObject* colObj0, 
            int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1)
    {
        //Get the OgreBullet objects.
        btCollisionObject *phy1 = const_cast<btCollisionObject *>(colObj0);
        btCollisionObject *phy2 = const_cast<btCollisionObject *>(colObj1);

        //Get the NGF GameObjects, and call the relevant methods.
        BulletGameObject *obj1 = fromBulletObject(phy1);
        BulletGameObject *obj2 = fromBulletObject(phy2);

        if (obj1 && obj2)
        {
            obj1->collide(obj2, phy2, cp);
            obj2->collide(obj1, phy1, cp);
        }
        else if (obj1)
        {
            obj1->collide(0, phy2, cp);
        }
        else if (obj2)
        {
            obj2->collide(0, phy1, cp);
        }

        return true;
    }

} //namespace Bullet

} //namespace NGF
