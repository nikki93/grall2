#!/bin/sh

LOCALDIR=~/Development/Projects/grall2/etc/local/sounds
DOWNLOADDIR=~/Downloads/Firefox/
DATADIR=~/Development/Projects/grall2

help=1
while getopts ":gftlpdn" opt; do
 case "$opt" in
  p)
    mplayer $LOCALDIR/$2-$3.*
    exit
    ;;
  t)
    mplayer $DATADIR/data/Sounds/FX/$2.*
    exit
    ;;
  l)
    ls $LOCALDIR
    exit
    ;;
  g)
    rm -v $DATADIR/data/Sounds/FX/$2.*
    NAME=`ls $LOCALDIR/$2-$3.*`
    EXT=${NAME:${#NAME}-3}
    OUT=$DATADIR/data/Sounds/FX/$2.$EXT
    cp -v $NAME $OUT

    if [[ "$EXT" != "wav" && "$EXT" != "ogg" ]]; then
        ffmpeg -i $OUT $DATADIR/data/Sounds/FX/$2.wav
        rm $OUT
        OUT=$DATADIR/data/Sounds/FX/$2.wav
    fi

    echo 
    echo "$OUT created!"
    exit
    ;;
  f)
    cp -v $DOWNLOADDIR/$2 $LOCALDIR/$3-$4.${2:${#2}-3}
    exit
    ;;
  d)
    NAME=`date +%s`
    cp -v $DOWNLOADDIR/$2 $LOCALDIR/$3-$NAME.${2:${#2}-3}
    exit
    ;;
  n)
    ARR=("$@")
    N=0

    for name in ${ARR[@]}
    do
        if [[ $N > 1 ]];then
            NAME=`ls $LOCALDIR/$2-$name.*`
            EXT=${NAME:${#NAME}-3}

            #Allows 'first' things to keep their name to keep old uses compatible.
            if [[ $N != 2 ]];then
                OUT=$DATADIR/data/Sounds/FX/$2.$EXT
            else
                OUT=$DATADIR/data/Sounds/FX/$2-$name.$EXT
            fi

            cp -v $NAME $OUT

            if [[ "$EXT" != "wav" && "$EXT" != "ogg" ]]; then
                ffmpeg -i $OUT ${OUT:0:${#OUT}-3}wav
                rm $OUT
                OUT=${OUT:0:${#OUT}-3}wav
            fi

        fi
        N=`expr $N + 1`
    done
    exit
    ;;
 esac
done

if [[ $help == 1 ]];then
        echo "Usage: "
        echo "       scripts/sound.sh -d <downloadfilename> <gamename>"
        echo "       scripts/sound.sh -f <downloadfilename> <gamename> <localname>"
        echo "       scripts/sound.sh -p <gamename> <localname>"
        echo "       scripts/sound.sh -t <gamename>"
        echo "       scripts/sound.sh -g <gamename> <localname>"
        echo "       scripts/sound.sh -n <basegamename> <localname> <localname> ..."
        echo "       scripts/sound.sh -l"
        echo
        echo " 'gamename' identifies the actual sound in the game. 'localname' can be used to"
        echo " differentiate local (candidate) sounds."
        echo
        echo "Eg: "
        echo "       scripts/sound.sh -f shoot01.wav TurretShoot bigshoot"
        echo "       scripts/sound.sh -p TurretShoot bigshoot"
        echo
        echo "-d        Copy sound from downloads directory to local directory, generating unique localname using date/time."
        echo "-f        Copy sound from downloads directory to local directory"
        echo "-p        Preview sound from local directory."
        echo "-t        Test sound from game directory."
        echo "-g        Copy sound from local directory to game directory."
        echo "-n        Copy a number of sounds."
        echo "-l        List local sounds."
        exit
fi

