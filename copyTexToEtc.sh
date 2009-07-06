#!/bin/sh
cp -r data/BrushTextures/* etc/BrushTextures/
cp -r data/ObjectTextures/* etc/ObjectTextures/

cd etc/BrushTextures/
rm `find -name '*_NM*'`

cd ../../etc/ObjectTextures/
rm `find -name '*_NM*'`
