/*
 * =====================================================================================
 *
 *       Filename:  PythonBinds.cpp
 *
 *    Description:  The GraLL2-Python library.
 *
 *        Created:  06/11/2009 09:54:33 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"

//--- Wrapper functions ---------------------------------------------------------

//Just a test function.
void testFunc()
{
    GlbVar.console->print("TEST\n");
}

//--- The module ----------------------------------------------------------------

BOOST_PYTHON_MODULE(GraLL2)
{
    py::def("testFunc", testFunc);
}

//--- The function that gets called from Game::init() ---------------------------

void initPythonBinds()
{
    initGraLL2();
}
