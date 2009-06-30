#!/bin/sh
cp -r data/BrushTextures/* etc/BrushTextures/
cd etc/BrushTextures/
rm `find -name '*_NM*'`
