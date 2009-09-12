/*
 * =====================================================================================
 *
 *       Filename:  ObjectClicker.h
 *
 *    Description:  Couldn't find a better name. :/ Used by the PythonObject
 *                  select-by-click thing. When you call 'click()', after a mouse click,
 *                  the GameObject clicked on can be retreived using 'getClicked()'.
 *
 *        Created:  09/12/2009 02:32:50 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */


#ifndef __OBJECTCLICKER_H__
#define __OBJECTCLICKER_H__

#include "Globals.h"

class ObjectClicker
{
    protected:
        NGF::GameObject *mClickedObj;
        bool mIsClicking;

    public:
        ObjectClicker()
            : mClickedObj(NULL),
              mIsClicking(false)
        {
        }

        //Start a click.
        void click()
        {
            mIsClicking = true;
        }

        //Retreive last clicked object.
        NGF::GameObject *getClickedObject()
        {
            return mClickedObj;
        }

        void tick()
        {
            if (mIsClicking)
            {
                //We have to be able to do it even when console is visible, so get MouseState directly.
                OIS::MouseState ms = GlbVar.mouse->getMouseState();

                if (ms.buttonDown(OIS::MB_Left))
                {
                    //Get ray from camera origin through viewport.
                    Ogre::Real w = GlbVar.ogreWindow->getWidth();
                    Ogre::Real h = GlbVar.ogreWindow->getHeight();
                    Ogre::Ray ray = GlbVar.ogreCamera->getCameraToViewportRay(ms.X.abs / w, ms.Y.abs / h);

                    //The callback gets the first object that isn't a no-contact-response.
                    struct ClickRayResult : public btDynamicsWorld::ClosestRayResultCallback
                    {
                        ClickRayResult(const btVector3 &from, const btVector3 &to)
                            : btCollisionWorld::ClosestRayResultCallback(from, to)
                        {
                        }

                        bool needsCollision(btBroadphaseProxy* proxy0) const
                        {
                            return proxy0->m_collisionFilterGroup & GlbVar.dimMgr->getCurrentDimension() //Viewed dimension only.
                                && !(((btCollisionObject*) proxy0->m_clientObject)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE); //If no contact response, ignore.
                        }
                    };

                    btVector3 pos1 = BtOgre::Convert::toBullet(ray.getOrigin());
                    btVector3 pos2 = BtOgre::Convert::toBullet(ray.getPoint(100));
                    ClickRayResult res(pos1, pos2);

                    GlbVar.phyWorld->rayTest(pos1, pos2, res);

                    if (res.m_collisionObject)
                    {
                        NGF::GameObject *gobj = NGF::Bullet::fromBulletObject(res.m_collisionObject);

                        //If it's all ok, save it, and stop.
                        if (gobj)
                        {
                            mClickedObj = gobj;
                            GlbVar.console->print("Object clicked! ID: " + Ogre::StringConverter::toString(mClickedObj->getID()) + ", Flags: " + mClickedObj->getFlags() + "\n");
                            GlbVar.goMgr->sendMessage(mClickedObj, NGF_MESSAGE(MSG_CLICKED));
                            mIsClicking = false; //We're done! :D
                            PyRun_SimpleString("import GraLL2\nclicked = GraLL2.getClickedObject()");
                            return;
                        }
                    }
                    GlbVar.console->print("No object found. Try again.\n");
                    mIsClicking = false; //Unsuccessful, but still done. ;-)
                }
            }
        }
};

#endif
