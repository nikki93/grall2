/*
 * =====================================================================================
 *
 *       Filename:  DimensionManager.h
 *
 *    Description:  Handles dimension switches etc. Dimensions are just flags, so it's
 *                  possible to have objects existent in both dimensions, or enable both 
 *                  dimensions at the same time. 
 *
 *        Created:  04/30/2009 11:46:10 AM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __DIMENSIONMANAGER_H__
#define __DIMENSIONMANAGER_H__

class DimensionManager
{
    public:
        enum 
        {
            DIM_NONE = 0,
            DIM_1 = 1,
            DIM_2 = 2,
        };

    protected:
        int mCurrentDimension;

    public:
        DimensionManager()
            : mCurrentDimension(DIM_1)
        {
            Ogre::CompositorManager::getSingleton().addCompositor(GlbVar.ogreCamera->getViewport(), "Compositor/Dimension2");
        }

        //Toggle the current dimension. If both dimensions are on, then it switches
        //to no dimension.
        void switchDimension()
        {
            mCurrentDimension ^= DIM_1 ^ DIM_2;
            //GlbVar.goMgr->forEachGameObject(&DimensionManager::_sendMessage);
        }

        void tick()
        {
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(GlbVar.ogreCamera->getViewport(), "Compositor/Dimension2", mCurrentDimension & DIM_2);
        }

        //Enable the given dimension(s).
        void setDimension(int dimension)
        {
            mCurrentDimension = dimension;
            //GlbVar.goMgr->forEachGameObject(&DimensionManager::_sendMessage);
        }

        int getCurrentDimension() const
        {
            return mCurrentDimension;
        }

        //--- Internal stuff ----------------------------------------------------------

        static void _sendMessage(NGF::GameObject *obj)
        {
            GlbVar.goMgr->sendMessage(obj, NGF_MESSAGE(MSG_DIMENSIONCHANGED));
        }
};

#endif
