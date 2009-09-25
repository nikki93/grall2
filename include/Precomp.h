/*
 * =====================================================================================
 *
 *       Filename:  Precomp.h
 *
 *    Description:  This header is precompiled where it is supported by premake. It
 *                  '#includes' all library headers, which rarely change while the
 *                  project is in development.
 *
 *        Created:  04/28/2009 04:55:58 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Ogre.h"
#include "OIS/OIS.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include "MyGUI.h"
#include "OgreAL.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include "Python.h"
#else
#include "python2.6/Python.h"
#endif
#include "Ngf.h"
#include "ngfplugins/NgfBullet.h"
#include "ngfplugins/NgfPython.h"
#include "ngfplugins/NgfSerialisation.h"
#include "ngfplugins/NgfExtras.h"
