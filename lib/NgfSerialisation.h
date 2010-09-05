/*
 * =====================================================================================
 *
 *       Filename:  NgfSerialisation.h
 *
 *    Description:  GameObject saving and loading mechanism.
 *
 *        Version:  1.0
 *        Created:  03/22/2009 09:47:54 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __NGF_SERIALISATION_H__
#define __NGF_SERIALISATION_H__

#include <sstream>

#include <Ogre.h> //Change this to only include specific headers when done.
#include <Ngf.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace NGF { namespace Serialisation {

class GameObjectRecord;

/*
 * =====================================================================================
 *        Class:  SerialisableGameObject
 *
 *  Description:  An interface for GameObjects that can be saved and loaded. Implement
 *  		  the 'save' and 'load' methods.
 * =====================================================================================
 */

class SerialisableGameObject : virtual public GameObject
{
    public:
        SerialisableGameObject()
            : GameObject(Ogre::Vector3(), Ogre::Quaternion(), ID(), PropertyList(), "")
        {
        }

        //Override this to get serialisability. The 'save' parameter tells you whether
        //we are in write mode (if true) or read mode (if false). If in write mode,
        //return a GameObjectRecord with your type and whatever information you want
        //to save. If in read mode, use the 'in' PropertyList to read back the information
        //you wrote.
        virtual GameObjectRecord serialise(bool save, PropertyList &in) = 0;
};

/*
 * =====================================================================================
 *        Class:  Serialiser
 *
 *  Description:  Easy saving and loading of GameObjects with just 
 *  		  NGF::Serialiser::save(filename) and NGF::Serialiser::load(filename).
 * =====================================================================================
 */

class Serialiser : public Ogre::Singleton<Serialiser>
{
    protected:
	static std::vector<GameObjectRecord> mRecords;

    public:
	//Save the game. Automatically saves all GameObjects inheriting form 'SerialisableGameObject'. 
        //Calls 'serialise' on all GameObjects to allow them to save their position and rotation 
        //and any extra information.
	static void save(Ogre::String filename);

	//Load the game. Creates GameObjects based on the type contained in the GameObjectRecord, 
        //with the old GameObject's name, properties, and position and rotation saved by the 
        //GameObject. Additional information saved by the GameObject is also restored.
	static void load(Ogre::String filename);

	//--- Internal stuff --------------------------------------------------------------
	
	static void _saveOne(GameObject*);
};

/*
 * =====================================================================================
 *        Class:  GameObjectRecord
 *
 *  Description:  Stores information for one GameObject. It stores the type of the
 *  		  object, the name, and the properties, and extra information returned
 *  		  by the object.
 * =====================================================================================
 */

class GameObjectRecord
{
    protected:
        //Usual GameObject data.
	ID mID;
	Ogre::String mType;
	Ogre::String mName;
        bool mPersistent;
	std::map<Ogre::String, std::vector<Ogre::String> > mProps;

        //Extra information from 'serialise' function.
	std::map<Ogre::String, std::vector<Ogre::String> > mInfo;

	friend class boost::serialization::access;
	friend class Serialiser;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
	    ar & mID;
	    ar & mType;
	    ar & mName;
            ar & mPersistent;
	    ar & mProps;
	    ar & mInfo;
	}

    public:
	GameObjectRecord()
	    : mType("none"),
              mPersistent(false),
              mID(0),
              mName("")
	{
	}

	GameObjectRecord(Ogre::String type, PropertyList info)
	    : mType(type),
	      mInfo(info),
              mPersistent(false),
              mID(0),
              mName("")
	{
	}

	PropertyList getInfo() { return PropertyList(mInfo); }
};

} //namespace Serialisation

} //namespace NGF

/*
 * =====================================================================================
 *       Macros:  NGF_SERIALISE_*
 *
 *  Description:  These macros make defining the 'serialise' function easier for
 *  		  Ogre datatypes (Real, Vector3, Quaternion etc.). If you look at the
 *  		  macros below together, you can see a 'serialise' function take form.
 *  		  ;-)
 *
 *  		  Note: all the 'var' parameters take the variable name in the current 
 *  		        context. So, if the name of a member is 'mHealth', write 
 *  		        'mHealth'. Local variables in the 'serialise' function will
 *  		        work too.
 * =====================================================================================
 */

//Begins the serialise function definition.
#define NGF_SERIALISE_BEGIN(classnm)                                                           \
	NGF::Serialisation::GameObjectRecord serialise(bool save, NGF::PropertyList &in)       \
	{                                                                                      \
	    NGF::PropertyList out;                                                             \
	    Ogre::String type = #classnm;

//The position saved this way will be restored as the position sent in the constructor.
#define NGF_SERIALISE_POSITION(pos)                                                            \
	    if (save)                                                                          \
                out.addProperty("NGF_POSITION", Ogre::StringConverter::toString(pos), "")

//The rotation saved this way will be restored as the rotation sent in the constructor.
#define NGF_SERIALISE_ROTATION(rot)                                                            \
            if (save)                                                                          \
                out.addProperty("NGF_ROTATION", Ogre::StringConverter::toString(rot), "")

//For saving strings.
#define NGF_SERIALISE_STRING(var)                                                              \
	    if (save) {                                                                        \
		out.addProperty(#var , var, "");                                               \
	    } else  {                                                                          \
		var = in.getValue(#var, 0, "");                                                \
	    }

//Works for all types that the Ogre::StringConverter::parse* functions support. 'type' must be
//the '*' part of the parse* function used.
#define NGF_SERIALISE_OGRE(type, var)                                                          \
	    if (save) {                                                                        \
		out.addProperty(#var , Ogre::StringConverter::toString(var), "");              \
	    } else  {                                                                          \
		Ogre::String var##str = in.getValue(#var, 0, "n");                             \
		if (var##str != "n")                                                           \
		    var = Ogre::StringConverter::parse##type(var##str);                        \
	    }

//For serialising pointers to GameObjects. The GameObject pointed to must itself be a
//SerialisableGameObject, because this doesn't actually create any GameObject, but just restores
//the pointer to the pointed GameObject. Works also for NULL pointers.
#define NGF_SERIALISE_GAMEOBJECTPTR(var)                                                       \
            if (save) {                                                                        \
                if (var)                                                                       \
                    out.addProperty(#var , Ogre::StringConverter::toString(var->getID()), ""); \
                else                                                                           \
                    out.addProperty(#var , "n", "");                                           \
            } else {                                                                           \
		Ogre::String var##str = in.getValue(#var, 0, "n");                             \
                if (var##str != "n")                                                           \
                {                                                                              \
                    NGF::ID id = Ogre::StringConverter::parseInt(var##str);                    \
                    var = NGF::GameObjectManager::getSingleton().getByID(id);                  \
                }                                                                              \
                else                                                                           \
                    var = NULL;                                                                \
            }

		//LogManager::getSingleton().logMessage(var->getID());     \
//Blocks following these macros would be called on saving and loading respectively.
#define NGF_SERIALISE_ON_SAVE                                                                  \
            if(save)
#define NGF_SERIALISE_ON_LOAD                                                                  \
            if(!save)

//Ends the serialise function definition.
#define NGF_SERIALISE_END                                                                      \
	    return NGF::Serialisation::GameObjectRecord(type , out);                           \
	}

//If you're using NGF::Python, you can save the Python locals ('m_*' variables) with this.
#define NGF_SERIALISE_PYTHON_LOCALS()                                                          \
            if (save) {                                                                        \
                out.addProperty("NGF_PY_LOCALS" , mConnector->dumpLocals(), "");               \
            } else  {                                                                          \
                mConnector->loadLocals(in.getValue("NGF_PY_LOCALS", 0, "'(dp0\n.'"));          \
            }

//Serialises a Bullet RigidBody's state. Uses BtOgre for Vector3/Quaternion conversion. The
//body's position, orientation, linear and angular velocity get saved.
#define NGF_SERIALISE_BULLET_BODY(var)                                                         \
            {                                                                                  \
                Ogre::Vector3 var##_pos, var##_lVel, var##_aVel;                               \
                Ogre::Quaternion var##_rot;                                                    \
                                                                                               \
                NGF_SERIALISE_ON_SAVE                                                          \
                {                                                                              \
                    btTransform trans = var->getWorldTransform();                              \
                                                                                               \
                    var##_pos = BtOgre::Convert::toOgre(trans.getOrigin());                    \
                    var##_rot = BtOgre::Convert::toOgre(trans.getRotation());                  \
                    var##_lVel = BtOgre::Convert::toOgre(var->getLinearVelocity());            \
                    var##_aVel = BtOgre::Convert::toOgre(var->getAngularVelocity());           \
                }                                                                              \
                                                                                               \
                NGF_SERIALISE_OGRE(Vector3, var##_pos);                                        \
                NGF_SERIALISE_OGRE(Quaternion, var##_rot);                                     \
                NGF_SERIALISE_OGRE(Vector3, var##_lVel);                                       \
                NGF_SERIALISE_OGRE(Vector3, var##_aVel);                                       \
                                                                                               \
                NGF_SERIALISE_ON_LOAD                                                          \
                {                                                                              \
                    btVector3 pos = BtOgre::Convert::toBullet(var##_pos);                      \
                    btQuaternion rot = BtOgre::Convert::toBullet(var##_rot);                   \
                                                                                               \
                    var->setWorldTransform(btTransform(rot, pos));                             \
                    var->setLinearVelocity(BtOgre::Convert::toBullet(var##_lVel));             \
                    var->setAngularVelocity(BtOgre::Convert::toBullet(var##_aVel));            \
                }                                                                              \
            }                                                                                      

//Serialises tasks in a TaskManagingGameObject.
#define NGF_SERIALISE_TASKS()                                                                  \
            NGF_SERIALISE_OGRE(Real, mCurrTime);                                               \
            NGF_SERIALISE_OGRE(UnsignedInt, mTasksDone);                                       \
                                                                                               \
            /* Remove tasks that are already done. */                                          \
            NGF_SERIALISE_ON_LOAD                                                              \
            {                                                                                  \
                unsigned int i = 0;                                                            \
                for (TaskMap::iterator iter = mTasks.begin();                                  \
                        iter != mTasks.end() && i < mTasksDone; ++i, ++iter)                   \
                {                                                                              \
                    mTasks.erase(iter);                                                        \
                }                                                                              \
            }

//Serialises an STL container, with boost-serialisable element types. You'll have to make sure
//to #include the boost::serialisation header for the container type (usually
//'boost/serialization/<type>.hpp', like 'boost/serialization/map.hpp').
#define NGF_SERIALISE_STL_CONTAINER(var)                                                       \
            {                                                                                  \
                std::string var##_str;                                                         \
                std::stringstream var##_strm(std::stringstream::in | std::stringstream::out);  \
                                                                                               \
                NGF_SERIALISE_ON_SAVE                                                          \
                {                                                                              \
                    boost::archive::text_oarchive oa(var##_strm);                              \
                    oa << var;                                                                 \
                    var##_str = var##_strm.str();                                              \
                }                                                                              \
                                                                                               \
                NGF_SERIALISE_STRING(var##_str);                                               \
                                                                                               \
                NGF_SERIALISE_ON_LOAD                                                          \
                {                                                                              \
                    var##_strm << var##_str;                                                   \
                    boost::archive::text_iarchive ia(var##_strm);                              \
                    ia >> var;                                                                 \
                }                                                                              \
            }

//If you're using Ngf::States you can save the state of the state system (pun intended) with
//this.
#define NGF_SERIALISE_STATE_SYSTEM()                                                           \
            std::vector<unsigned int> stateInds;                                               \
            int currState;                                                                     \
            int nextState;                                                                     \
                                                                                               \
            NGF_SERIALISE_ON_SAVE                                                              \
            {                                                                                  \
                for (std::vector<__State *>::iterator iter = __mStateStack.begin();            \
                        iter != __mStateStack.end(); ++iter)                                   \
                    stateInds.push_back((*iter)->getIndex());                                  \
                                                                                               \
                currState = mCurrState ? mCurrState->getIndex() : 0;                           \
                nextState = __mNextState ? __mNextState->getIndex() : 0;                       \
            }                                                                                  \
                                                                                               \
            NGF_SERIALISE_STL_CONTAINER(stateInds);                                            \
            NGF_SERIALISE_OGRE(Int, currState);                                                \
                                                                                               \
            NGF_SERIALISE_ON_LOAD                                                              \
            {                                                                                  \
                __mStateStack.clear();                                                         \
                for (std::vector<unsigned int>::iterator iter = stateInds.begin();             \
                        iter != stateInds.end(); ++iter)                                       \
                    __mStateStack.push_back(NGF_STATES_GET_STATE_FROM_INDEX(*iter));           \
                                                                                               \
                mCurrState = currState ? NGF_STATES_GET_STATE_FROM_INDEX(currState) : 0;       \
                __mNextState = nextState ? NGF_STATES_GET_STATE_FROM_INDEX(nextState) : 0;     \
            }

#endif //#ifndef __NGF_SERIALISATION_H__
