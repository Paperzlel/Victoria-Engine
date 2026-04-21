# To-do list
A list of all things that are on the current plan for the engine, whether short- or long-term. These are listed in no particular order but items may change as the project changes scope.

## Buildsystem
- [x] Merge Windows and Linux into one buildsystem
- [x] Add GCC build options to the engine
- [ ] Add version hash into the engine during generation
- [ ] Embed fonts and shaders within executable using Python scripts

## Codestyle
- [ ] Update documentation

## Core
- [ ] Add more `ERR_COND_FATAL_*` conditions so that context for crashes can be given
- [ ] Implement `HashTable` collision detection in `HashTable::_find_hashed_index`
- [ ] Remove `Event` as a class in favour of `CommandQueues` and other datatypes
- [x] Remove unnecessary `main_loop.h` include from `os.h`
- [ ] Game loading (i.e. loading items from a binary to be used by the engine)
- [ ] .NET scripting
- [x] C++ Extension API of some kinds
- [ ] Add a `Variant::operator==()` for non-variants or some kind of conversion tool
- [x] Add a `child_entered_tree` and `child_exited_tree` notifications to fix child transforms being set to the origin.
- [ ] Add Colours as a datatype
- [x] Add a `NOTIFICATION_DRAW` to update canvas items whenever is needed
- [x] Optimise `HashTable` to reduce memory allocations (most memory allocations are from HashTables) and improve searching algorithms (maybe do some looking up on the best and implement that?)
- [ ] Add method binds to `ClassRegistry` to enable API generation
- [ ] Add a `Type` class to `Object` so that classes can be determined faster
- [ ] Implement project configs and their loaders
- [ ] Implement scene loaders
- [ ] Implement `Dictionaries`
- [x] Define more math functions like `floor()` and `log()`
- [x] Optimise `Vector<T>` and other classes to have a singular pointer to save class size
- [x] Use move semantics in `Vector<T>` where applicable
- [ ] Add a `CommandQueue` structure to be used for processing end-of-frame logic and other places a queue would be appropriate
- [ ] Add multithreading support in the form of `Mutex`es, `Semaphore`s, `Thread`s and a `WorkerThreadPool`

## Drivers
- [x] Add `resize_viewport` command to EGL

## Editor (on hold)
- [x] Move 3D data to a sub-viewport (as the editor is 2D)
- [ ] Create a logger sub-class
- [ ] Add the `EditorLogger` to the bottom panel
- [ ] Refactor editor so that sub-nodes manage their children rather than all at once
- [ ] Add 3D gizmos to the scene editor (world grid, X/Y/Z axes)
- [ ] (REQUIRES SCENE LOADERS) Implement scene editing

## GUI
- [x] Implement size overrides for objects
- [ ] Make the default size for text 1 when being output, add a font scale to text
- [x] Add SDF fonts
- [x] Implement some kind of `FontAtlas` to save the number of font glyphs and resulting textures created at runtime.
- [x] Add a toggle for SDF fonts or not.
- [ ] Add a toggle for `FontAtlas`es or not.
- [x] Make text scale downwards from top-left (currently, it is acting from not-quite-bottom-left and needs a per-pixel transformation)
- [x] Add anchor points so that a `CanvasItem` is fixed to its specific part of the screen
- [x] Add a `Container` object that clips its children to within its bounds
- [ ] Add a `VBoxSeparator` and `HBoxSeparator` object that separates its children horizontally and vertically along their respective axes
- [x] Reparent `ViewportContainer` to being derived from `Container`.
- [ ] Add position, rotation and scale options to `ObjectUI`s.
- [ ] Add a `Theme` class to define things such as colours and fonts used
- [ ] Add options for theme overrides on specific GUI objects (requires themes)

## Importing
- [ ] Remove font atlas options from the font importers

## Platform
- [x] Fix X11 displays not resizing when using EGL
- [x] Add `set_window_resize_callback` to Windows
- [x] Add `is_suspended` and `set_is_suspended` to Windows
- [ ] Add OS-specific library loading
- [x] Add `get_environment` and `set_environment` to Windows
- [ ] Update Windows to use the new window settings and `r_error` in construction
- [x] Fully implement Wayland as a display manager option
- [x] Rework DisplayManagers so that they fall back to the other if they are unable to be created
- [x] Implement a way to get the preferred DisplayManager option from the OS
- [ ] Add some kind of message queue to Wayland so that messages can be processed properly

## Rendering
- [x] Fix bug with running any EGL code
- [x] Add EGL and ANGLE as OpenGL alternatives (currently there is an issue with Intel drivers and native OpenGL)
- [x] Add GPU vendor detection to EGL
- [x] Update `ITEM_FLAG_USE_GRAYSCALE` to `ITEM_FLAG_USE_ALPHA_ONLY`
- [x] Use viewport size for the 2D UI pass to get elements in actual screen coordinates
- [x] Pass a `RenderData` object to the `draw` function (maybe use separate functions for `draw_scene` and `draw_canvas`?)
- [x] Implement a y-sorting algorithm to render certain objects over another
- [x] Fix window aspect ratio not updating
- [x] Implement `GL_KHR_debug` for better OpenGL debugging
- [x] Improve `CanvasShader` so that both vertex position and rect offset can be used at once (requires re-config of how rect data is managed i.e. use rect or transform but not both)
- [ ] Rework the renderer so that Items and their base types act like a queue rather than as unique items to reduce RID allocations

## Scene
- [x] Add a `TextureRect` and `ColourRect` as children of `CanvasItem`
- [x] Add objects `Viewport` and `Window`
- [ ] Add resizing options to text

## Tests
- [ ] Add tests for `RBMaps`, `RIDs` and `RIDOwners`.
- [ ] Add tests for all `VectorN` and `VectorNi` classes.
- [ ] Add tests for `Transform2D` and `Transform3D`.
- [x] Improve tests for `HashTable`.
- [ ] Improve tests for `List`.
- [ ] Improve tests for `Vector`.
- [x] Improve tests for `Array`.
- [x] Add tests for `Variant`.
- [ ] Add tests for `String`.
- [ ] Add tests for `Input`.
- [ ] Add tests for `Memory`.
- [ ] Add tests for `ObjectUI` and derived classes.
- [ ] Add tests for `Object2D` and derived classes.
- [ ] Add tests for `Object3D` and derived classes.
- [ ] Add a test for running the main loop.
- [ ] Add dummy servers and managers for testing the above which require them.
- [ ] Add a `SceneTree` mockup test.
