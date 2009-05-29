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

#define BIT(x) (1<<(x))

class DimensionManager
{
    public:
        //Bullet object masks.
        enum 
        {
            NONE = 0,

            DIM_1 = BIT(0),
            DIM_2 = BIT(1),
            
            NO_DIM_CHECK = BIT(2),

            PLAYER = BIT(3),

            //XOR with this to switch dimension bits.
            DIM_SWITCH = BIT(0) ^ BIT(1),
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
            mCurrentDimension ^= DIM_SWITCH;
            GlbVar.goMgr->forEachGameObject(&DimensionManager::_sendMessage);
        }

        void tick()
        {
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(GlbVar.ogreCamera->getViewport(), "Compositor/Dimension2", mCurrentDimension & DIM_2);
        }

        //Enable the given dimension(s).
        void setDimension(int dimension)
        {
            mCurrentDimension = dimension;
            GlbVar.goMgr->forEachGameObject(&DimensionManager::_sendMessage);
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
