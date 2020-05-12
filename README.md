NeuroEditor
=====================================================

## Introduction
Neuroeditor is a software tool for the visualization and edition of morphological tracings that offers manual edition capabilities together with a set of algorithms that can automatically identify potential errors in the tracings and, in some cases, propose a set of actions in order to automatically correct them. Neuroeditor visualizes the original tracing, the modified tracing and a 3D mesh that  approximates the neuronal membrane. This approximation of the cell's surface is computed on-the-fly and instantaneously reflects any changes made to the tracing. Besides the already implemented methods, Neuroeditor can be easily extended by the users, who can program their own algorithms in Python and run them within the tool.


## Dependencies

* Required dependencies:
    * Qt5.9
    * OpenGL
    * glut
    * QGLViewer
    * nsol
    * neurolots
    * eigen3
    * pygems

Note: nsol, neurolots and pygems libraries are automatically downloaded and built if not found by cmake.

## Building

Description

```bash
git clone https://github.com/gmrvvis/NeuroEditor
mkdir NeuroEditor/build && cd NeuroEditor/build
cmake .. -DCLONE_SUBPROJECTS=ON
make
```
