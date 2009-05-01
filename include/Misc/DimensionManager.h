/*
 * =====================================================================================
 *
 *       Filename:  DimensionManager.h
 *
 *    Description:  Handles dimension switches etc.
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
        }

        void switchDimension()
        {
            mCurrentDimension ^= DIM_1 ^ DIM_2;
            //GlbVar.goMgr->forEachGameObject(&DimensionManager::_sendMessage);
        }

        void setDimension(int dimension)
        {
            mCurrentDimension = dimension;
            //GlbVar.goMgr->forEachGameObject(&DimensionManager::_sendMessage);
        }

        int getCurrentDimension() const
        {
            return mCurrentDimension;
        }

        static void _sendMessage(NGF::GameObject *obj)
        {
            GlbVar.goMgr->sendMessage(obj, NGF_MESSAGE(MSG_DIMENSIONCHANGED));
        }
};

#endif
