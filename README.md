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

## Screenshot
![](Screenshot_2022_10_23.png)

## Build
There are two projects; one is `CanavarGraphicsEngineEditor.pro` under `Engine` folder and the other one is `CanavarFlightSimulator.pro` under `Simulator` folder.
Compiling those projects are similar. So I will only describe how to compile `CanavarFlightSimulator.pro` project.

First clone the repo `git clone --recurse-submodules https://github.com/berkbavas/CanavarGraphicsEngine.git`.

Then add a `Custom Process Step` in order to run `PostBuild.bat` script.

Settings are as follows on my computer. Change those according to yours. Note the quotes `"%{buildDir}"`.

![image](https://user-images.githubusercontent.com/53399385/202234025-6aec414d-3049-4e95-8683-36033d42c27c.png)

Compile with `Qt 6.3.0 MSVC 2019 64bit` kit and then you can run the application.

Compiling `CanavarGraphicsEngineEditor.pro` is similar.

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
