/*
 * =====================================================================================
 *
 *       Filename:  Ngf.h
 *
 *    Description:  The NGF framework header.
 *
 *        Created:  10/22/2008 11:32:56 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef _NGF_H_
#define _NGF_H_

#include <map>
#include <vector>

#include "OgreSingleton.h"
#include "OgreException.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreFrameListener.h"
#include "OgreStringConverter.h"

#include "boost/any.hpp"

#include "FastDelegate.h"

//Name of singleton member depends on Ogre version.
#if (OGRE_VERSION < ((1 << 16) | (8 << 8)))
#define SINGLETON_MEMBER ms_Singleton
#else
#define SINGLETON_MEMBER msSingleton
#endif

//Register a GameObject type. For easy registration if you're too lazy to type both the
//typename and a string. ;-)
#define NGF_REGISTER_OBJECT_TYPE(type) NGF::GameObjectFactory::getSingleton().registerObjectType< class type >( #type )

//Allows NGF_MESSAGE(MSG_SETTRANSFORM, Vector3(10,20,30), Quaternion(1,2,3,4))
#define NGF_MESSAGE(name, ...) (NGF::Message( name ), ##__VA_ARGS__)

//For consistency with 'NGF_NO_REPLY', because 'return boost::any(something)' looks different.
#define NGF_SEND_REPLY return boost::any

//For 'void messages'.
#define NGF_NO_REPLY() return boost::any()

namespace NGF {

/*
 * =====================================================================================
 *        Class: PropertyList
 *  Description: A map of Strings and StringVectors.
 * =====================================================================================
 */

typedef std::pair<Ogre::String, std::vector<Ogre::String> > PropertyPair;

class PropertyList : public std::map<Ogre::String, std::vector<Ogre::String> >
{
public:
	PropertyList() { }

	//Convert from 'usual' map.
	PropertyList(std::map<Ogre::String, std::vector<Ogre::String> > &x) { this->swap(x); }

	//Get a value. It returns defaultVal if the key isn't found or the index is out of bounds.
	Ogre::String getValue(Ogre::String key, unsigned int index, Ogre::String defaultVal);
    
	//Add a property. Specify the key, and the values (seperated by delemiters specified in delims).
	//You can chain this method like so: props.addProperty(x,y).addProperty(a,b).addProperty(m,n).
	PropertyList & addProperty(Ogre::String key, Ogre::String values, Ogre::String delims = " ");
	PropertyList & operator()(Ogre::String key, Ogre::String values, Ogre::String delims = " ")
        { return addProperty(key, values, delims); }

	//Allows you to quickly create a new PropertyList. Same parameters as 'addProperty',
	//but just creates a new PropertyList instead of adding to an existing one.
	static PropertyList create(Ogre::String key, Ogre::String values, Ogre::String delims = " ");
};

/*
 * =====================================================================================
 *       Struct: Message
 *  Description: This struct represents the Messages that are sent between the
 *               GameObjects. It uses the MessageParams typedef, which allows you to
 *               send any number of copy-constructible (Vector3, Quaternion etc) objects
 *		 in a message. This requres boost.
 *
 *		 Messages also have an unsigned 'code' field so you can use them with
 *		 enums and 'switch'. It is recommended to use '0' as a 'no code' field, 
 *		 because that's the default in case no code is given.
 * =====================================================================================
 */

typedef std::vector<boost::any> MessageParams;

struct Message
{
	Ogre::String name;
	MessageParams params;
	unsigned int code;

	//Create a Message with the given name or code and parameters.
	
	Message(Ogre::String nm, MessageParams parameters = MessageParams())
	    : name(nm), code(0), params(parameters) { }
	Message(unsigned int cod, MessageParams parameters = MessageParams())
	    : name(""), code(cod), params(parameters) { }

	//These methods allow you to get the name, code and parameters respectively.
	template<typename T> T getParam(int index) { return boost::any_cast<T>(params[index]); }

	//So you can do:
	// gom->sendMessage(obj, (NGF::Message("printStuff"), Vector3(10,20,30), Quaternion(1,2,3,4)));
	template<typename T> Message& operator,(T thing) { params.push_back(boost::any(thing)); return *this; }
};

/*
 * =====================================================================================
 *        Class: GameObject
 *  Description: This class acts as a base class for an object in the game. This class 
 *  		 is one of the most important classes since the NGF system was chosen to
 *  		 be object-centric.
 * =====================================================================================
 */

//Each GameObject has a unique ID.
typedef unsigned int ID;

//So users don't have to know about boost.
typedef boost::any MessageReply;

class GameObject
{
	ID mID;
	Ogre::String mType;
	Ogre::String mFlags;
	Ogre::String mName;
        bool mPersistent;

	friend class GameObjectManager;

protected:
	PropertyList mProperties;

public:
	//------ Called by the Framework, to be overridden --------

	//Called on creation.
	GameObject(Ogre::Vector3, Ogre::Quaternion, ID id, PropertyList properties = PropertyList(), Ogre::String name = "")
	    : mID(id),
	      mName(name),
	      mProperties(properties),
	      mType("NGF::GameObject"),
              mPersistent(false)
	{
	}

	//Called on destruction.
	virtual ~GameObject() { }

	//Called every unpaused frame.
	virtual void unpausedTick(const Ogre::FrameEvent& evt ) { }

	//Called every paused frame.
	virtual void pausedTick(const Ogre::FrameEvent& evt) { }

	//Called when a message is received.
	virtual MessageReply receiveMessage(Message msg) { NGF_NO_REPLY(); }

	//Called on destruction (for scripted objects, as they are GCed).
	virtual void destroy(void) { }

	//------ Called by other objects, and not overridden ------

	//Returns the ID of the  GameObject.
	ID getID(void) const { return mID; }

	//Returns the name of the  GameObject.
	Ogre::String getName(void) const { return mName; }

	//Returns the properties of the GameObject.
	PropertyList getProperties(void) const { return mProperties; }

	//Adds a flag to the GameObject's flags.
	GameObject* addFlag(Ogre::String flag);

	//Removes a flag from the GameObject's flags. Returns whether it was found.
	bool removeFlag(Ogre::String flag);

	//Checks whether the GameObject has a flag.
	bool hasFlag(Ogre::String flag) const;

	//Returns the flags string.
	Ogre::String getFlags() const { return mFlags; }

        //Set persistent (not destroyed when you call 'destroyAll').
        void setPersistent(bool persistent) { mPersistent = persistent; }

        //Check whether persistent.
        bool isPersistent() { return mPersistent; }
};

/*
 * =====================================================================================
 *        Class: GameObjectactory
 *  Description: Manages creation of GameObjects with the type as a string. This is
 *  		 useful for creating GameObjects by parsing a text file, or in a
 *  		 scripting language, for instance.
 * =====================================================================================
 */

class GameObjectFactory : public Ogre::Singleton<GameObjectFactory>
{
protected:
	typedef std::map<Ogre::String, fastdelegate::FastDelegate< 
		GameObject* (Ogre::Vector3 , Ogre::Quaternion , PropertyList, Ogre::String) > > 
		CreateFunctionMap;
	CreateFunctionMap mCreateFunctions;

	typedef std::map<Ogre::String, fastdelegate::FastDelegate< 
		GameObject* (ID, Ogre::Vector3 , Ogre::Quaternion , PropertyList, Ogre::String) > > 
		IDCreateFunctionMap;
	IDCreateFunctionMap mIDCreateFunctions;

public:
	//Register a GameObject type. Give the class as the template parameter, and the
	//string name of the type as the string parameter. You can then use
	//GameObjectManager::createObject to create an object of this type by passing a
	//string.
	template<typename T>
	void registerObjectType(Ogre::String type);

	//Create an object with the given type as a string. The type should be registered. 
	//Use GameObjectManager::createObject instead for consistency. This is similar to 
	//GameObjectManager::createObject's template version, just the way the type is 
	//specified is different.
	NGF::GameObject *createObject(Ogre::String type, Ogre::Vector3 pos, 
			Ogre::Quaternion rot, PropertyList props, Ogre::String name);

	//Create an object with the given type as a string, and given ID. The type
	//should be registered.  Use GameObjectManager::createObject instead for
	//consistency. This is similar to GameObjectManager::createObject's template
	//version, just the way the type is specified is different.
        //
        //Use this only if you're sure you know what you're doing!
	NGF::GameObject *_createObject(Ogre::String type, ID id, Ogre::Vector3 pos, 
			Ogre::Quaternion rot, PropertyList props, Ogre::String name);
		
	//------ Singleton functions ------------------------------
	
	static GameObjectFactory& getSingleton(void);
	static GameObjectFactory* getSingletonPtr(void);
};


/*
 * =====================================================================================
 *        Class: GameObjectManager
 *  Description: Manages and handles all GameObjects, and notifies them of
 *  		 events, including collision events.
 * =====================================================================================
 */

class GameObjectManager : public Ogre::Singleton<NGF::GameObjectManager>
{
protected:
	std::map<ID,GameObject*> mGameObjectMap;
	GameObjectFactory *mObjectFactory;

	std::vector<ID> mObjectsToDestroy;

public:

	typedef fastdelegate::FastDelegate1<GameObject*> ForEachFunction;

	//------ Constructor/Destructor ---------------------------
	
	GameObjectManager();

	~GameObjectManager() { destroyAll(); delete mObjectFactory; }

	//------ Tick function ------------------------------------

	//Updates the GameObjects. Should be called per frame. Tell it whether
	//the game is paused, and pass it the Ogre::FrameEvent.
	void tick(bool paused, const Ogre::FrameEvent & evt);

	//------ Singleton functions ------------------------------

	static GameObjectManager* getSingletonPtr(void);
	static GameObjectManager& getSingleton(void);

	//------ Create/Destroy functions -------------------------

	//Creates a GameObject of the given type. Returns a pointer to the GameObject created.
	//Give name "noname" if you want the GameObject to not have a name.
	template<typename T>
	GameObject* createObject(Ogre::Vector3 pos, Ogre::Quaternion rot, PropertyList properties = PropertyList(), 
		Ogre::String name = "");

	//Creates a GameObject of the given type as a string. Returns a pointer to the 
	//GameObject created. Give name "noname" if you want the GameObject to not have a name.
	GameObject* createObject(Ogre::String type, Ogre::Vector3 pos, Ogre::Quaternion rot, 
		PropertyList properties = PropertyList(), Ogre::String name = "")
	{
		return mObjectFactory->createObject(type, pos, rot, properties, name);
	}

	//Creates a GameObject of the given type and ID. Returns a pointer to the GameObject
	//created.  Give name "noname" if you want the GameObject to not have a name.
        //
        //Use this only if you're sure you know what you're doing!
	template<typename T>
	GameObject* _createObject(ID id, Ogre::Vector3 pos, Ogre::Quaternion rot, PropertyList properties = PropertyList(), 
		Ogre::String name = "");

	//Creates a GameObject of the given type as a string, and given ID. Returns a
	//pointer to the GameObject created. Give name "noname" if you want the GameObject
	//to not have a name.
        //
        //Use this only if you're sure you know what you're doing!
	GameObject* _createObject(Ogre::String type, ID id, Ogre::Vector3 pos, Ogre::Quaternion rot, 
		PropertyList properties = PropertyList(), Ogre::String name = "")
	{
		return mObjectFactory->_createObject(type, id, pos, rot, properties, name);
	}

	//Destroys the GameObject with the given ID.
	//Returns false if it was not found and true if it was.
	bool destroyObject(ID objID);

	//Requests the GameObjectManager to destroy a GameObject soon. This is needed
	//if an object wants to destroy itself, or for other crazy situations.
	void requestDestroy(ID objID) { mObjectsToDestroy.push_back(objID); }

	//Destroys all the GameObjects that exist.
	void destroyAll(void);

	//------ Miscellaneous functions --------------------------

	//Returns a pointer to the GameObject with the given ID. If it was
	//not found, a NULL pointer will be returned.
	GameObject* getByID(ID objID) const;

	//Returns a pointer to the GameObject with the given name. If not found,
	//a NULL pointer is returned.
	GameObject* getByName(Ogre::String name);

	//Calls the function passed for each GameObject that exists. One argument
	//is passed to that function, which is the GameObject. Quite useful if
	//used with boost::lambda.
	void forEachGameObject(ForEachFunction func);

	//------ Messaging functions ------------------------------

	//This function sends a message to the GameObject.
	void sendMessage(GameObject *obj, Message msg) const;

	//This function sends the message, and gives a reply. GameObjects can reply using 
	//NGF_SEND_REPLY(reply) in a GameObject::receiveMessage() function.
	template<class ReturnType>
	ReturnType sendMessageWithReply(GameObject *obj, Message msg);

};

/*
 * =====================================================================================
 *        Class: World
 *  Description: This class acts as a base class for all Worlds (levels).
 * =====================================================================================
 */

class World
{
public:
	//The World constructor. This is called when the World is constructed, not when
	//it is run (look for World::init).
	World() { }

	//Called when the World is destroyed. Usually when the WorldManager is destroyed.
	//This is not called when the World ends (look for World::stop);
	virtual ~World() { }

	//Called when the World is run.
	virtual void init(void) { }

	//Called every frame the World is running.
	virtual void tick(const Ogre::FrameEvent &evt) { }

	//Called when the World stops running, that is, when we switch to a different World.
	virtual void stop(void) { }
};

/*
 * =====================================================================================
 *        Class: WorldManager
 *  Description: This class manages the Worlds, and plays them in order.
 * =====================================================================================
 */

class WorldManager : public Ogre::Singleton<NGF::WorldManager>
{
protected:
	unsigned int currentWorld;
	std::vector<World*> worlds;
	bool shuttingdown;
	bool stoppedLast;

public:
	WorldManager();
	~WorldManager();

	//Shutdown. Doesn't really shutdown, it just makes it return false
	//from the tick function, you gotta do the actual shutdown yourself.
	void shutdown();

	//Tick function. Call it every frame. Shutdown if it returns false.
	bool tick(const Ogre::FrameEvent &evt);

	//Add a world to the list. You can just call addWorld(new MyWorld()), and
	//not have to manually call delete worldPointer. All added Worlds are
	//automatically deleted by the WorldManager when it is destroyed.
	void addWorld(World *newWorld);
	
	//Start playing the worlds.
	void start(unsigned int firstWorld);

	//Go to the next world.
	void nextWorld();

	//Go to the previous world.
	bool previousWorld();

	//Jump to the specified world.
	void gotoWorld(unsigned int worldNumber);

	//Get the index number of the currently playing world.
	unsigned int getCurrentWorldIndex() { return currentWorld; }

	//Get the number of registered worlds.
	unsigned int getNumWorlds() { return worlds.size(); }

        //Get a pointer to the given world.
        World *getWorld(unsigned int i) { return worlds[i]; }

        //Remove the world with the given index.
        void removeWorld(unsigned int worldNumber);

	//------ Singleton functions ------------------------------
	
	static WorldManager& getSingleton(void);
	static WorldManager* getSingletonPtr(void);
};

namespace Loading {

//The LoaderHelperFunctions. I know, I should have called it LoaderCallbackFunction.
//At that time, I didn't think of 'callback' for some reason (probably wasn't very familiar with programming jargon),
//and the name stuck. 
typedef fastdelegate::FastDelegate<void (Ogre::String,Ogre::String,Ogre::Vector3,Ogre::Quaternion,PropertyList) > LoaderHelperFunction;

/*
 * =====================================================================================
 *        Class: Loader
 *  Description: This class parses .ngf files and calls the callback function
 *               accordingly. If the type of object is 'Mesh', it makes a static mesh
 *		 with the given meshfile (this feature removed).
 * =====================================================================================
 */

class Loader
{
protected:
	LoaderHelperFunction mHelper;
	GameObjectManager *mGameMgr;

	bool mUseFactory;

public:
	//Create the loader. Give it a pointer to the helper function, or NULL (0) if you want it to use the 
	//GameObjectFactory (through GameObjectManager::createObject(<string>, ...)).
	Loader(LoaderHelperFunction help = 0);

	//Whether to use factory or not. If no, you provide the callback (helper) function. Otherwise,
	//we use the GameObjectFactory (through GameObjectManager::createObject(<string>, ...)).
	void useFactory(bool use, LoaderHelperFunction help = 0) { mUseFactory = use; mHelper = help; }

	//Loads an NGF level. Give it the name of the level in the '.ngf' script. You can also give an additional
	//positional displacement and rotation. This can be useful for loading when a level is already loaded, to
	//'add on' to the existing level.
	void loadLevel(Ogre::String levelname, Ogre::Vector3 displace = Ogre::Vector3::ZERO, Ogre::Quaternion rotate = Ogre::Quaternion::IDENTITY);

	//Returns a vector containing the level names of all the levels parsed. Returns an empty vector if no levels
	//were found.
	std::vector<Ogre::String> getLevels();
};

} //namespace Loading

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//-+-+-+ End of the 'actual' header file, template function definitions follow +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

template<typename T>
void GameObjectFactory::registerObjectType(Ogre::String type)
{
	mCreateFunctions[type] = fastdelegate::MakeDelegate(GameObjectManager::getSingletonPtr(), &GameObjectManager::createObject<T>);
	mIDCreateFunctions[type] = fastdelegate::MakeDelegate(GameObjectManager::getSingletonPtr(), &GameObjectManager::_createObject<T>);
}
//--------------------------------------------------------------------------------------
template<typename T>
GameObject* GameObjectManager::createObject(Ogre::Vector3 pos, Ogre::Quaternion rot, 
	PropertyList properties, Ogre::String name)
{
	//Calculate ID to assign. Just search for an unused ID.
        ID id = 0;
        for (; mGameObjectMap.find(id) != mGameObjectMap.end(); ++id);

        return _createObject<T>(id, pos, rot, properties, name);
}
//--------------------------------------------------------------------------------------
template<typename T>
GameObject* GameObjectManager::_createObject(ID id, Ogre::Vector3 pos, Ogre::Quaternion rot, 
	PropertyList properties, Ogre::String name)
{
	//No name.
	name = ((name == "noname" ) ? "" : name);

	//Check if name is already used.
	if (name != "")
	{
		std::map<ID,GameObject*>::iterator objIter;

		for (objIter = mGameObjectMap.begin();
				   objIter != mGameObjectMap.end(); ++objIter)
		{
			GameObject *obj = objIter->second;
			if (obj->getName() == name)
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM, "GameObject with name'" 
					+ name + "' already exists!", "NGF::GameObjectManager::createObject()");
			}
		}
	}

	//Create object.
	T *obj = new T(pos, rot, id, properties, name);

	//Check if name and ID was correctly passed.
	if ((obj->getID() != id) || (obj->getName() != name))
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Incorrect name or ID passed for GameObject with ID: " 
			+ Ogre::StringConverter::toString(obj->getID()) + ", and name: '" + obj->getName() 
			+ "'.", "NGF::GameObjectManager::createObject()");
	}

	//Put in map.
	mGameObjectMap.insert(std::pair<ID,GameObject*>(id, obj));

	return obj;
}
//--------------------------------------------------------------------------------------
template<typename ReturnType>
ReturnType GameObjectManager::sendMessageWithReply(GameObject *obj, Message msg)
{
	if (obj)
	{
		boost::any reply = obj->receiveMessage(msg);

		if (reply.empty())
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "No reply!", "NGF::GameObjectManager::sendMessageWithReply()");

		try
		{
			return boost::any_cast<ReturnType>(reply);
		}
		catch (boost::bad_any_cast)
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Bad ReturnType!", "NGF::GameObjectManager::sendMessageWithReply()");
		}

	}
	OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "GameObject doesn't exist!", "NGF::GameObjectManager::sendMessageWithReply()");
}

} //namespace NGF

#endif //#define _NGF_H_
