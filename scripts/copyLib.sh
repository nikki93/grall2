#!/bin/sh

rm -rf lib
mkdir lib
cp `find ../ngf/ -name 'FastDel*.h'` `find ../ngf/ -name 'Ngf*.h'` `find ../ngf/ -name 'Ngf*.cpp'` `find ../btogre/ -name 'BtOgre*.h'` `find ../btogre/ -name 'BtOgre*.cpp'` ./lib
cd lib
mkdir ngfplugins
mv Ngf*.h ngfplugins
mv ngfplugins/Ngf.h .

