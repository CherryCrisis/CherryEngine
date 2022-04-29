Changelog
===

*29-04-2022*
-
**[Adding]**:
* Assets browser (Drag & drop in sub-folders, from explorer to engine browser, search bar)
* File management (F2 as shortcut for renaming, display filename when hovered)
* Launcher
* Cell System (Entity display depending on current selected cell)
* PhysX (Callbacks for trigger and collision enter and exit, and collision stay)
* Editor (Material inspection)

**[Change]**:
* Serialization (Improve speed (optimization), purge chen changing scene)

**[Fixing]**:
* Parenting (Entity in hierarchie can now become child of entity after it, Entities keep their world transform when becoming child instead of local)
* Viewer (Subscribe/unsubscribe, light and models disapearing on game stop/restart)
* Cell system (Creating two physicScene for each cell -> now creates one)

*22-04-2022*
-
**[Adding]**:
* Cell System (Added cell manager, moving object between cells and physic per scene)
* Resources (Added texture generation from .fbx, texture compression)
* Picking (object picking in scene)
* Editor (Display current scene name, drag & drop of .fbx)
* Rendering (Renderpass system refont, Added normal map, Added shadow map, added post-process effects: HDR, bloom)
* Scripting (Added debugging info in C#)
* Release (Added Build/Build & Run)

**[Change]**:
* Serialization (System refont)
* Editor (Change graphic chart)
* Resources (Changed Hot-reload)
* PhysX (Object transform no longer overrided by physx tranformed)

**[Fixing]**:
* Camera movement
* Editor: Guizmo (fix guizmo rotation, added edition in local or global referential)
* Editor: Camera (fix rotation matrix calculation)
* Resource (Fixed asset reloading)

**[Other]**
* Remove warning related to PhysX
* Fix some warnings

*08-04-2022*
-
**[Adding]**:
* InputManager (Contextual base input, poll and fetch contexts)
* Editor (Scene & Game focus, Guizmo, Scene editor cameras movement, Add component)
* Standalone (Basic implementation)
* Debug (Log refont)
* Behaviour (Awake, Start, Update functions as events)
* Scripting (Hot reload)

*01-04-2022*
-
**[Adding]**:
* Serialization (inspector, saving/loading)
* Debug
* PhysX (intergration, structure)
* Editor (drag & drop)
* Resource manager (delete, hot reload)
* Scripting (add / get componenet)

*24-03-2022*
-
**[Adding]**:
* Input manager (Mouse, InputAction references)
* Event System
* Basic Serialization (UUID system)
* Scripting hotreload 

**[Fixing]**:
* Camera movement

*18-03-2022*
-
**[Adding]**:
* Input manager (Keyboard context, Axis)
* Editor (Input management, Scene display aspect ratio)
* Multithreading (Resource loading)
* Scripting (Serialization debut)
* Time manager (Deltatime and FixedDeltatime)
* Sound manager

*11-03-2022*
-
**[Adding]**:
* Scripting system (using mono and Swig)
* Input manager (Mouse callbacks)

**[Improving]**
* Content browser

*25-02-2022*
-
**[Adding]**:
* Editor (Content browser, scene and game viewer)
* Mathematic library (Vectors, matrices and quaternions)
* Generic resource manager (Textures, materials, meshes, models and shaders)
* Input manager (Keycodes)
* Renderpass system
* Lit rendering (Blinn-Phong)
* Skybox

