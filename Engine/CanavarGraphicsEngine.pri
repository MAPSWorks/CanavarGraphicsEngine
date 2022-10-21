HEADERS += \
    $$PWD/Camera.h \
    $$PWD/CameraManager.h \
    $$PWD/Common.h \
    $$PWD/Controller.h \
    $$PWD/DummyCamera.h \
    $$PWD/FreeCamera.h \
    $$PWD/Haze.h \
    $$PWD/Helper.h \
    $$PWD/HosekData.h \
    $$PWD/HosekFunctions.h \
    $$PWD/Light.h \
    $$PWD/LightManager.h \
    $$PWD/Manager.h \
    $$PWD/Material.h \
    $$PWD/Mesh.h \
    $$PWD/Model.h \
    $$PWD/ModelData.h \
    $$PWD/ModelDataNode.h \
    $$PWD/Mouse.h \
    $$PWD/Node.h \
    $$PWD/NodeManager.h \
    $$PWD/PerspectiveCamera.h \
    $$PWD/RendererManager.h \
    $$PWD/Shader.h \
    $$PWD/ShaderManager.h \
    $$PWD/Sky.h \
    $$PWD/Speed.h \
    $$PWD/Sun.h \
    $$PWD/TileGenerator.h

SOURCES += \
    $$PWD/Camera.cpp \
    $$PWD/CameraManager.cpp \
    $$PWD/Controller.cpp \
    $$PWD/DummyCamera.cpp \
    $$PWD/FreeCamera.cpp \
    $$PWD/Haze.cpp \
    $$PWD/Helper.cpp \
    $$PWD/Light.cpp \
    $$PWD/LightManager.cpp \
    $$PWD/Manager.cpp \
    $$PWD/Material.cpp \
    $$PWD/Mesh.cpp \
    $$PWD/Model.cpp \
    $$PWD/ModelData.cpp \
    $$PWD/ModelDataNode.cpp \
    $$PWD/Mouse.cpp \
    $$PWD/Node.cpp \
    $$PWD/NodeManager.cpp \
    $$PWD/PerspectiveCamera.cpp \
    $$PWD/RendererManager.cpp \
    $$PWD/Shader.cpp \
    $$PWD/ShaderManager.cpp \
    $$PWD/Sky.cpp \
    $$PWD/Speed.cpp \
    $$PWD/Sun.cpp \
    $$PWD/TileGenerator.cpp

INCLUDEPATH += $$PWD/Dependencies/Assimp/include
DEPENDPATH += $$PWD/Dependencies/Assimp/include

LIBS += -L$$PWD/Dependencies/Assimp/lib/ -lassimp-vc142-mt

RESOURCES += \
    $$PWD/Resources.qrc
