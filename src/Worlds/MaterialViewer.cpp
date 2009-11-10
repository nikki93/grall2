/*
=========================================
MaterialViewer.cpp
=========================================
*/

#include "Worlds/MaterialViewer.h"

#include "Objects/Misc/Light.h"

//-------------------------------------------------------------------------------
void MaterialViewer::init()
{
    //Create the mesh.
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode("materialcube-node");
    mEntity = GlbVar.ogreSmgr->createEntity("materialcube-entity", "Mesh_Cube.mesh");
    mNode->attachObject(mEntity);

    //Camera.
    mCameraYawNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode("materialviewercamera-yawnode");
    mCameraPitchNode = mCameraYawNode->createChildSceneNode("materialviewercamera-pitchnode");
    mCameraDist = 3;

    //Some test settings.
    mMaterialType = "BaseNormalMapParallax";
    mMaterialSettings["diffuse_tex"] = "WhiteTile1.png";
    mMaterialSettings["normal_tex"] = "WhiteTile_NM.png";
    mMaterialSettings["glow_tex"] = "WhiteLight_GW.png";

    mMaterialSettings["ambient"] = "0.7 0.7 0.7 1";
    mMaterialSettings["diffuse"] = "0.9 0.9 0.9 1";
    mMaterialSettings["specular"] = "0.8 0.8 0.8 1";
    mMaterialSettings["scaleBias"] = "0.04 0.02";
    mMaterialSettings["glow_amount"] = "0.5";

    //First compile.
    compileMaterial();

    //Load the layout.
    MyGUI::LayoutManager::getInstance().load("MaterialViewer.layout");
    mWindow = GlbVar.gui->findWidget<MyGUI::Window>("win_materialViewer");

    //Callbacks.
    MyGUI::ButtonPtr button = GlbVar.gui->findWidget<MyGUI::Button>("but_applyMaterial");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MaterialViewer::onClickApply);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_saveMaterial");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MaterialViewer::onClickSave);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_createLight");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MaterialViewer::onClickCreateLight);

    //Update the editboxes.
    MyGUI::EnumeratorWidgetPtr iter = mWindow->getEnumerator();
    while (iter.next())
    {
        MyGUI::Widget *widget = iter.current();
        if (widget->getUserString("type") == "MaterialProperty") 
            dynamic_cast<MyGUI::Edit*>(widget)->setOnlyText(mMaterialSettings[widget->getUserString("property")]);
    }
    GlbVar.gui->findWidget<MyGUI::Edit>("edt_lightColour")->setOnlyText("0.7 0.7 0.7");

    //Populate the material type list.
    MyGUI::ComboBoxPtr list = GlbVar.gui->findWidget<MyGUI::ComboBox>("cmb_materialType");
    list->insertItemAt(0, "Base");
    list->insertItemAt(1, "BaseNormalMap");
    list->insertItemAt(2, "BaseNormalMapParallax");
    list->insertItemAt(3, "BaseGlow");
    list->insertItemAt(4, "BaseNormalMapGlow");
    list->insertItemAt(5, "BaseNormalMapParallaxGlow");
    list->insertItemAt(6, "BaseTransparent");
    list->setIndexSelected(2);
}
//-------------------------------------------------------------------------------
void MaterialViewer::tick(const Ogre::FrameEvent &evt)
{
    OIS::MouseState ms = Util::getMouseState();
    if (ms.buttonDown(OIS::MB_Right))
    {
        Ogre::Degree hor(ms.X.rel * GlbVar.settings.controls.turningSensitivity * -0.4);
        Ogre::Degree ver(ms.Y.rel * GlbVar.settings.controls.turningSensitivity * 0.4);

        mCameraYawNode->yaw(hor);
		mCameraPitchNode->pitch(ver);
    }
    else if (ms.buttonDown(OIS::MB_Middle))
    {
        mCameraDist += ms.Y.rel * GlbVar.settings.controls.turningSensitivity * 0.02;
        mCameraDist = Util::clamp<Ogre::Real>(mCameraDist, 0.5, 10);
    }

    GlbVar.camNode->setPosition(mCameraPitchNode->_getDerivedOrientation() * Ogre::Vector3(0,0,-mCameraDist));
    GlbVar.camNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TS_WORLD);

    if (Util::isKeyDown(OIS::KC_ESCAPE))
        Util::gotoWorld(0);
}
//-------------------------------------------------------------------------------
void MaterialViewer::stop()
{
    //Destroy stuff.
    GlbVar.goMgr->destroyAll();

    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
    GlbVar.ogreSmgr->destroySceneNode(mNode);

    mCameraYawNode->detachAllObjects();
    GlbVar.ogreSmgr->destroySceneNode(mCameraYawNode);
    GlbVar.ogreSmgr->destroySceneNode(mCameraPitchNode);

    GlbVar.gui->destroyWidget(mWindow);
}
//-------------------------------------------------------------------------------
Ogre::String MaterialViewer::createMaterialSource(const Ogre::String &name)
{
    Ogre::String source = 
        "import * from Base.program\n"
        "\n"
        "material " + name + " : " + mMaterialType + "\n"
        "{\n";

    for (MaterialSettings::iterator iter = mMaterialSettings.begin(); iter != mMaterialSettings.end(); ++iter)
        source += "    set $" + iter->first + " \"" + iter->second + "\"\n";

    source += "}\n";

    return source;
}
//-------------------------------------------------------------------------------
void MaterialViewer::compileMaterial()
{
    try 
    {
        Ogre::String name = "MaterialViewer/Material" + Ogre::StringConverter::toString(mMaterialCount++);
        Ogre::String source = createMaterialSource(name);
        mCompiler->compile(source, name, "General");

        mEntity->setMaterialName(name);
    }
    catch (Ogre::Exception &e)
    {
        MyGUI::Message::createMessageBox("Message", "An error occured!", 
                "The following error was detected, make sure your material properties and texture filenames are correct!\n\n" + Ogre::String(e.what()),
                MyGUI::MessageBoxStyle::Ok | MyGUI::MessageBoxStyle::IconError);
    }
}
//-------------------------------------------------------------------------------
void MaterialViewer::onClickApply(MyGUI::WidgetPtr)
{
    //Update properties.
    MyGUI::EnumeratorWidgetPtr iter = mWindow->getEnumerator();
    while (iter.next())
    {
        MyGUI::Widget *widget = iter.current();
        if (widget->getUserString("type") == "MaterialProperty") 
            mMaterialSettings[widget->getUserString("property")] = dynamic_cast<MyGUI::Edit*>(widget)->getOnlyText();
    }
    MyGUI::ComboBoxPtr list = GlbVar.gui->findWidget<MyGUI::ComboBox>("cmb_materialType");
    mMaterialType = list->getItemNameAt(list->getIndexSelected());
    
    //Compile.
    compileMaterial();
}
//-------------------------------------------------------------------------------
void MaterialViewer::onClickSave(MyGUI::WidgetPtr)
{
    Ogre::String name = mMaterialSettings["diffuse_tex"];
    Ogre::String source = createMaterialSource("Brushes/" + name);

    //Remove extension!
    Ogre::String filename = name.substr(0, name.find_last_of('.')) + ".material";
    std::ofstream out((USER_PREFIX "Content/" + filename).c_str());
    out << source;
    out.close();

    MyGUI::Message::createMessageBox("Message", "File '" + filename + "' saved!", 
            "Your material file has been saved as 'Content/" + filename + "' in the user directory!");
}
//-------------------------------------------------------------------------------
void MaterialViewer::onClickCreateLight(MyGUI::WidgetPtr)
{
    GlbVar.goMgr->createObject("Light", GlbVar.camNode->getPosition(), Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("lightType", "point")
            ("colour", GlbVar.gui->findWidget<MyGUI::Edit>("edt_lightColour")->getOnlyText())
            ("attenuation", "10 0.3 0.1 0.05")
            );
}
//-------------------------------------------------------------------------------
