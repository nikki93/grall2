import bpy
import functools
import subprocess

bl_info = {
    "name": "GraLL 2 NGF Exporter",
    "description": "Exports a GraLL 2 NGF level",
    "author": "Nikhilesh Sigatapu",
    "version": (1, 0),
    "blender": (2, 63, 0),
    "location": "File > Export",
    "warning": "", # used for warning icon and text in addons panel
    "category": "Import-Export"
}

GRALL2_COMMAND='/home/nikki/Development/Projects/grall2/bin/debug/GraLL2_d'
GRALL2_CWD='/home/nikki/Development/Projects/grall2/bin/debug/'

class GraLL2RunGame(bpy.types.Operator):
    bl_idname = "export.grall2_run_game"
    bl_label = "Run GraLL 2 game with currently edited level"

    @classmethod
    def poll(cls, context):
        return True

    def execute(self, context):
        subprocess.Popen([GRALL2_COMMAND, context.scene.name],
                cwd = GRALL2_CWD)
        return {'FINISHED'}

class GraLL2OgreMeshes(bpy.types.Operator):
    bl_idname = "export.grall2_ngf_ogre_meshes"
    bl_label = "Export GraLL 2 NGF Level and run Ogre Mesh Exporter"

    @classmethod
    def poll(cls, context):
        return True

    def execute(self, context):
        bpy.ops.export.grall2_ngf()
        #bpy.ops.ogre3d.export() # doesn't work at the moment

        return {'FINISHED'}

class GraLL2Exporter(bpy.types.Operator):
    bl_idname = "export.grall2_ngf"
    bl_label = "Export GraLL 2 NGF Level"

    filepath = bpy.props.StringProperty(subtype="FILE_PATH")

    def __init__(self):
        filepath = ''

    @classmethod
    def poll(cls, context):
        return True

    # called when the exporter is invoked
    def invoke(self, context, event):
        return self.execute(context)

    # called by the file selection dialog
    def execute(self, context):
        cfg = bpy.data.texts.get('NGFExport.cfg')
        if cfg:
            self.save_ngf(cfg.lines[0].body)
        elif self.filepath != '':
            self.save_ngf(self.filepath)
            self.filepath = ''
        else:
            context.window_manager.fileselect_add(self)
            return {'RUNNING_MODAL'}
        return {'FINISHED'}

    # actually write the file
    def save_ngf(self, filename):
        out = open(filename, "w")

        # each scene becomes one NGF level
        for scene in bpy.data.scenes:
            out.write("ngflevel %s\n{\n" % (scene.name))

            # brush counter
            brushIndex = 0

            for obj in scene.objects:
                # deselect now, reselect later if brush
                obj.select = False

                # get position, orientation, game properties
                obj.rotation_mode = 'QUATERNION'
                pos = obj.location
                rot = obj.rotation_quaternion
                props = obj.game.properties

                # write basic game properties and position, orientation
                out.write("\tobject\n\t{\n")
                out.write("\t\ttype %s\n" % props['type'].value)
                out.write("\t\tname %s\n" % props['name'].value)
                out.write("\t\tposition %f %f %f\n" % (pos[0], pos[2], -pos[1]))
                out.write("\t\trotation %f %f %f %f\n" % (rot[0], rot[1], rot[3], -rot[2]))

                # write special properties
                haveProps = False # whether there are any properties - initially no
                propStr = "\n\t\tproperties\n\t\t{\n"

                # (GRALL 2 MODIFICATION: write dimension value corresponding to obj.layers boolean vector)
                propStr += "\t\t\tdimensions %d\n" % (functools.reduce(
                    lambda p, b: (p[0] + 1, (b << p[0]) | p[1]), obj.layers, (0, 0))[1])

                for prop in props:
                    # (GRALL 2 MODIFICATION: don't write 'dimensions', ... )
                    if prop.name not in ['type', 'name', 'dimensions', 'dimension1', 'dimension2']:
                        haveProps = True
                        name = prop.name
                        value = ''

                        # check if brush and save relevant info
                        if name == 'isBrush':
                            # update name to follow naming convention
                            brushName = "%s_b%d" % (scene.name, brushIndex)
                            obj.name = brushName       #object name
                            obj.data.name = brushName  #mesh name

                            # rename materials to follow naming convention
                            for matIndex, mat in enumerate([slot.material for slot in obj.material_slots]):
                                mat.name = '%s_m%d' % (brushName, matIndex)

                            # store mesh name in property
                            name = "brushMeshFile"
                            value = obj.data.name + ".mesh"
                            brushIndex += 1

                            # select for subsequent Ogre .mesh export
                            obj.select = True

                        # check for refText - reference to Blender text object (useful for scripts etc.)
                        elif prop.type == 'STRING' and prop.value.startswith('refText'):
                            print(obj.name)
                            lines = [line.body for line in bpy.data.texts[prop.value[8:]].lines]

                            # first line is simple
                            value = ": " + lines[0]

                            # indent other lines
                            lineFormat = "\n\t\t\t" + (" " * len(name)) + " : %s"
                            for line in lines[1:]:
                                value += lineFormat % line

                        # else simply convert to string
                        else:
                            value = str(prop.value)

                        # write name, value pair
                        propStr += "\t\t\t%s %s\n" % (name, value)

                if haveProps:
                    out.write(propStr + "\t\t}\n")

                out.write("\t}\n")

            # (GRALL 2 MODIFICATION: enable all layers)
            scene.layers = [True] * len(scene.layers)

            # end of scene
            out.write("}\n\n")

        # save filename
        cfg = bpy.data.texts.get('NGFExport.cfg')
        if cfg:
            bpy.data.texts.remove(cfg)
        cfg = bpy.data.texts.new('NGFExport.cfg')
        cfg.write(filename)

        # we're done!
        out.close()

# Blender registration
def menu_func(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(GraLL2Exporter.bl_idname, text='GraLL 2 NGF Exporter')
def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)
def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
