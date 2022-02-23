# uvwasm

DynaDraw and Ring running in your browser using Emscripten, wasm and webgl.

### DynaDraw

DynaDraw is a dynamic drawing program

[Try DynaDraw](https://paulhaeberli.github.io/uvwasm/dynadraw/index.html)

Click the button in the lower left corner to clear. The c++ source for this
app is in uvDynaDraw.cpp, 204 lines.

### Ring

Ring renders 3D graphics, with a UI to change the rendering.

[Try Ring](https://paulhaeberli.github.io/uvwasm/ring/index.html)

### MultiView

A mini window manager. Prepare for glitch! The c++ source for the window
manager is in 276 lines of c++ see uvWindowManager.cpp

[Try MultiView](https://paulhaeberli.github.io/uvwasm/multiview/index.html)


### NOTE

On macOS: This runs fine on Firefox, Google Chrome. Currently having problems on macOS Safari.

On iOS: Fine on iPad and iPhone.

On Ubuntu: Fine on FireFox.

On Windows: Fine Firefox, Chrome, and Edge

The font rendering is not quite right...


### Thank you

Many thanks to Patricio González Vivo for creating glslViewer.

This is based on his code [https://github.com/patriciogonzalezvivo/glslViewer](https://github.com/patriciogonzalezvivo/glslViewer)

