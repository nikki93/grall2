#/bin/sh

#Mesh XML, OgreXMLConverter.log.
find data/*Meshes -name '*.mesh.xml'
find data/*Meshes -name '*.log'

echo "OK?"
read xyz

rm -v `find data/*Meshes -name '*.mesh.xml'` `find data/*Meshes -name '*.log'`

echo ""

#.blend1 files.
find etc/ -name '*.blend1'

echo "OK?"
read xyz

rm -v `find etc/ -name '*.blend1'`

echo ""

#Blender dummy materials.
#find data -name '*-Blender.material'

#echo "OK?"
#read xyz

#rm -v `find data -name '*-Blender.material'`
