/*
 * =====================================================================================
 *
 *       Filename:  NgfExtras.h
 *
 *    Description:  Some extra GameObject types etc. for use in games.
 *
 *        Version:  1.0
 *        Created:  03/31/2009 08:06:00 PM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __NGF_EXTRAS_H__
#define __NGF_EXTRAS_H__

#include "Ngf.h"
#include "ngfplugins/NgfPython.h"
#include <boost/function.hpp>

#include <map>

namespace NGF { namespace Extras {

class TaskManagingGameObject : virtual public GameObject
{
    protected:
        typedef boost::function<void ()> Task;

        typedef std::multimap<Ogre::Real, Task> TaskMap;
        typedef std::pair<Ogre::Real, Task> TaskPair;
        TaskMap mTasks;

        Ogre::Real mCurrTime;
        unsigned int mTasksDone;

    public:
        TaskManagingGameObject()
            : GameObject(Ogre::Vector3(), Ogre::Quaternion(), ID(), PropertyList(), ""),
              mCurrTime(0),
              mTasksDone(0)
        {
        }

        void addTask(Ogre::Real time, Task task)
        {
            mTasks.insert(TaskPair(mCurrTime + time, task));
        }

        void updateTasks(Ogre::Real deltaTime)
        {
            mCurrTime += deltaTime;

            for (TaskMap::iterator iter = mTasks.begin(); 
                    iter != mTasks.end() && iter->first < mCurrTime;)
            {
                iter->second();
                mTasks.erase(iter++);
                ++mTasksDone;
            }
        }
};

} //namespace Extras

} //namespace NGF

#define NGF_EXTRAS_PYTHON_METHOD_ADD_TASK(func)                                                \
    NGF_PY_METHOD_IMPL(func)                                                                   \
    {                                                                                          \
        addTask(py::extract<float>(args[0]), boost::function<void ()>(args[1]));               \
                                                                                               \
	return py::object();                                                                   \
    }

#endif //#ifndef __NGF_EXTRAS_H__
