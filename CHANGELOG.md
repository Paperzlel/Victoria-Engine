# Changelog
Changes exist in chronological order (i.e. new changes are to be appended to the end of the file). Dates are done in DD/MM/YYYY format with the version number applied to each date if needed.

## 29/4/2026
- Core:
	- Added `CommandQueue` and `GlobalCommandQueue` objects as new API classes from core.
		- `CommandQueue`s are designed to have method pointers pushed to them, and then flushed after so many update cycles.
		- Tests should be added in the future to ensure consistent behaviour - at the moment it has no yet been tested properly.
	- Added `call_deferred` method to `CallableMethod`.
		- Pushes data to the `GlobalCommandQueue` when called, to call the function itself at a later date. The `CallableMethod` can go out of scope as the refcount is maintained by the one in the queue.

## 27/4/2026
- Core:
	- Remove the `Event` class and its related functionality.
		- This class was not really solving the problem it wanted to solve, as we will likely just use `Vector<CallableMethod>` if required or simply have one callback function. `CommandQueues` are needed anyways, which we'll add anyways.

## 26/4/2026
- Core:
	- Linux/Wayland:
		- Reduced the number of calls made to the resize event handler when the window is maximised, and changed the logic around window resizes to (mostly) fit in its own function.
			- Hopefully reduces some of the overhead when using Wayland, which I now do on both of my devices.
	- Input:
		- Move around the input classes and add a `SPECIAL` flag into the mix.
			- This has broken the input class and needs a new system to make it work again.

## 22/4/2026
- Core:
	- Linux/Wayland:
		- Implement keyboard input for the Wayland backend.
			- These are sent whenever the hardware calls to the `_key` function, and not during processing. To do so requires a message queue which we have no yet set up.
		- Added `libxkbcommon` as a dependency to Wayland.
			- Part of the above changes.
	- Input:
		- Added new keyboard keys and redefine the enum as a class.
			- The `enum class` just allows us to keep all the keys out of the global scope, which could conflict with the `linux/input-event-codes.h` as some names do collide.
			- Windows now needs to have its own keyboard remapping, since it's out of date. It will be further once the changes I have planned are done.

## 20/4/2026
- Core:
	- String:
		- Fix incorrect format sizing in `vformat`.
	- Add a check to `core_initialize` that properly returns an error if the display manager could be allocated but not properly initialized (possibly due to an invalid combination, i.e. trying to run Wayland on an X11 client).
	- Linux/Wayland:
		- Implement mouse scrolling for Wayland clients.
			- It's a little weird at the moment, since we don't have a clear dispatch function and Wayland doesn't send "stop" events for regular mouse scrolling. Future Wayland work will resolve this problem (hopefully).
- Runtime:
	- Importers:
		- Fixed a segmentation fault that would occur when trying to load file from an invalid path. Previously, there was no check for if `FileSystem::open` would return a valid resource, which it now does. 
	- Rendering:
		- Deprecate `ITEM_USE_RECT` and remove its functionality from the OpenGL backend and shaders.
			- Its functionality ended up limiting how transforms worked with UI code. Now transforms are applied properly in code.
		- OpenGL:
			- Changed the error messaging for the OpenGL utilities to properly reflect the error level.
	- Scene:
		- Properly set the offset of the transform when returning from `get_transform()` in `UIObject`.
			- Part of the above changes. Prior, the transform was only changed on a draw step, which meant reading back the transform revealed an identity matrix. Now, all reads from `get_transform()` will return the proper transform.
		- `ColourRect` and `TextureRect` now don't specify their position when creating their render spaces.
			- This issue was caused because of how transforms are handled in the canvas shader with the `ITEM_USE_RECT` option now gone. In this command, a position will now act as an offset and can be applied if extending the class.
		- Added a new entry `offsets` and a new private method `_update_offsets` to `UIObject`.
			- Assists with the above changes. Offsets are the position relative to an anchor, and thus are modified whenever the user moves an item on-screen.

## 19/4/2026
- Core:
	- Data:
		- Migrate `Vector<T>` data out to `CoWData<T>`.
			- In short, the logic behind how we want to control ownership of data in the pointer began to conflict with how `Vector`'s methods were designed, namely `push_back` and `insert`. In the lack of a better name for the class (`SharedVector`, `InternalVector` and `UniqueVector` were considered) we decided to take the Godot name instead since the original class is copied more or less exactly to our own implementation.
		- Remove the `push_front()`, `pop_front()` and `pop_back()` methods from `Vector<T>`.
			- Not really useful outside of unit tests and mirroring `Array` logic. Changed for the better.
		- Rework `Vector<T>` so that invalid read/writes no longer occur.
			- For some reason, calling `_realloc_buffer` fixed a bunch of issues with vector length. Considering the lack of actual change (the data was doing the same when calling `resize()`) it's beyond me but now it should work.
		- Remove `get_ptr_size()` from `Vector<T>`.
			- Not useful. Used in too few a place to warrant existing. Can be and has easily been replaced.
	- Linux/X11:
		- Fix a memory leak when calling `XGetWindowProperty` that would not free an X11-alloc'd buffer.
			- A minor leak but a good catch nonetheless - this method is called whenever the window state is changed and could have become an issue in the future.
	- Platform:
		- Made the `DisplayManager` destructor virtual, and actually added it in.
			- Prior, any data relevant to the `DisplayManager` and its inherited classes would never have been freed, and this memory would have stuck around until the kernel cleans it up. This isn't really an issue (the `DisplayManager` IS allocated only once, after all) but for the sake of being clean about it we free it here.
			- As a side note, this patch has removed all known memory leaks - all classes can be used without the worry of data leaking over time. GPU drivers are not the same and really suck with how much they leak - but not a lot we can do about that.

## 18/4/2026
- Core:
	- Linux/Wayland:
		- Upgraded the Wayland display manager to properly support `DisplayManager::toggle_mouse_mode()`. In doing so, also took the time to properly destroy any lingering connections, as well as restructured what data goes where, to hopefully convey more sense to the user.
		- New extensions `staging/pointer-warp`, `unstable/pointer-constraints` and `unstable/relative-pointer` are now used if available. The first has little support but does exist for the most common DEs (KDE, Hyprland and GNOME). The other two are almost entirely supported by all common DEs. 

## 17/4/2026
- Core:
	- Data: 
		- Upgraded `HashTable` to be fully documented, as well as having a new `reserve` function and far more class constructors and destructors.
			- The class is now more usable as it properly handles assigment and construction as opposed to the previous implementations.
- Tests:
	- Overhauled unit tests for `HashTable`.
	- Added a `README.md` for an explanation on how to write unit tests.
		- More unit tests will be created and existing ones will be overhauled.

## 16/4/2026
- Core:
	- Platform:
		- Fixed compilation of X11/Wayland not actually disabling the two properly when specified on the command line.
		- Wayland:
			- Fixed an issue where scrolling in Wayland windows would crash the application due to an invalid Wayland opcode.
				- Thanks to InhaledBus for identifying the problem being scrolling.
	- Math:
		- Added `lerp()` as a function to the `Math` class and our `Vector` classes.
- Tests:
	- Added unit tests for math functions.


## 15/4/2026
- Core:
	- Renamed `Item` to `Object` and `Instance` to `RefCounted`. 
		- This is closer to Godot, but the naming convention is too convenient to pass up. The function is the same, so familiarity is better.
	- Fixed an issue with `HashTable`s whereby the size was not updated to reflect the new size, leading to incorrect hashing information on retrieval. 
- Runtime:
	- Renamed `Object` to `GameObject` and updated derivatives.
		- Required for the above change to work. A longer name, but ultimately better for clarity. `Node` would fir the naming conventions but I would like to keep a semblance of uniqueness.
	- Changed `ObjectUI` to `UIObject`.
		- `GameObjectUI` would be correct, but UI will be used for the editor so the name doesn't quite work. It still inherits from `GameObject` (which is, admittedly, a poor name and might be changed) but it is unique enough to deserve a proper name like this.
	- Renamed `RenderingServer` to `RenderingManager`.
		- The server API is an idea from Godot. We prefer "managers" as our terminology but functionally they can go either way. 
- Testbed:
	- Updated editor to have a register/unregister classes method.
		- Seemed to fix an invalid access bug, but it may have been the result of the change from `Object` -> `GameObject` not being updated properly.
	- The editor code will be moved to a separate subfolder and developed once the game creation process is understood, for ideally obvious reasons. How APIs and scripting will work is up for debate.

## 14/4/2026
- Core:
	- Fixed some general memory leaks, most notably `List<T>` having no destructor, `display_manager` not being freed and a use-after-free with the `Event` setup in `DisplayManagerX11`.
		- The Event class may well be deprecated in favour of signals, as it is only used in the `DisplayManager` for destroying windows when requested which is done anyway during `DisplayManager::finalize()`.

## 13/4/2026
- Core:
	- OS:
		- Removed `os_internal.h` in favour of writing the creation functions in their own `.cpp` files.
			- Fixes a pointless include for a more elegant solution.
- Runtime:
	- UI:
		- Fix position not being applied properly when using non-top-left anchors.
			- Fixes a longstanding issue with how anchors were not acting properly. Also removes the `position` and `size` factors entirely to prevent further issues with the system.
	- Change the name of `runtime_setup` to `runtime_initialize`.
		- Naming consistency reasons.

## 5/4/2026
- Reordered the engine into separate "modules" for simplicity purposes. Core engine code, including platform init/shutdown, the type system, templates and core I/O now reside in `victoria.core`, with runtime features including rendering, asset importing, scene information and so forth being available via the `victoria.runtime` library. Freetype and GLAD have been split into two separate libraries that are not intended to be used by end-user code.

## 29/3/2026
- Buildsystem:
	- Allow each part of the engine to be built from their own Makefile, assuming the required dependencies exist.
		- Allows project-based IDEs (Visual Studio) to compiler their constituent parts. Further work to allow for Visual Studio projects is required.
	- We now automatically detect as to whether a compiler exists on the user's system, so as to prevent errors from arising as a result of compiling without the proper tools or command-line options enabled.
		- As a consequence, GCC is now the default compiler for non-Windows OSes, since it is far more common than Clang.

## 21/3/2026
- Buildsystem:
	- Overhauled the buildsystem to be far more feature-rich and modular. We now generate `compile_commands.json`, use `clangd` and `clang-format` for debugging and formattings, and generally have updated how we approach building files to now minimize external dependencies.
	- Fixed a longstanding issue with GCC failing to compile due to differences in signedness.
		- GCC will now compile properly on Linux.
- Core:
	- Reverted back to using standard C++ type names (`uint32_t` and the like).

## 18/1/2026
- Buildsystem:
	- Fixed compilation errors caused by compiling C code as C++.
		- We now use separate compilers for each.

## 11/1/2026
- Editor:
	- Began reworking the editor to our intended design.
		- Begins the phase of making the engine usable rather than a testing grounds for other stuff.
- Rendering:
	- Fixed an issue where font rendering was working incorrectly as a result of alpha blending not being applied.
		- Text should now be rendering as intended.

## 10/1/2026
- Rewrote the engine API as a shared library and moved all editor functionality to an executable.
	- Part of a wider change in how engine development will work. Godot (of which this can be considered a fork of) uses a single executable binary which works for their API but not for our use cases.
	- The API used for the editor will be the same as that used for games, with the editor autogenerating some of the content required (registering C++ types and the like).
- Bumped version to 0.2.0
	- Above change made it seem a good idea to do so.

## 8/1/2026
- Buildsystem:
	- Overhaul the buildsystem to now use a singular Makefile instead of one for Linux and Windows respectively.
- Platform:
	- Begin implementation of Wayland as another display manager implementation.
		- Required a change whereby multiple display drivers can be registered when registering a display backend. Victoria will always try to use the first backend that exists prior to any others.
		- Future implementations may add more advance settings to automatically specify what driver is being used over another before use.

## 15/10/2025
- Rendering:
	- Fixed an OpenGL error caused by a shader not being unbound.

## 12/10/2025
- Core:
	- Overhaul `HashTable` to be a more viable type across all subsystems.
		- Added new methods for finding a searching, added Robin-Hood hashing as our go-to, allowed for dynamic reallocation, and so on!
	- Add new files `hashfuncs`.
		- Useful hashing functions for different datatypes across our engine. Mainly for `HashTable`.
	- Changed `call()` in `CallableMethod` to return `Error` instead of `void`.
	- Added signal handling to both `Object` and `ClassRegistry`.
		- Not intended at first, but due to how our new hashtables work we needed to fix a bunch of other things first.
	- Added method `has()` to `List`.
		- Utility method. Updated other places where `find()` was used to the new method.
	- Added new `Error`s `ERR_ALREADY_EXISTS`, `ERR_DOESNT_EXIST` and `ERR_FAILED`.
- Platform:
	- Fixed multiple window resizing calls on X11.
- Scene:
	- Changed register order in `register_scene_classes.cpp`.
		- Some classes were being reigstered ahead of others, causing invalid lookups in `ClassRegistry`.		
	- Added `size_changed` signals to both `ObjectUI` and `Window`.
		- Part of the compatibility with the new hashtables, funnily enough.

## 10/10/2025
- Core:
	- Added `ConstIterator` struct to `List` and `Vector`.
		- Needed for iterating in const functions.
	- Added method `has()` to `Vector`.
		- Utility/replacement for `find()`.
	- Added method `replace()` to `String`.
		- Replaces every instance of character A with character B.
	- Added methods `is_suspended()` and `set_is_suspended()` to `OS`.
		- "Suspended" refers to when the application is minimized or otherwise not needing to run at any point. May move this to some kind of `Engine` class in the future if needed, as it's not *technically* an OS thing.
- Scene:
	- Added method `get_object_or_null()` and `get_object` to `Object`.
		- Checking up and down the scene tree. Needed for our use cases.
	- Added automatic naming to `Objects`.
		- Utility to support the above method.

## 4/10/2025
- Assets:
	- Updated `canvas_shader.frag` to use the model matrix position over the rect offset.
		- Temporary, should use both but need to ensure that the position is zeroed on the rect before we can use it.
- Core:
	- Implemented a `callables` HashTable that points to a list of `CallableMethod`s which can be called at any point.
		- Used currently for signals or our derivative of the Godot signal.
	- Added methods `connect_method()` and `emit_method()` to `Item`.
		- Connect and call to the above methods. Due for redesign.
- Editor:
	- Removed `root_scene` from being the root.
		- It was labelled as a test scene. We're past testing for now.
- Scene:
	- GUI:
		- Added method `get_parent_rect()` to `ObjectUI`.
			- Obtains the size of the parent rect, if it exists. If not, then the size of the currently-used viewport is applied instead.
		- Added internal methods `_rect_changed()`, `_update_minimum_size()`, `_update_anchors`, `_update_anchor` and `_update_canvas_item_transform` to `ObjectUI`.
			- For managing the anchors and internals of each `ObjectUI`. These methods may be exposed at some point in the future.
		- Added enum `Axis` to `ObjectUI`.
			- Discerns between X/Y axis when checking anchors. 
	- Main:
		- Added new method `get_child_count()` to `Object`.
			- Returns the number of children of the given `Object`. Saves having to request the child list directly.
		- Added internal method `_propagate_size_changed()` to `Viewport`.
			- Used when forcing `CanvasItems` to be redrawn
- Rendering:
	- Fixed intial values for the OpenGL viewport struct.
		- Some values would be non-zero which caused errors with texture drawing.

## 1/10/2025
- Core:
	- Modified `CallableMethodPointerBase` to no longer add the class name to the method, as well as removing the ampersand from text.
		- Fixes duplication of the class name when printing the `CallableMethod` name and class.
	- Added a window resized callback function to `DisplayManager`.
		- Allows for interop between API functions and the raw display.
- Scene:
	- Added private method `_window_resized_callback` to `Window`.
		- Callback function for detecting window size changes, resizes the viewport on the renderer side so that the root window draws the whole framebuffer.

## 27/9/2025
- Assets:
	- Added a `CopyShader` to the shader list.
		- `CopyShader`s simply copy a rectangle or triangle onto the screen in normalized device coordinates.
- Core:
	- Added `get()` methods to `List`.
	- Fixed crash message that would occur if `_copy_on_write()` was called with a refcount > 1.
	- Added prefix `ALWAYS_INLINE` to `typedefs.h`. 
		- Needed for the `predelete` in `memory.h` to be overriden by the one in `item.h`.
	- Added methods `get_memory_usage()` and `get_mem_max_usage()`.
- Editor:
	- Added a 3D viewport into the editor.
		- This will house the scenes we are editing.
- Platform:
	- Added method `OS.delete_main_loop()`.
		- Needed to begin the process of cleaning up our scene tree in most scenarios.
- Rendering:
	- Added core functionality for viewports into the OpenGL renderer.
		- Viewports allow for us to render and manage a sub-scene which is incredibly useful for the editor.
	- Separated out the OpenGL renderer into separate draw functions.
		- Needed to make life easier for us, as well as accomodating the above implementation.
	- Added base functionality to a `Canvas` item into the OpenGL renderer.
		- Canvases are surfaces that 2D/GUI objects can render to.
- Scene:
	- Added new class `ViewportContainer`.
		- Embedding object for a `Viewport` object. Allows for a sub-viewport to be rendered as a GUI element.
	- Added notification `NOTIFICATION_DRAW`.
	- Change `children` in Object from a `Vector` to a `List`.

## 22/9/2025
- Core:
	- Added class `MainLoop`.
		- A customizable version of `SceneTree` that can be used instead in a project. Designed to interface with the `DisplayManager` over the former due to X11 name conflicts.
	- Renamed `MainLoop` to `Main`.
		- To fix class naming conflicts with the above class.
- Rendering:
	- Add an implementation of `GL_KHR_debug` to improve OpenGL debugging and profiling.
		- Not every callback notification is outputted by default, some may need to be written to a debug file later on.
- Scene:
	- Added class `Viewport` and `Window`.
		- Objects within the game scene will render within a `Viewport`, which may or may not be a `Window` as well. This is part of a set of changes that will allow the editor to exist.
	- Added methods `get_viewport()` and `get_window()` to `Object`.
		- Any `Object`-derived class will be nested within a `SceneTree`, and since there may be more than one `Viewport` in a tree there needs to be a way for the `Object` to find such a parent. Finding the `Window` is also important for certain `Viewport` commands in the future.
	- Made `SceneTree` extend new class `MainLoop`.
		- Part of wider changes to the engine. See above.
	- Changed `root` to being a `Window` instead of an `Object`.
		- `SceneTree` is a singleton, and hence every game instance will need a window.

## 20/9/2025
- Scene:
	- Added class `ColourRect`.
		- A basic rectangle that has a colour attached to it.

## 19/9/2025
- Drivers:
	- Added function `resize_viewport` to EGL.
		- EGL can now have its contents scale with the viewport size.
- Rendering:
	- Fixed viewport width/height not updating on Linux, cauing weird screen ratios.

## 18/9/2025
- Buildsystem:
	- Re-added the compiler flag `-MD`.
		- Fixes an issue where modifying headers would not have the engine compile properly due to a lack of dependency tracking. Part of the bugs left over from EGL implementation.
- Rendering:
	- Implemented a base colour value into the 2D renderer.
		- Rects can now have pretty colours. This will blend with materials and textures to give an overall tint, should the user wish. Used via the function `item_set_colour()`.
	- Renamed and removed some unecessary functions from the RenderingServer.
		- Removed `item_set_size`; renamed `item_create_rect` to `item_set_rect` and `item_set_rect_uv_override` to `item_set_uv_rect`. Should improve clarification about what each function in the RenderingServer does.
- Scene:
	- Added node `TextureRect` as a child of `ObjectUI`.
		- A simple texture that can be added to the canvas.
	- Changed `Panel` to use colours instead of materials.
		- Partial change towards a potential `Theme` class that will define each colour value. Should make using Panels somewhat nicer.
	- Added function `canvas_set_colour` to `CanvasItem`.
		- Sets the base colour of the object. Default is `Vector4(1, 1, 1, 1)`.
	

## 13/9/2025
- Rendering:
	- Implemented a y-sorting algorithm into the renderer.
		- Items with a ysort of 0 are rendered first, then 1, and so forth. 
- Scene:
	- Added functions `get_ysort()` and `set_ysort()` to `CanvasItem`.

## 12/9/2025
- Rendering:
	- Fixed incorrect font offsets causing text to be lower down the screen than expected.
	- Moved text to have its root at the top-left corner of the text line
- Core:
	- Added `CHILD_ENTERED_TREE` and `CHILD_EXITED_TREE` notifications.
		- Children that are either being deleted or removed from their parent's ownership will now notify when this occurs.
- Scene:
	- Migrated `Object2D` and `ObjectUI`'s `_propagate_transform_changed()` functions to `CanvasItem`.
		- `CanvasItem` will now handle said propagation, to prevent some objects within the tree not updating their global transforms when added.

## 11/9/2025
- Core:
	- Implemented a default fallback to OpenGL native if a driver is unspecified.
- Tests:
	- Re-added tests back into the engine. Tests will now run on Linux, which was previously left unimplemented.

## 10/9/2025
- Buildsystem:
	- Revamped the core Linux Makefile in order to increase modularity and also to try and fix a handful of bugs.
		- Some changes may break some configurations. A reorder of said buildsystem is long overdue. May move to scons or another buuldsystem.
- Core:
	- Implemented new functions for memory allocation, `vnew()`, `vnew_placement()` and `vdelete`.
		- Related to a bug on Linux where trying to load shared objects would use the overriden new operator and attempt operations in userspace.
		- These should be used whenever allocating/freeing memory that doesn't need `Memory::vallocate()` or other functions over the default for memory tracking purposes.
	- Added a new error type, `ERR_BUG`.
		- Used for moments when an in-engine bug has occured, and a bug report must be filed. This should be a last-resort option only.
	- Renamed `gl_native` to `opengl` and add rendering option `opengl_es`.
		- OpenGL ES will be used primarily with EGL options, since it is not the native display driver on most PCs.
	- Removed command `--use-opengl`.
		- Replaced with `--rendering-driver`.
	- Added command `--rendering-driver`.
		- Can be specified as to what driver to use. Currently supports `opengl` for OpenGL native and `opengl_es` for EGL.
- Modules/Rendering:
	- Added a new option for a rendering interface, EGL.
		- EGL acts much like GLX or WGL, except for the fact that it is platform-agnostic. This will also allow for native ANGLE support in the future. Currently, EGL is set up for X11 only, but will eventually be ported to Windows with time.
- Thirdparty:
	- Added a GLX and EGL function loader to GLAD.
	- Updated licenses


## 12/7/2025
- Core:
	- Implement a font atlas in order to render text with less bitmap resources.
	- Added argument options to pass to resource importers.
		- Can be used to customise data import options.
	- Added two new functions, `next_po2(u32)` and `find_po2(u32)` to `typedefs.h`.
		- One acts to round up to the next power of 2 and the other acts as a version of `log2(n)` for unsigned integers.
- Modules:
	- Gave the font importer the ability to round upwards to the next power of 2 for font atlases.
		- Prevents issues with font bitmaps running out of space when loading a font into them. May not work for notably large font sizes, but designed for ranges 12 -> 64.
		- Feature may be disabled in SDF fonts when an appropriate shader is found for them.
- Rendering:
	- Added new `RenderingServer` functions `item_set_flag` and `item_set_rect_uv_override`.
		- Flags for more control over behaviour, the UV override for rendering smaller parts of textures for font atlases
	- Added new flag `ItemFlag::ITEM_FLAG_USE_TEXTURE_SUBCOORDS`.
- Resources:
	- Added new function `set_font_size(u32)` to `Font`.
	- Added new functions `get_bitmap_size() const` and `set_bitmap_size(u32)` to `Font`.
		- Useful for finding texture coordinates within a font from its position data.
	

## 1/7/2025
- Core:
	- Created new `WindowFlags` enum for the `DisplayManager`. Currently allows for windows to be made invisible upon creation, which is useful for obtaining system information that requires a window to exist.
- Platform:
	- Implemented obtaining OpenGL version, vendor and renderer for X11.
- Rendering:
	- Replace `Item::ITEM_FLAG_USE_GRAYSCALE` with `Item::ITEM_FLAG_ALPHA_ONLY`.
		- Change in order to clarify its intended purpose for a given texture.

## 28/6/2025
- GUI:
	- Fix a segmentation fault that occurs when a Text paragraph tries to load from a null Font resource.

## 27/6/2025
- Merged `text-rendering` PR. Following information is what's changed.
- Core:
	- Implemented `KeyValue<K, V>`.
		- A simple enough pair class for things such as Dictionaries to use.
	- Implemented `RBMap<T>`.
		- Mostly stolen from Godot, with some self-done algorithms by your truely. Possibly very broken and needs fixing at some point.
	- Modified `HashTable<T>` to use `KeyValue<K, V>` for `get()` and `get_ptr()`.
	- Make `FileSystem::open()` return a `Ref<FileSystem>` instead of `void` and be `static`.
		- To update new class behaviour as inheriting from `Resource` instead of being a stanalone class.
- GUI:
	- Added new class `Text`.
		- A string that is displayed on-screen in the UI layer.
	- Added `set_transform()` to `ObjectUI`.
		- Sets the local transform of the GUI element to whatever it needs to be.
- Modules:
	- Added `ResourceFormatImporterFont` as a new `ResourceFormatImporter`.
		- Does what it says on the tin using FreeType.
	- Moved `ResourceFormatImporter::get_extension_list()` from being virtual to in the base class.
		- It existed in every class the same way so it made sense to make it its own function.
- Planning:
	- Added a `TODO.md`.
- Rendering:
	- Added alpha-only textures for texture rects.
		- A fundamental part of text rendering. The actual data is held in the R channel, so using alpha-only allows us to filter out part of the texture that aren't being used.
		- Denoted in-engine with the flag `INSTANCE_USE_GRAYSCALE` or `ITEM_FLAG_USE_GRAYSCALE`. Will be updated to be `ALPHA_ONLY_CHANNEL` in the future.
	- Implement SDF as a valid texture format.
		- Uses `ITEM_FLAG_USE_SDF` and can be toggled per-texure using `RenderingServer::texture_use_sdf()`.
		- All fonts as of current use SDF as their default importing methods so that scaling is less egregious.
	- Disable the depth mask for the UI pass.
		- To prevent issues with overlapping characters. Will produce problems.
	- Add the TextureMask to `RenderingServer::texture_set_from_data()` as an optional argument.
		- Allows for specifing what kind of filters to use and clamping options to use.
- Resources:
	- Added new resource `Font`.
		- A collection of characters that can be displayed on-screen as a visual.

	

## 18/6/2025
- Core:
	- Moved `DisplayManager` to be finalized after the rendering server.
		- Linux had an issue with this messing up some stuff so it's fixed now.
- Rendering:
	- Free the vertex buffer instead of the vertex array in `RenderingServerGL::finalize()`.
		- The previous example was incorrect behaviour and has been now fixed.
	- Move `glBindVertexArray(0)` to its proper location in `RenderingServerGL::draw()` in the scene pass.
		- Fixes potential problems with writing to the wrong vertex array.

## 8/5/2025
- Buildsystem:
	- Added compilation of Freetype to Linux
		- This includes some flags that may affect code in the future
- Core:
	- Removed unecessary `<windows.h>` include from `memory.cpp`
	- Added define for `size_t` on Linux


## 4/5/2025
- Added `CHANGELOG.md` to the repository. 
- All previous commits before this are not to be counted.
