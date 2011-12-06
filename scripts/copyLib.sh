#!/bin/sh

rm -rf lib
mkdir lib
cp -v `find ../ngf/ -name 'FastDel*.h'` `find ../ngf/ -name 'Ngf*.h'` `find ../ngf/ -name 'Ngf*.cpp'` `find ../btogre/ -name 'BtOgre*.h'` `find ../btogre/ -name 'BtOgre*.cpp'` ./lib
cp -rv ../../Libraries/mygui/Common/{BaseLayout,MessageBox/*.h} ./lib
rm -rvf ./lib/BaseLayout/.svn
cd lib
mkdir ngfplugins
mv Ngf*.h ngfplugins
mv ngfplugins/Ngf.h .

