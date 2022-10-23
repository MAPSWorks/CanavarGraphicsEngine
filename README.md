# Canavar Graphics Engine

A basic graphics engine written with `C++` using `OpenGL API`.
I use `Qt 6` for window and event management.
All 3D math operation is done by `Qt 6`'s math module.
The engine has following features:
- Supports loading several 3D model formats thanks to `assimp` library
- Parent - child node hiearchy
- Transformation of individual meshes of models
- Particle generator
- Terrain, sky, sun, volumetric clouds, water, haze
- Dummy and free cameras
- Directional, point and spot lights
- Easy shader loading and linking
- Framebuffer generation with several attachments and different formats 

I will add following features:
- Editor
- WGS84 support
- Terrain generation using DTED
- Albedo generation using satellite images
- Post processing effects

## Build
All dependencies is under `Dependencies` folder. I already built the libraries which are needed to be dynamically linked. The compiler and kit I used for compiling those libraries and developing this engine is  `Qt 6.3.0 MSVC 2019 64bit`.
I have a post build script `PostBuild.bat` that does all necessary tasks for running the project. 
In order to run the project, all you need to do is to add a custom build step to `Qt Creator`. 
For the custom build step, set `command` as the absolute path of `PostBuild.bat`;
set `arguments` as `%{buildDir}`
and finally set the working directory as the project directory. 
Then you can compile with `Qt 6.3.0 MSVC 2019 64bit` kit;
the script should do all tasks in order to run the application.

## Usage
Creating a model can be done with one liner:
```
Node *aircraft = NodeManager::instance()->create(Node::NodeType::Model, "f16c");
```
`NodeManager` class automatically register this model to render list. You do not have to do anything else.
Ownership belongs to `NodeManager`. You can remove any node with one liner again.
```
NodeManager::instance()->removeNode(aircraft);
```
It is `NodeManager`'s responsibility to the clean up the resources.

Attaching a camera to a node is also easy:
```
Camera *camera = NodeManager::instance()->create(Node::NodeType::DummyCamera);
aircraft->addChild(camera);
```
You can set local position of `camera`:

```
camera->setPosition(QVector3D(0, 0, 10));
```
Now you can set `camera` as the active camera:

```
CameraManager::instance()->setActiveCamera(camera);
```

## Acknowledgements
I thank Federico Vaccaro for his amazing shaders and Joey de Vries for his OpenGL tutorials.
