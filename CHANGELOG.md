# Changelog
Changes exist in chronological order (i.e. new changes are to be appended to the end of the file). Dates are done in DD/MM/YYYY format with the version number applied to each date if needed.

## 21/3/2026
- Buildsystem:
	- Overhauled the buildsystem to be far more feature-rich and modular. We now generate `compile_commands.json`, use `clangd` and `clang-format` for debugging and formattings, and generally have updated how we approach building files to now minimize external dependencies.

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
