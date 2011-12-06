/*
 * =====================================================================================
 *
 *       Filename:  Ngf.cpp
 *
 *    Description:  NGF implementation
 *
 *        Version:  1.0
 *        Created:  10/22/2008 11:29:15 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "OgreScriptLoader.h"
#include "OgreResourceGroupManager.h"
#include "OgreLogManager.h"

#include "Ngf.h"

using namespace std;
using namespace Ogre;

template<> NGF::GameObjectFactory* Ogre::Singleton<NGF::GameObjectFactory>::SINGLETON_MEMBER = 0;
template<> NGF::GameObjectManager* Ogre::Singleton<NGF::GameObjectManager>::SINGLETON_MEMBER = 0;
template<> NGF::WorldManager* Ogre::Singleton<NGF::WorldManager>::SINGLETON_MEMBER = 0;

namespace NGF {

/*
 * =====================================================================================
 * NGF::PropertyList
 * =====================================================================================
 */

    Ogre::String PropertyList::getValue(Ogre::String key, unsigned int index, Ogre::String defaultVal)
    {
	    PropertyList::iterator itr = find(key);
	    if (itr != end())
	    {
                    std::vector<Ogre::String> values = itr->second;

		    if (index < values.size())
		    {
			    return values[index];
		    }
	    }

	    return defaultVal;
    }
    //----------------------------------------------------------------------------------   
    PropertyList & PropertyList::addProperty(Ogre::String key, Ogre::String values, 
		    Ogre::String delims)
    {
            std::vector<Ogre::String> vals;
            vals.reserve(10);

            unsigned int numSplits = 0;
            size_t start = 0, pos;
            do 
            {
                    pos = values.find_first_of(delims, start);
                    if (pos == start)
                    {
                            //Do nothing
                            start = pos + 1;
                    }
                    else if (pos == Ogre::String::npos)
                    {
                            //Copy the rest of the string
                            vals.push_back(values.substr(start));
                            break;
                    }
                    else
                    {
                            //Copy up to delimiter
                            vals.push_back(values.substr(start, pos - start));
                            start = pos + 1;
                    }
                    //Parse up to next real data
                    start = values.find_first_not_of(delims, start);
                    ++numSplits;

            } while (pos != Ogre::String::npos);

            insert(PropertyPair(key, vals));
            return *this;
    }
    //----------------------------------------------------------------------------------
    PropertyList PropertyList::create(Ogre::String key, Ogre::String values, Ogre::String delims)
    {
	    NGF::PropertyList props;
	    props.addProperty(key, values, delims);
	    return props;
    }

/*
 * =====================================================================================
 * NGF::GameObject
 * =====================================================================================
 */

    GameObject* GameObject::addFlag(Ogre::String flag)
    {
	    if (mFlags.empty())
	    {
		    mFlags = "|";
	    }
	    mFlags += (flag + "|");

	    return this;
    }
    //----------------------------------------------------------------------------------
    bool GameObject::removeFlag(Ogre::String flag)
    {
	    std::string::size_type pos1 = mFlags.find("|" + flag + "|");

	    if (pos1 == Ogre::String::npos)
	    {
		    return false;
	    }

	    ++pos1;
	    std::string::size_type pos2 = flag.length() + 1;

	    mFlags.erase(pos1, pos2);

	    return true;
    }
    //----------------------------------------------------------------------------------
    bool GameObject::hasFlag(Ogre::String flag) const
    {
	    return !(mFlags.find("|" + flag + "|") == Ogre::String::npos);
    }

/*
 * =====================================================================================
 * NGF::GameObjectFactory
 * =====================================================================================
 */

    GameObjectFactory& GameObjectFactory::getSingleton(void)
    {
	    assert(SINGLETON_MEMBER); return *SINGLETON_MEMBER;
    }
    GameObjectFactory* GameObjectFactory::getSingletonPtr(void)
    {
	    return SINGLETON_MEMBER;
    }
    //----------------------------------------------------------------------------------
    GameObject* GameObjectFactory::createObject(Ogre::String type, Ogre::Vector3 pos, Ogre::Quaternion rot, PropertyList props, Ogre::String name)
    {
	    CreateFunctionMap::iterator iter = mCreateFunctions.find(type);

	    if (iter != mCreateFunctions.end())
		    return iter->second(pos, rot, props, name); //Found.
	    return 0; //Not found.
    }
    //----------------------------------------------------------------------------------
    GameObject* GameObjectFactory::_createObject(Ogre::String type, ID id, Ogre::Vector3 pos, Ogre::Quaternion rot, PropertyList props, Ogre::String name)
    {
	    IDCreateFunctionMap::iterator iter = mIDCreateFunctions.find(type);

	    if (iter != mIDCreateFunctions.end())
		    return iter->second(id, pos, rot, props, name); //Found.
	    return 0; //Not found.
    }

/*
 * =====================================================================================
 * NGF::GameObjectManager
 * =====================================================================================
 */

    GameObjectManager* GameObjectManager::getSingletonPtr(void)
    {
	    return SINGLETON_MEMBER;
    }
    GameObjectManager& GameObjectManager::getSingleton(void)
    {
	    assert(SINGLETON_MEMBER); return *SINGLETON_MEMBER;
    }
    //----------------------------------------------------------------------------------
    GameObjectManager::GameObjectManager()
	    : mObjectFactory(new GameObjectFactory())
    {
    }
    //----------------------------------------------------------------------------------
    void GameObjectManager::tick(bool paused, const Ogre::FrameEvent & evt)
    {
	    std::map<ID,GameObject*>::iterator objIter;

	    for (objIter = mGameObjectMap.begin();
			    objIter != mGameObjectMap.end(); ++objIter)
	    {
		    GameObject *obj = objIter->second;

		    if (paused)
		    {
			    obj->pausedTick(evt);
		    }
		    else
		    {
			    obj->unpausedTick(evt);
		    }
	    }

	    std::vector<ID>::iterator iter;

	    for (iter = mObjectsToDestroy.begin();
			    iter != mObjectsToDestroy.end(); ++iter)
	    {
		    destroyObject(*iter);
	    }
	    mObjectsToDestroy.clear();
    }
    //----------------------------------------------------------------------------------
    bool GameObjectManager::destroyObject(ID objID)
    {
	    std::map<ID,GameObject*>::iterator objIter = mGameObjectMap.find(objID);

	    if (objIter == mGameObjectMap.end())
	    {
		    return false;
	    }
	    else
	    {
		    GameObject *obj = objIter->second;

		    mGameObjectMap.erase(objIter);
		    obj->destroy(); //For scripting, as scripting languages are GCed.
		    delete obj;

		    return true;
	    }
    }
    //----------------------------------------------------------------------------------
    void GameObjectManager::destroyAll(void)
    {
	    std::map<ID,GameObject*>::iterator objIter;

            std::map<ID,GameObject*> persistentObjects;

	    for (objIter = mGameObjectMap.begin();
			    objIter != mGameObjectMap.end();)
	    {
                GameObject *obj = objIter->second;

                if (obj->isPersistent()) //If it doesn't want to die...
		    persistentObjects.insert(*objIter);
                else
                    delete obj;

                mGameObjectMap.erase(objIter++);
	    }

            mGameObjectMap = persistentObjects;
    }
    //----------------------------------------------------------------------------------
    GameObject* GameObjectManager::getByID(ID objID) const
    {
	    std::map<ID,GameObject*>::const_iterator objIter = mGameObjectMap.find(objID);

	    return (objIter == mGameObjectMap.end()) ? NULL : objIter->second;
    }
    //----------------------------------------------------------------------------------
    void GameObjectManager::forEachGameObject(ForEachFunction func)
    {
	    std::map<ID,GameObject*>::iterator objIter;

	    for (objIter = mGameObjectMap.begin();
			    objIter != mGameObjectMap.end(); ++objIter)
	    {
		    func(objIter->second);
	    }
    }
    //----------------------------------------------------------------------------------
    void GameObjectManager::sendMessage(GameObject *obj, Message msg) const
    {
	    if (obj)
	    {
		    obj->receiveMessage(msg);
	    }
    }
    //----------------------------------------------------------------------------------
    GameObject* GameObjectManager::getByName(Ogre::String name)
    {
	    GameObject* findObj = NULL;
	    std::map<ID,GameObject*>::iterator objIter;

	    for (objIter = mGameObjectMap.begin();
			    objIter != mGameObjectMap.end(); ++objIter)
	    {
		    GameObject *obj = objIter->second;
		    if (obj->getName() == name)
		    {
			    findObj = obj;
			    break;
		    }
	    }

	    return findObj;
    }

/*
 * =====================================================================================
 * NGF::WorldManager
 * =====================================================================================
 */

    WorldManager& WorldManager::getSingleton(void)
    {
	    assert(SINGLETON_MEMBER); return *SINGLETON_MEMBER;
    }
    WorldManager* WorldManager::getSingletonPtr(void)
    {
	    return SINGLETON_MEMBER;
    }
    //----------------------------------------------------------------------------------
    WorldManager::WorldManager()
    {
	    shuttingdown = false;
	    stoppedLast = false;
    }
    //----------------------------------------------------------------------------------
    WorldManager::~WorldManager()
    {
	    std::vector<World*>::iterator iter;
	    for (iter = worlds.begin(); iter!= worlds.end(); ++iter)
	    {
		    delete *iter;
	    }
	    worlds.clear();
    }
    //----------------------------------------------------------------------------------
    void WorldManager::shutdown(void)
    {
	    shuttingdown = true;
    }
    //----------------------------------------------------------------------------------
    bool WorldManager::tick(const Ogre::FrameEvent &evt)
    {
	    if (!shuttingdown)
	    {
		    worlds[currentWorld]->tick(evt);
		    return true;
	    }
	    else
	    {
		    if (!stoppedLast)
		    {
			    worlds[currentWorld]->stop();
			    stoppedLast = true;
		    }

		    return false;
	    }
    }
    //----------------------------------------------------------------------------------
    void WorldManager::addWorld(World *newWorld)
    {
	    worlds.push_back(newWorld);
    }
    //----------------------------------------------------------------------------------
    void WorldManager::start(unsigned int firstWorld)
    {
	    if (firstWorld <= worlds.size() && firstWorld != -1)
	    {
		    currentWorld = firstWorld;
		    worlds[currentWorld]->init();
	    }
	    else
	    {
		    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Bad world index given", "NGF::WorldManager::start()");
	    }
    }
    //----------------------------------------------------------------------------------
    void WorldManager::nextWorld(void)
    {
	    if ((currentWorld + 1) < worlds.size())
	    {
		    worlds[currentWorld]->stop();
		    worlds[++currentWorld]->init();
	    }
	    else
	    {
		    shutdown();
	    }
    }
    //----------------------------------------------------------------------------------
    bool WorldManager::previousWorld(void)
    {
	    if(currentWorld != 0)
	    {
		    worlds[currentWorld]->stop();
		    worlds[--currentWorld]->init();

		    return true;
	    }
	    return false;
    }
    //----------------------------------------------------------------------------------
    void WorldManager::gotoWorld(unsigned int worldNumber)
    {
	    if (worldNumber < worlds.size() && worldNumber != -1)
	    {
		    worlds[currentWorld]->stop();
		    worlds[(currentWorld = worldNumber)]->init();
	    }
	    else
	    {
		    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Bad world index given", "NGF::WorldManager::gotoWorld()");
	    }
    }
    //----------------------------------------------------------------------------------
    void WorldManager::removeWorld(unsigned int worldNumber)
    {
	    if (worldNumber < worlds.size() && worldNumber != -1)
	    {
                if (worldNumber == currentWorld)
                    previousWorld();
                delete worlds[worldNumber];
                worlds.erase(worlds.begin() + worldNumber);
	    }
	    else
	    {
		    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Bad world index given", "NGF::WorldManager::removeWorld()");
	    }
    }

/*
 * =====================================================================================
 * NGF::Loading
 * =====================================================================================
 */

    namespace Loading {
        //The parser class definitions.
        class ConfigNode
        {
        public:
                ConfigNode(ConfigNode *parent, const Ogre::String &name = "untitled");
                ~ConfigNode();

                inline void setName(const Ogre::String &name)
                {
                        this->name = name;
                }

                inline Ogre::String &getName()
                {
                        return name;
                }

                inline void addValue(const Ogre::String &value)
                {
                        values.push_back(value);
                }

                inline void clearValues()
                {
                        values.clear();
                }

                inline std::vector<Ogre::String> &getValues()
                {
                        return values;
                }

                inline const Ogre::String &getValue(unsigned int index = 0)
                {
                        assert(index < values.size());
                        return values[index];
                }

                inline float getValueF(unsigned int index = 0)
                {
                        assert(index < values.size());
                        return Ogre::StringConverter::parseReal(values[index]);
                }

                inline double getValueD(unsigned int index = 0)
                {
                        assert(index < values.size());

                        std::istringstream str(values[index]);
                        double ret = 0;
                        str >> ret;
                        return ret;
                }

                inline int getValueI(unsigned int index = 0)
                {
                        assert(index < values.size());
                        return Ogre::StringConverter::parseInt(values[index]);
                }

                ConfigNode *addChild(const Ogre::String &name = "untitled", bool replaceExisting = false);
                ConfigNode *findChild(const Ogre::String &name, bool recursive = false);

                inline std::vector<ConfigNode*> &getChildren()
                {
                        return children;
                }

                inline ConfigNode *getChild(unsigned int index = 0)
                {
                        assert(index < children.size());
                        return children[index];
                }

                void setParent(ConfigNode *newParent);

                inline ConfigNode *getParent()
                {
                        return parent;
                }

        private:
                Ogre::String name;
                std::vector<Ogre::String> values;
                std::vector<ConfigNode*> children;
                ConfigNode *parent;

                int lastChildFound;  //The last child node's index found with a call to findChild()

                std::vector<ConfigNode*>::iterator _iter;
                bool _removeSelf;
        };
        class ConfigScriptLoader: public Ogre::ScriptLoader
        {
        public:
                ConfigScriptLoader(Ogre::String);
                ~ConfigScriptLoader();

                inline static ConfigScriptLoader &getSingleton() { return *singletonPtr; }
                inline static ConfigScriptLoader *getSingletonPtr() { return singletonPtr; }

                Ogre::Real getLoadingOrder() const;
                const Ogre::StringVector &getScriptPatterns() const;

                ConfigNode *getConfigScript(const Ogre::String &type, const Ogre::String &name);
                std::vector<std::string> getScriptsOfType(const Ogre::String &type);

                void parseScript(Ogre::DataStreamPtr &stream, const Ogre::String &groupName);

        private:
                static ConfigScriptLoader *singletonPtr;

                typedef std::map<std::string, std::vector<std::string> > ScriptMap;
                ScriptMap scriptListMap;

                Ogre::Real mLoadOrder;
                Ogre::StringVector mScriptPatterns;

                std::map<Ogre::String, ConfigNode*> scriptList;

                //Parsing
                char *parseBuff, *parseBuffEnd, *buffPtr;
                size_t parseBuffLen;

                enum Token
                {
                        TOKEN_Text,
                        TOKEN_NewLine,
                        TOKEN_OpenBrace,
                        TOKEN_CloseBrace,
                        TOKEN_EOF,
                };

                Token tok, lastTok;
                Ogre::String tokVal, lastTokVal;
                char *lastTokPos;

                void _parseNodes(ConfigNode *parent);
                void _nextToken();
                void _prevToken();
        };
        //----------------------------------------------------------------------------------
        Loader::Loader(LoaderHelperFunction help)
        {
                mHelper = help;
                mUseFactory = (help == NULL);
                mGameMgr = GameObjectManager::getSingletonPtr();
                new ConfigScriptLoader("*.ngf");
        }
        //----------------------------------------------------------------------------------
        void Loader::loadLevel(Ogre::String levelname, Ogre::Vector3 displace, Ogre::Quaternion rotate)
        {
                //Get the script and its children (the objects).
                ConfigNode *lvl = ConfigScriptLoader::getSingleton().getConfigScript("ngflevel", levelname);

                if (!lvl)
                {
                        OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, "NGF level not found!", "NGF::Loading::Loader::loadNGF()");
                        return;
                }

                std::vector<ConfigNode*> objs = lvl->getChildren();

                //Iterate through the children and do stuff.
                for (std::vector<ConfigNode*>::iterator i = objs.begin(); i != objs.end(); ++i)
                {
                        ConfigNode *obj = (*i);

                        //Get the type and name.
                        Ogre::String type = obj->findChild("type")->getValues()[0];
                        Ogre::String name = obj->findChild("name")->getValues()[0];

                        //Get the position.
                        std::vector<Ogre::String> posCoord = obj->findChild("position")->getValues();

                        Ogre::Vector3 pos(Ogre::StringConverter::parseReal(posCoord[0]),
                                        Ogre::StringConverter::parseReal(posCoord[1]),
                                        Ogre::StringConverter::parseReal(posCoord[2]));

                        //Displace it accordingly.
                        pos = rotate * pos;
                        pos += displace;

                        //Get the rotation.
                        std::vector<Ogre::String> rotCoord = obj->findChild("rotation")->getValues();

                        Ogre::Quaternion rot( Ogre::StringConverter::parseReal(rotCoord[0]),
                                        Ogre::StringConverter::parseReal(rotCoord[1]),
                                        Ogre::StringConverter::parseReal(rotCoord[2]),
                                        Ogre::StringConverter::parseReal(rotCoord[3]));

                        //Displace it accordingly.
                        rot = rot * rotate;

                        //Since there are property keys and each key has more than one value, we have a lot to do.
                        PropertyList properties;
                        ConfigNode *propNode = obj->findChild("properties");

                        //Some objects might not store properties.
                        if (propNode)
                        {
                                //We get the keys and iterate through them.
                                std::vector<ConfigNode*> props = propNode->getChildren();

                                for (std::vector<ConfigNode*>::iterator j = props.begin(); j != props.end(); ++j)
                                {
                                        //Put the key and its values in the map. Luckily, a ConfigNode can return an std::vector
                                        //containing all its values, so we don't have to iterate through them.
                                        ConfigNode *prop = (*j);
                                        properties.insert(PropertyPair(prop->getName(), prop->getValues()));
                                }
                        }
                        else
                        {
                                //Use an empty property list in case the object doesn't have any properties.
                                properties = PropertyList();
                        }

                        //Call the callback function.
                        if (mUseFactory)
                                mGameMgr->createObject(type, pos, rot, properties, name);
                        else
                                mHelper(type, name, pos, rot, properties);
                }
        }
        //----------------------------------------------------------------------------------
        std::vector<Ogre::String> Loader::getLevels()
        {
                return ConfigScriptLoader::getSingleton().getScriptsOfType("ngflevel");
        }
        //----------------------------------------------------------------------------------
        ConfigScriptLoader *ConfigScriptLoader::singletonPtr = NULL;
        //----------------------------------------------------------------------------------
        ConfigScriptLoader::ConfigScriptLoader(Ogre::String pattern = "*.object")
        {
                //Init singleton
                if (singletonPtr)
                        OGRE_EXCEPT(1, "Multiple ConfigScriptManager objects are not allowed", "ConfigScriptManager::ConfigScriptManager()");
                singletonPtr = this;

                //Register as a ScriptLoader
                mLoadOrder = 100.0f;
                mScriptPatterns.push_back(pattern);
                ResourceGroupManager::getSingleton()._registerScriptLoader(this);
        }
        //----------------------------------------------------------------------------------
        ConfigScriptLoader::~ConfigScriptLoader()
        {
                singletonPtr = NULL;

                //Delete all scripts
                std::map<String, ConfigNode*>::iterator i;
                for (i = scriptList.begin(); i != scriptList.end(); i++){
                        delete i->second;
                }
                scriptList.clear();

                //Unregister with resource group manager
                if (ResourceGroupManager::getSingletonPtr())
                        ResourceGroupManager::getSingleton()._unregisterScriptLoader(this);
        }
        //----------------------------------------------------------------------------------
        Real ConfigScriptLoader::getLoadingOrder() const
        {
                return mLoadOrder;
        }
        //----------------------------------------------------------------------------------
        const StringVector &ConfigScriptLoader::getScriptPatterns() const
        {
                return mScriptPatterns;
        }
        //----------------------------------------------------------------------------------
        ConfigNode *ConfigScriptLoader::getConfigScript(const String &type, const String &name)
        {
                std::map<String, ConfigNode*>::iterator i;

                String key = type + ' ' + name;
                i = scriptList.find(key);

                //If found..
                if (i != scriptList.end())
                        return i->second;
                else
                        return NULL;
        }
        //----------------------------------------------------------------------------------
        std::vector<std::string> ConfigScriptLoader::getScriptsOfType(const Ogre::String &type)
        {
                ScriptMap::iterator scripts = scriptListMap.find(type);

                if (scripts != scriptListMap.end())
                {
                        return (*scripts).second;
                }

                return std::vector<std::string>();
        }
        //----------------------------------------------------------------------------------
        void ConfigScriptLoader::parseScript(DataStreamPtr &stream, const String &groupName)
        {
                //Copy the entire file into a buffer for fast access
                parseBuffLen = stream->size();
                parseBuff = new char[parseBuffLen];
                buffPtr = parseBuff;
                stream->read(parseBuff, parseBuffLen);
                parseBuffEnd = parseBuff + parseBuffLen;

                //Close the stream (it's no longer needed since everything is in parseBuff)
                //stream->close(); //Commented out until ZipDataStream 'double close' problem is fixed.

                //Get first token
                _nextToken();
                if (tok == TOKEN_EOF)
                        return;

                //Parse the script
                _parseNodes(0);

                if (tok == TOKEN_CloseBrace)
                        OGRE_EXCEPT(1, "Parse Error: Closing brace out of place", "ConfigScript::load()");

                //Delete the buffer
                delete[] parseBuff;
        }
        //----------------------------------------------------------------------------------
        void ConfigScriptLoader::_nextToken()
        {
                lastTok = tok;
                lastTokVal = tokVal;
                lastTokPos = buffPtr;

                //EOF token
                if (buffPtr >= parseBuffEnd){
                        tok = TOKEN_EOF;
                        return;
                }

                //(Get next character)
                int ch = *buffPtr++;
                while (ch == ' ' || ch == 9){	//Skip leading spaces / tabs
                        ch = *buffPtr++;
                }

                //Newline token
                if (ch == '\r' || ch == '\n'){
                        do {
                                ch = *buffPtr++;
                        } while ((ch == '\r' || ch == '\n') && buffPtr < parseBuffEnd);
                        buffPtr--;

                        tok = TOKEN_NewLine;
                        return;
                }

                //Open brace token
                else if (ch == '{'){
                        tok = TOKEN_OpenBrace;
                        return;
                }

                //Close brace token
                else if (ch == '}'){
                        tok = TOKEN_CloseBrace;
                        return;
                }

                //Text token, verify valid char
                if (ch < 32 || ch > 122) 
                        OGRE_EXCEPT(1, "Parse Error: Invalid character", "ConfigScript::load()");

                tokVal = "";
                tok = TOKEN_Text;

                //Very hacky parsing here. Please don't use this as an example for parsing. :-)
                if (ch == '"') {
                        ch = *buffPtr++; //Skip the " character.
                        do {
                                //Skip comments
                                if (ch == '/'){
                                        int ch2 = *buffPtr;

                                        //C++ style comment (//)
                                        if (ch2 == '/'){
                                                buffPtr++;
                                                do {
                                                        ch = *buffPtr++;
                                                } while (ch != '\r' && ch != '\n' && buffPtr < parseBuffEnd);

                                                tok = TOKEN_NewLine;
                                                return;
                                        }
                                }

                                //Add valid char to tokVal
                                tokVal += ch;

                                //Next char
                                ch = *buffPtr++;
                        } while (ch >= 32 && ch <= 122 && ch != '"' && buffPtr < parseBuffEnd);
                }
                else
                {
                        do {
                                //Skip comments
                                if (ch == '/'){
                                        int ch2 = *buffPtr;

                                        //C++ style comment (//)
                                        if (ch2 == '/'){
                                                buffPtr++;
                                                do {
                                                        ch = *buffPtr++;
                                                } while (ch != '\r' && ch != '\n' && buffPtr < parseBuffEnd);

                                                tok = TOKEN_NewLine;
                                                return;
                                        }
                                }

                                //':' means 'string to end of line', and all next lines starting with ':'.
                                if (ch == ':')
                                {
                                        again:

                                        ++buffPtr;
                                        do {
                                                ch = *buffPtr++;
                                                if (ch != '\r' && ch != '\n')
                                                        tokVal += ch;
                                        } while (ch != '\r' && ch != '\n' && buffPtr < parseBuffEnd);

                                        //(Get next character)
                                        char *old = (buffPtr - 1);
                                        ch = *buffPtr++;
                                        while (ch == '\r' || ch == '\n' || ch == ' ' || ch == 9) {	//Skip any other stuff.
                                                ch = *buffPtr++;
                                        }

                                        if (ch == ':')
                                        {
                                                tokVal += '\n';
                                                goto again;
                                        }

                                        buffPtr = old;
                                        return;
                                }

                                //Add valid char to tokVal
                                tokVal += ch;

                                //Next char
                                ch = *buffPtr++;
                        } while (ch > 32 && ch <= 122 && buffPtr < parseBuffEnd);
                }

                buffPtr--;

                return;
        }
        //----------------------------------------------------------------------------------
        void ConfigScriptLoader::_prevToken()
        {
                tok = lastTok;
                tokVal = lastTokVal;
                buffPtr = lastTokPos;
        }
        //----------------------------------------------------------------------------------
        void ConfigScriptLoader::_parseNodes(ConfigNode *parent)
        {
                typedef std::pair<String, ConfigNode*> ScriptItem;

                while (1) {
                        switch (tok){
                        //Node
                        case TOKEN_Text:
                                //Add the new node
                                ConfigNode *newNode;
                                if (parent)
                                        newNode = parent->addChild(tokVal);
                                else
                                        newNode = new ConfigNode(0, tokVal);

                                //Get values
                                _nextToken();
                                while (tok == TOKEN_Text){
                                        newNode->addValue(tokVal);
                                        _nextToken();
                                }

                                //Add root nodes to scriptList
                                if (!parent){
                                        String key;

                                        if (newNode->getValues().empty())
                                        {
                                                key = newNode->getName() + ' ';
                                        }
                                        else
                                        {
                                                String name = newNode->getValues().front();
                                                String type = newNode->getName();

                                                key = type + ' ' + name;

                                                ScriptMap::iterator scripts = scriptListMap.find(type);

                                                if (scripts == scriptListMap.end())
                                                {
                                                        std::vector<std::string> newVector;
                                                        newVector.push_back(name);
                                                        scriptListMap.insert(std::pair<std::string, std::vector<std::string> >
                                                                        (type, newVector));
                                                }
                                                else
                                                {
                                                        ((*scripts).second).push_back(name);
                                                }
                                        }

                                        scriptList.insert(ScriptItem(key, newNode));
                                }

                                //Skip any blank spaces
                                while (tok == TOKEN_NewLine)
                                        _nextToken();

                                //Add any sub-nodes
                                if (tok == TOKEN_OpenBrace){
                                        //Parse nodes
                                        _nextToken();
                                        _parseNodes(newNode);

                                        //Skip blank spaces
                                        while (tok == TOKEN_NewLine)
                                                _nextToken();

                                        //Check for matching closing brace
                                        if (tok != TOKEN_CloseBrace)
                                                OGRE_EXCEPT(1, "Parse Error: Expecting closing brace", "ConfigScript::load()");
                                } else {
                                        //If it's not a opening brace, back up so the system will parse it properly
                                        _prevToken();
                                }

                                break;

                        //Out of place brace
                        case TOKEN_OpenBrace:
                                OGRE_EXCEPT(1, "Parse Error: Opening brace out of plane", "ConfigScript::load()");
                                break;

                        //Return if end of nodes have been reached
                        case TOKEN_CloseBrace:
                                return;

                        //Return if reached end of file
                        case TOKEN_EOF:
                                return;
                        }

                        //Next token
                        _nextToken();
                };
        }
        //----------------------------------------------------------------------------------
        ConfigNode::ConfigNode(ConfigNode *parent, const String &name)
        {
                ConfigNode::name = name;
                ConfigNode::parent = parent;
                _removeSelf = true;	//For proper destruction
                lastChildFound = -1;

                //Add self to parent's child list (unless this is the root node being created)
                if (parent != NULL){
                        parent->children.push_back(this);
                        _iter = --(parent->children.end());
                }
        }
        //----------------------------------------------------------------------------------
        ConfigNode::~ConfigNode()
        {
                //Delete all children
                std::vector<ConfigNode*>::iterator i;
                for (i = children.begin(); i != children.end(); i++){
                        ConfigNode *node = *i;
                        node->_removeSelf = false;
                        delete node;
                }
                children.clear();

                //Remove self from parent's child list
                if (_removeSelf && parent != NULL)
                        parent->children.erase(_iter);
        }
        //----------------------------------------------------------------------------------
        ConfigNode *ConfigNode::addChild(const String &name, bool replaceExisting)
        {
                if (replaceExisting) {
                        ConfigNode *node = findChild(name, false);
                        if (node)
                                return node;
                }
                return new ConfigNode(this, name);
        }
        //----------------------------------------------------------------------------------
        ConfigNode *ConfigNode::findChild(const String &name, bool recursive)
        {
                int indx, prevC, nextC;
                int childCount = (int)children.size();

                if (lastChildFound != -1){
                        //If possible, try checking the nodes neighboring the last successful search
                        //(often nodes searched for in sequence, so this will boost search speeds).
                        prevC = lastChildFound-1; if (prevC < 0) prevC = 0; else if (prevC >= childCount) prevC = childCount-1;
                        nextC = lastChildFound+1; if (nextC < 0) nextC = 0; else if (nextC >= childCount) nextC = childCount-1;
                        for (indx = prevC; indx <= nextC; ++indx){
                                ConfigNode *node = children[indx];
                                if (node->name == name) {
                                        lastChildFound = indx;
                                        return node;
                                }
                        }

                        //If not found that way, search for the node from start to finish, avoiding the
                        //already searched area above.
                        for (indx = nextC + 1; indx < childCount; ++indx){
                                ConfigNode *node = children[indx];
                                if (node->name == name) {
                                        lastChildFound = indx;
                                        return node;
                                }
                        }
                        for (indx = 0; indx < prevC; ++indx){
                                ConfigNode *node = children[indx];
                                if (node->name == name) {
                                        lastChildFound = indx;
                                        return node;
                                }
                        }
                } else {
                        //Search for the node from start to finish
                        for (indx = 0; indx < childCount; ++indx){
                                ConfigNode *node = children[indx];
                                if (node->name == name) {
                                        lastChildFound = indx;
                                        return node;
                                }
                        }
                }

                //If not found, search child nodes (if recursive == true)
                if (recursive){
                        for (indx = 0; indx < childCount; ++indx){
                                children[indx]->findChild(name, recursive);
                        }
                }

                //Not found anywhere
                return NULL;
        }
        //----------------------------------------------------------------------------------
        void ConfigNode::setParent(ConfigNode *newParent)
        {
                //Remove self from current parent
                parent->children.erase(_iter);

                //Set new parent
                parent = newParent;

                //Add self to new parent
                parent->children.push_back(this);
                _iter = --(parent->children.end());
        }

    }
} //namespace NGF
