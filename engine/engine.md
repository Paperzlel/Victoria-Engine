# Engine
This folder contains all of the behind-the-scenes info that the engine uses in order to run properly

## Folder Structure
Most files have general purposes, as laid out below. Some files that are used universally, such as `defines.h` are held in the main src directory.
### Core
This is where funadmental files are held within the engine - mainly things that are used in a wide number of places but **not** everywhere. 
### Maths
This is where any files relating to maths operations or types need to go - matrices, vectors, RNG, the like
### Renderer
This folder comprises of several sub-folders as detailed below. The main files here are the link between the application layer and the renderer itself.
#### OpenGL
This folder contains all of the OpenGL-specific functions that we call when using this specific renderer.
### Systems
This folder contains the many systems that are not essential to the running of the application (like in core) but are not part of the renderer itself, and so must be contained here instead.
### Third-Party
This folder contains all of the files that are not mine, but are part of the application and are therefore needed to allow it to function. They also must need to be compiled alongside the application (in that they don't have their own DLL exported alongside the application) to be included here.
