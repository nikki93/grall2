#!/bin/sh
cp -r data/BrushTextures/* etc/BrushTextures/
cp -r data/ObjectTextures/* etc/ObjectTextures/

cd etc/BrushTextures/
rm `find -name '*_NM*'`
rm `find -name '*_GW*'`

cd ../../etc/ObjectTextures/
rm `find -name '*_NM*'`
rm `find -name '*_GW*'`
