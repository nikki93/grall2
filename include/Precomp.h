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
#include "Ngf.h"
#include "ngfplugins/NgfBullet.h"
#include "ngfplugins/NgfPython.h"
#include "ngfplugins/NgfSerialisation.h"
#include "ngfplugins/NgfStates.h"
#include "ngfplugins/NgfExtras.h"
#include "boost/format.hpp"
#include "sys/stat.h"
