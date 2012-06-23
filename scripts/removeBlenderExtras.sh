#/bin/sh

#Mesh XML, OgreXMLConverter.log.
find data/*Meshes -name '*.mesh.xml'
find data/*Meshes -name '*.log'

echo "OK?"
read xyz

rm -vf `find data/*Meshes -name '*.mesh.xml'` `find data/*Meshes -name '*.log'`

echo ""

#.blend1 files.
find -regex '.*\.blend.+'

echo "OK?"
read xyz

rm -vf `find -regex '.*\.blend.+'`

echo ""

#Blender dummy materials.
#find data -name '*-Blender.material'

#echo "OK?"
#read xyz

#rm -v `find data -name '*-Blender.material'`
