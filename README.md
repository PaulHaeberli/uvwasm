# uvwasm

OpenGL applications running in your browser using Emscripten, wasm and webgl.

### DynaDraw

DynaDraw is a dynamic drawing program

[Try DynaDraw](https://paulhaeberli.github.io/uvwasm/dynadraw/index.html)

Click the button in the lower left corner to clear. The c++ source for this
app is in uvDynaDraw.cpp, 202 lines. This is a reimplementation the original
[DynaDraw from 1989](http://www.graficaobscura.com/dyna/index.html).

### Ring

Ring renders 3D graphics, with a UI to change the rendering.

[Try Ring](https://paulhaeberli.github.io/uvwasm/ring/index.html)

### MultiView

A mini window manager. Prepare for glitch! The c++ source for the window
manager is in 276 lines of c++ see uvWindowManager.cpp

[Try MultiView](https://paulhaeberli.github.io/uvwasm/multiview/index.html)

### Font Catalog

Shows glyph generation and storage for font rendering.

[Try FontCat](https://paulhaeberli.github.io/uvwasm/fontcat/index.html)

### NOTE

On macOS: This runs fine on Firefox, Google Chrome. Currently having problems on macOS Safari on M1,
macOS Safari on intel works.

On iOS: Fine on iPad and iPhone.

On Ubuntu: Fine on FireFox.

On Windows: Fine Firefox, Chrome, and Edge

The font rendering is not quite right...


### Thank you

Many thanks to Patricio González Vivo for creating glslViewer.

This is based on his code [https://github.com/patriciogonzalezvivo/glslViewer](https://github.com/patriciogonzalezvivo/glslViewer)

