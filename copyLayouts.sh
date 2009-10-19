#!/bin/sh

EDITORDIR=~/Development/Libraries/ogre/mygui-head/build/bin

help=1
while getopts ":ge" opt; do
 case "$opt" in
  g) toHere=1 help=0 ;;
  e) toThere=1 help=0 ;;
 esac
done

if [[ $help == 1 ]];then
	echo "Usage:"
	echo "-g 	Copy layouts from editor directory to game directory"
	echo "-e	Copy layouts from game directory to editor directory"
	exit
fi

if [[ $toHere == 1 ]];then
 cp $EDITORDIR/*.layout data/GUI/Layouts/
fi

if [[ $toThere == 1 ]];then
 cp data/GUI/Layouts/*.layout $EDITORDIR
fi
