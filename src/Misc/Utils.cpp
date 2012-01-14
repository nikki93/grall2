/*
=========================================
Utils.cpp
=========================================
*/

#include "Misc/Utils.h"

#include "Objects/Misc/MessageBox.h"
#include "Worlds/Level.h"

namespace Util {

NGF::GameObject *showMessage(Ogre::String message, Ogre::Real time)
{
    return GlbVar.goMgr->createObject("MessageBox", Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("message", message, "")
            ("time", Ogre::StringConverter::toString(time))
            );
}

Ogre::String checkpointName(unsigned int worldNum)
{
    //Makes savefile name from world number of Level.
    Level *lvl = dynamic_cast<Level*>(GlbVar.woMgr->getWorld(worldNum));
    if (lvl)
        return lvl->getNgfName();
    return "NULL";
}

void screenshot(Ogre::String filename, const Ogre::String &extension)
{
    //Keep a count.
    static int count = -1;
    ++count;

    //Write it.
    filename = (USER_PREFIX "Screenshots/") + filename + Ogre::StringConverter::toString(count) + extension;
    GlbVar.ogreWindow->writeContentsToFile(filename);
}

void highResScreenshot(Ogre::RenderWindow* pRenderWindow, Ogre::Camera* pCamera, const int& pGridSize, Ogre::String pFileName, const Ogre::String& pFileExtension, const bool& pStitchGridImages)
{
    //Keep a count.
    static int count = -1;
    ++count;
    pFileName = (USER_PREFIX "Screenshots/") + pFileName + Ogre::StringConverter::toString(count);

    //Don't want mouse cursor showing up in the shot.
    MyGUI::PointerManager::getInstance().setVisible(false);

    Ogre::String gridFilename;

    if(pGridSize <= 1)
    {
        // Simple case where the contents of the screen are taken directly
        // Also used when an invalid value is passed within pGridSize (zero or negative grid size)
        gridFilename = pFileName + pFileExtension;

        pRenderWindow->writeContentsToFile(gridFilename);
    }
    else
    {
        // Generate a grid of screenshots
        pCamera->setCustomProjectionMatrix(false); // reset projection matrix
        Ogre::Matrix4 standard = pCamera->getProjectionMatrix();
        double nearDist = pCamera->getNearClipDistance();
        double nearWidth = (pCamera->getWorldSpaceCorners()[0] - pCamera->getWorldSpaceCorners()[1]).length();
        double nearHeight = (pCamera->getWorldSpaceCorners()[1] - pCamera->getWorldSpaceCorners()[2]).length();
        Ogre::Image sourceImage;
        Ogre::uchar* stitchedImageData;

        // Process each grid
        for (int nbScreenshots = 0; nbScreenshots < pGridSize * pGridSize; nbScreenshots++) 
        { 
            // Use asymmetrical perspective projection.
            int y = nbScreenshots / pGridSize; 
            int x = nbScreenshots - y * pGridSize; 
            Ogre::Matrix4 shearing( 
                    1, 0,(x - (pGridSize - 1) * 0.5) * nearWidth / nearDist, 0, 
                    0, 1, -(y - (pGridSize - 1) * 0.5) * nearHeight / nearDist, 0, 
                    0, 0, 1, 0, 
                    0, 0, 0, 1); 
            Ogre::Matrix4 scale( 
                    pGridSize, 0, 0, 0, 
                    0, pGridSize, 0, 0, 
                    0, 0, 1, 0, 
                    0, 0, 0, 1); 
            pCamera->setCustomProjectionMatrix(true, standard * shearing * scale);
            Ogre::Root::getSingletonPtr()->_updateAllRenderTargets();
            gridFilename = pFileName + Ogre::StringConverter::toString(nbScreenshots) + pFileExtension;


            // Screenshot of the current grid
            pRenderWindow->writeContentsToFile(gridFilename);

            if(pStitchGridImages)
            {
                // Automatically stitch the grid screenshots
                sourceImage.load(gridFilename, "General"); // Assumes that the current directory is within the "General" resource group
                int sourceWidth = (int) sourceImage.getWidth();
                int sourceHeight = (int) sourceImage.getHeight();
                Ogre::ColourValue colourValue;
                int stitchedX, stitchedY, stitchedIndex;

                // Allocate memory for the stitched image when processing the screenshot of the first grid
                if(nbScreenshots == 0)
                    stitchedImageData = new Ogre::uchar[(sourceImage.getWidth() * pGridSize) * (sourceImage.getHeight() * pGridSize) * 3]; // 3 colors per pixel

                // Copy each pixel within the grid screenshot to the proper position within the stitched image
                for(int rawY = 0; rawY < sourceHeight; ++rawY)
                {
                    for(int rawX = 0; rawX < sourceWidth; ++rawX)
                    {
                        colourValue = sourceImage.getColourAt(rawX, rawY, 0);
                        stitchedX = x * sourceWidth + rawX;
                        stitchedY = y * sourceHeight + rawY;
                        stitchedIndex = stitchedY * sourceWidth * pGridSize + stitchedX;
                        Ogre::PixelUtil::packColour(colourValue,
                                Ogre::PF_R8G8B8,
                                (void*) &stitchedImageData[stitchedIndex * 3]);
                    }
                }
                // The screenshot of the grid is no longer needed
                remove(gridFilename.c_str());
            }
        } 
        pCamera->setCustomProjectionMatrix(false); // reset projection matrix 

        if(pStitchGridImages)
        {
            // Save the stitched image to a file
            Ogre::Image targetImage;
            targetImage.loadDynamicImage(stitchedImageData,
                    sourceImage.getWidth() * pGridSize,
                    sourceImage.getHeight() * pGridSize,
                    1, // depth
                    Ogre::PF_R8G8B8,
                    false);
            targetImage.save(pFileName + pFileExtension);
            delete[] stitchedImageData;
        }
    }

    MyGUI::PointerManager::getInstance().setVisible(true);
}

//--- Resource reloading ---

using namespace Ogre;

static void UnloadResource(Ogre::ResourceManager* resMgr, const std::string& resourceName)
{
    Ogre::ResourcePtr rPtr = resMgr->getByName(resourceName);
    if (rPtr.isNull())
    {
        StringUtil::StrStreamType msg;
        msg << "Resource no longer exists: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
        return;
    }

    rPtr->unload();
    if (rPtr->isLoaded())
    {
        StringUtil::StrStreamType msg;
        msg << "Resource was not unloaded: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }
    else
    {
        StringUtil::StrStreamType msg;
        msg << "Resource was unloaded: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }

    resMgr->remove(resourceName);
    rPtr = resMgr->getByName(resourceName);
    if (rPtr.isNull())
    {
        StringUtil::StrStreamType msg;
        msg << "Resource was removed: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }
    else
    {
        StringUtil::StrStreamType msg;
        msg << "Resource was not removed: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }

    if (resMgr->resourceExists(resourceName))
    {
        StringUtil::StrStreamType msg;
        msg << "Resource still exists: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }
    else
    {
        StringUtil::StrStreamType msg;
        msg << "Resource no longer exists: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }
}

static void LoadResource(Ogre::ResourceManager* resMgr, const std::string& resourceName, const std::string& resourceGroup)
{
    Ogre::ResourcePtr rPtr = resMgr->load(resourceName, resourceGroup);
    if (rPtr.isNull())
    {
        StringUtil::StrStreamType msg;
        msg << "Resource no longer exists: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
        return;
    }

    rPtr->escalateLoading();
    rPtr->reload();
    if (rPtr->isLoaded())
    {
        StringUtil::StrStreamType msg;
        msg << "Resource was reloaded: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }
    else
    {
        StringUtil::StrStreamType msg;
        msg << "Resource was not reloaded: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }

    if (resMgr->resourceExists(resourceName))
    {
        StringUtil::StrStreamType msg;
        msg << "Resource still exists: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }
    else
    {
        StringUtil::StrStreamType msg;
        msg << "Resource no longer exists: " << resourceName;
        Ogre::LogManager::getSingleton().logMessage(msg.str());
    }
}

static void UnloadMaterials(const std::string& filename)
{
    if (filename.empty())
    {
        Ogre::LogManager::getSingleton().logMessage("Filename is empty.");
        return;
    }

    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
    if(!stream.isNull())
    {
        try
        {
            while(!stream->eof())
            {
                std::string line = stream->getLine();
                StringUtil::trim(line);

                ///
                /// UNLOAD MATERIALS
                ///
                if (StringUtil::startsWith(line, "material"))
                {
                    StringVector vec = StringUtil::split(line," \t:");
                    bool skipFirst = true;
                    for (StringVector::iterator it = vec.begin(); it < vec.end(); ++it)
                    {
                        if (skipFirst)
                        {
                            skipFirst = false;
                            continue;
                        }
                        std::string match = (*it);
                        StringUtil::trim(match);
                        if (!match.empty())
                        {
                            UnloadResource(Ogre::MaterialManager::getSingletonPtr(), match);
                            break;
                        }
                    }
                }
            }
        }
        catch (Ogre::Exception &e)
        {
            StringUtil::StrStreamType msg;
            msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
            Ogre::LogManager::getSingleton().logMessage(msg.str());
        }
    }
    stream->close();
}

static void UnloadVertexPrograms(const std::string& filename)
{
    if (filename.empty())
    {
        Ogre::LogManager::getSingleton().logMessage("Material filename is empty.");
        return;
    }

    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
    if(!stream.isNull())
    {
        try
        {
            while(!stream->eof())
            {
                std::string line = stream->getLine();
                StringUtil::trim(line);

                ///
                /// UNLOAD VERTEX PROGRAMS
                ///
                if (StringUtil::startsWith(line, "vertex_program") && !StringUtil::startsWith(line, "vertex_program_ref"))
                {
                    StringVector vec = StringUtil::split(line," \t");
                    bool skipFirst = true;
                    for (StringVector::iterator it = vec.begin(); it < vec.end(); ++it)
                    {
                        if (skipFirst)
                        {
                            skipFirst = false;
                            continue;
                        }
                        std::string match = (*it);
                        StringUtil::trim(match);
                        if (!match.empty())
                        {
                            UnloadResource(Ogre::HighLevelGpuProgramManager::getSingletonPtr(), match);
                            break;
                        }
                    }
                }
            }
        }
        catch (Ogre::Exception &e)
        {
            StringUtil::StrStreamType msg;
            msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
            Ogre::LogManager::getSingleton().logMessage(msg.str());
        }
    }
    stream->close();
}

static void UnloadFragmentPrograms(const std::string& filename)
{
    if (filename.empty())
    {
        Ogre::LogManager::getSingleton().logMessage("Material filename is empty.");
        return;
    }

    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
    if(!stream.isNull())
    {
        try
        {
            while(!stream->eof())
            {
                std::string line = stream->getLine();
                StringUtil::trim(line);

                ///
                /// UNLOAD FRAGMENT PROGRAMS
                ///
                if (StringUtil::startsWith(line, "fragment_program") && !StringUtil::startsWith(line, "fragment_program_ref"))
                {
                    StringVector vec = StringUtil::split(line," \t");
                    bool skipFirst = true;
                    for (StringVector::iterator it = vec.begin(); it < vec.end(); ++it)
                    {
                        if (skipFirst)
                        {
                            skipFirst = false;
                            continue;
                        }
                        std::string match = (*it);
                        StringUtil::trim(match);
                        if (!match.empty())
                        {
                            UnloadResource(Ogre::HighLevelGpuProgramManager::getSingletonPtr(), match);
                            break;
                        }
                    }
                }
            }
        }
        catch (Ogre::Exception &e)
        {
            StringUtil::StrStreamType msg;
            msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
            Ogre::LogManager::getSingleton().logMessage(msg.str());
        }
    }
    stream->close();
}

static void ReloadMaterial(const std::string& materialName, const std::string& groupName, const std::string& filename, bool parseMaterialScript)
{
    if (materialName.empty())
    {
        Ogre::LogManager::getSingleton().logMessage("Material name is empty.");
        return;
    }

    if (groupName.empty())
    {
        Ogre::LogManager::getSingleton().logMessage("Group name is empty.");
        return;
    }

    if (filename.empty())
    {
        Ogre::LogManager::getSingleton().logMessage("Filename is empty.");
        return;
    }

    UnloadMaterials(filename);
    UnloadVertexPrograms(filename);
    UnloadFragmentPrograms(filename);

    if (parseMaterialScript)
    {
        Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
        if(!stream.isNull())
        {
            try
            {
                Ogre::MaterialManager::getSingleton().parseScript(stream, groupName);
                Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName(materialName);
                if (!materialPtr.isNull())
                {
                    materialPtr->compile();
                    materialPtr->load();
                }
            }
            catch (Ogre::Exception &e)
            {
                StringUtil::StrStreamType msg;
                msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
                Ogre::LogManager::getSingleton().logMessage(msg.str());
            }
        }
        stream->close();

        ///
        /// RELOAD MATERIAL SCRIPT CONTENTS
        ///
        stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
        if(!stream.isNull())
        {
            try
            {
                ///
                /// RELOAD ALL MATERIAL CONTENTS IN FILE
                ///
                while(!stream->eof())
                {
                    std::string line = stream->getLine();
                    StringUtil::trim(line);
                    ///
                    /// RELOAD MATERIALS
                    ///
                    if (StringUtil::startsWith(line, "material"))
                    {
                        StringVector vec = StringUtil::split(line," \t:");
                        bool skipFirst = true;
                        for (StringVector::iterator it = vec.begin(); it < vec.end(); ++it)
                        {
                            if (skipFirst)
                            {
                                skipFirst = false;
                                continue;
                            }
                            std::string match = (*it);
                            StringUtil::trim(match);
                            if (!match.empty())
                            {
                                LoadResource(Ogre::MaterialManager::getSingletonPtr(), match, groupName);
                                break;
                            }
                        }
                    }
                    ///
                    /// RELOAD VERTEX PROGRAMS
                    ///
                    if (StringUtil::startsWith(line, "vertex_program") && !StringUtil::startsWith(line, "vertex_program_ref"))
                    {
                        StringVector vec = StringUtil::split(line," \t");
                        bool skipFirst = true;
                        for (StringVector::iterator it = vec.begin(); it < vec.end(); ++it)
                        {
                            if (skipFirst)
                            {
                                skipFirst = false;
                                continue;
                            }
                            std::string match = (*it);
                            StringUtil::trim(match);
                            if (!match.empty())
                            {
                                LoadResource(Ogre::HighLevelGpuProgramManager::getSingletonPtr(), match, groupName);
                                break;
                            }
                        }
                    }
                    ///
                    /// RELOAD FRAGMENT PROGRAMS
                    ///
                    if (StringUtil::startsWith(line, "fragment_program") && !StringUtil::startsWith(line, "fragment_program_ref"))
                    {
                        StringVector vec = StringUtil::split(line," \t");
                        bool skipFirst = true;
                        for (StringVector::iterator it = vec.begin(); it < vec.end(); ++it)
                        {
                            if (skipFirst)
                            {
                                skipFirst = false;
                                continue;
                            }
                            std::string match = (*it);
                            StringUtil::trim(match);
                            if (!match.empty())
                            {
                                LoadResource(Ogre::HighLevelGpuProgramManager::getSingletonPtr(), match, groupName);
                                break;
                            }
                        }
                    }
                }
            }
            catch (Ogre::Exception &e)
            {
                StringUtil::StrStreamType msg;
                msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
                Ogre::LogManager::getSingleton().logMessage(msg.str());
            }
        }
        stream->close();

        try
        {
            // Do a render test if it fails, leave materials unloaded
            //Ogre::Root::getSingleton().renderOneFrame();
            return;
        }
        catch (Ogre::Exception &e)
        {
            UnloadVertexPrograms(filename);

            StringUtil::StrStreamType msg;
            msg << "Render test failed. Unloading vertex programs." << std::endl;
            msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
            Ogre::LogManager::getSingleton().logMessage(msg.str());
        }

        try
        {
            // Do a render test if it fails, leave materials unloaded
            //Ogre::Root::getSingleton().renderOneFrame();
        }
        catch (Ogre::Exception &e)
        {
            // Don't load the script this time
            ReloadMaterial(materialName, groupName, filename, false);

            StringUtil::StrStreamType msg;
            msg << "Render test failed. Unloading materials." << std::endl;
            msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
            Ogre::LogManager::getSingleton().logMessage(msg.str());
        }
    }
}

void reloadMaterials()
{
    ReloadMaterial("Objects/Player", "General", "Base.program", true);
    ReloadMaterial("Objects/Player", "General", "BaseShadowCaster.program", true);
    GlbVar.ogreSmgr->setShadowTextureCasterMaterial("BaseShadowCaster");

    ReloadMaterial("Objects/Player", "General", "Objects.material", true);
    ReloadMaterial("Objects/Player", "General", "Brushes.material", true);
    ReloadMaterial("Objects/Player", "General", "Props.material", true);

    Util::reloadSun();
}

}
