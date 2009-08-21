#!/bin/sh

help=1
while getopts ":fbh" opt; do
 case "$opt" in
  f) toHere=1 help=0 ;;
  b) toThere=1 help=0 ;;
  h) help=1 ;;
 esac
done

if [[ $help == 1 ]];then
	echo "Usage:"
	echo "-h 	Help (this text)"
	echo "-f 	Copy layouts from editor directory to data directory"
	echo "-b	Copy layouts from data directory to editor directory"
	exit
fi

if [[ $toHere == 1 ]];then
 cp ~/Development/Libraries/ogre/mygui-2.2/Shared/linux/*.layout data/GUI/Layouts/
fi

if [[ $toThere == 1 ]];then
 cp data/GUI/Layouts/*.layout ~/Development/Libraries/ogre/mygui-2.2/Shared/linux/
fi
