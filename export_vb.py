#!BPY

"""
Name: 'Stupid vertex buffer (.h)'
Blender: 244
Group: 'Export'
Tooltip: 'Exports header file for my stupid render'
"""
import Blender
from Blender import *
import bpy
import bpy
import os

def write_fv(out, v):
    out.write("\t{ %f, %f, %f },\n" %
              (v.x, v.y, v.z))

def write_obj(filepath):
    out = file(filepath, 'w')
    sce = bpy.data.scenes.active
    ob = sce.objects.active
    mesh = Mesh.New()
    mesh.getFromObject(ob.name)

    editmode = Window.EditMode()
    if editmode: Window.EditMode(0)
    has_quads = False
    for f in mesh.faces:
        if len(f) == 4:
            has_quads = True
            break

    if has_quads:
        oldmode = Mesh.Mode()
        Mesh.Mode(Mesh.SelectModes['FACE'])

        mesh.sel = True
        tempob = sce.objects.new(mesh)
        mesh.quadToTriangle(0) # more=0 shortest length
        oldmode = Mesh.Mode(oldmode)
        sce.objects.unlink(tempob)

        Mesh.Mode(oldmode)

    out.write("static const float vertices[][3] = {\n")
    for vert in mesh.verts:
        write_fv(out, vert.co)
    out.write("};\n\n")

    out.write("static const int indeces[][3] = {\n")
    for face in mesh.faces:
        out.write("\t{ %d, %d, %d },\n" %
                  (face.v[0].index, face.v[1].index, face.v[2].index))
    out.write("};\n\n")

    out.write("static const float normals[][3] = {\n")
    for vert in mesh.verts:
        write_fv(out, vert.no)
    out.write("};\n")

    if editmode:
        Window.EditMode(1)

    out.close()


filename = os.path.splitext(Blender.Get('filename'))[0]
Blender.Window.FileSelector(write_obj, "Export", '%s.h' % filename)
