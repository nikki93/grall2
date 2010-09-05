/*
 * =====================================================================================
 *
 *       Filename:  NgfSerialisation.cpp
 *
 *    Description:  NGF-Serialisation implementation
 *
 *        Version:  1.0
 *        Created:  03/31/2009 01:02:37 PM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "ngfplugins/NgfSerialisation.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <fstream>

std::vector<NGF::Serialisation::GameObjectRecord> NGF::Serialisation::Serialiser::mRecords 
        = std::vector<GameObjectRecord>();

namespace NGF { namespace Serialisation {

/*
 * =====================================================================================
 * NGF::Serialisation::Serialiser
 * =====================================================================================
 */

    void Serialiser::save(Ogre::String filename)
    {
            mRecords.clear();

            //Save each GameObject into mRecords (as a GameObjectRecord).
            GameObjectManager::getSingleton().forEachGameObject(_saveOne);

            //Serialise mRecords (a vector of GameObjectRecords), effectively saving
            //all GameObject information.
            std::ofstream ofs(filename.c_str());
            boost::archive::text_oarchive oa(ofs);
            oa << mRecords;
    }
    //----------------------------------------------------------------------------------   
    void Serialiser::load(Ogre::String filename)
    {
            mRecords.clear();

            {
                    //Load back the saved information into mRecords.
                    std::ifstream ifs(filename.c_str());
                    boost::archive::text_iarchive ia(ifs);
                    ia >> mRecords;
            }

            //We create 'em all first, and /then/ call the 'deserialise' function, so that
            //GameObjects can assume other GameObjects to exist (otherwise order-of-deserialisation
            //might cause dependency problems).

            //Stores the created objects and their info so we can tell 'em afterward.
            //typedef std::map<NGF::ID*, PropertyList> InfoMap;
            //InfoMap infos;

            for (std::vector<GameObjectRecord>::iterator iter = mRecords.begin(); iter != mRecords.end(); ++iter)
            {
                    GameObjectRecord rec = *iter;
                    PropertyList info = rec.getInfo();

                    //Restore the position and rotation.
                    Ogre::Vector3 pos 
                        = Ogre::StringConverter::parseVector3(info.getValue("NGF_POSITION", 0, "0 0 0"));
                    Ogre::Quaternion rot 
                        = Ogre::StringConverter::parseQuaternion(info.getValue("NGF_ROTATION", 0, "1 0 0 0"));

                    //Create GameObject with restored type, position, rotation, properties, name.
                    SerialisableGameObject *obj = dynamic_cast<SerialisableGameObject*>
                            (GameObjectManager::getSingleton()._createObject(rec.mType, rec.mID, pos, rot, rec.mProps, rec.mName));
                    obj->setPersistent(rec.mPersistent);

                    //Save this object and it's info so we can call the 'deserialise' later.
                    //if (obj)
                        //infos[obj] = info;
            } 

            for (std::vector<GameObjectRecord>::iterator iter = mRecords.begin(); iter != mRecords.end(); ++iter)
            {
                    GameObjectRecord rec = *iter;
                    PropertyList info = rec.getInfo();
                    SerialisableGameObject *obj = dynamic_cast<SerialisableGameObject*>
                        (GameObjectManager::getSingleton().getByID(rec.mID));

                    if (obj)
                        obj->serialise(false, info);
            } 
    }
    //----------------------------------------------------------------------------------   
    void Serialiser::_saveOne(GameObject *o)
    {
            SerialisableGameObject *obj = dynamic_cast<SerialisableGameObject*>(o);

            if (obj)
            {
                    PropertyList dummy;

                    //Tell the GameObject to write its stuff.
                    GameObjectRecord rec = obj->serialise(true, dummy);

                    //Save what we can save ourselves (ID, name, properties). Add something to the propertylist
                    //to allow GameObjects to know they're being loaded if they need to.
                    rec.mID = obj->getID();
                    rec.mName = obj->getName();
                    rec.mPersistent = obj->isPersistent();

                    PropertyList props = obj->getProperties();
                    props.addProperty("NGF_SERIALISED", "yes", "");
                    rec.mProps = props;

                    //Add the record to our records-holder.
                    mRecords.push_back(rec);
            }
    }

} //namespace Serialisation

} //namespace NGF
