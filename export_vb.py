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
    out.write("\t%f, %f, %f,\n" %
              (v.x, v.y, v.z))

def has_quads(mesh):
    return any(len(f) == 4 for f in mesh.faces)

def active():
    return bpy.data.scenes.active

def clean_quads(mesh):
    print "Cleaning up quads"
    oldmode = Mesh.Mode()
    Mesh.Mode(Mesh.SelectModes['FACE'])

    mesh.sel = True
    tempob = active().objects.new(mesh)
    mesh.quadToTriangle(0) # more=0 shortest length
    oldmode = Mesh.Mode(oldmode)
    active().objects.unlink(tempob)

    Mesh.Mode(oldmode)

def write_obj(filepath):
    out = file(filepath, 'w')
    mesh = Mesh.New()
    mesh.getFromObject(active().objects.active.name)

    editmode = Window.EditMode()
    if editmode:
        Window.EditMode(0)

    if has_quads(mesh):
        clean_quads(mesh)

    out.write("static const float vertices[] = {\n")
    for vert in mesh.verts:
        write_fv(out, vert.co)
    out.write("};\n\n")

    out.write("static const int indeces[] = {\n")
    for face in mesh.faces:
        out.write("\t%d, %d, %d,\n" %
                  (face.v[0].index, face.v[1].index, face.v[2].index))
    out.write("};\n\n")

    out.write("static const float normals[] = {\n")
    for vert in mesh.verts:
        write_fv(out, vert.no)
    out.write("};\n\n")

    sections = [
        ("vertices", len(mesh.verts)),
        ("indeces", len(mesh.faces)),
        ("normals", len(mesh.verts))
        ]

    out.write("static const VertexBuffer vb = {\n")
    for s in sections:
        out.write("\t{ %s, %d },\n" % s)
    out.write("\t{ NULL, 0 }\n")
    out.write("};")

    if editmode:
        Window.EditMode(1)

    out.close()


Blender.Window.FileSelector(write_obj, "Export a vertex buffer", "*.h")
