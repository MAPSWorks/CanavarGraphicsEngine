QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


include(Dependencies/qtimgui/qtimgui.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

HEADERS += \
    Camera.h \
    CameraManager.h \
    Controller.h \
    DummyCamera.h \
    FreeCamera.h \
    Helper.h \
    Light.h \
    LightManager.h \
    Material.h \
    Mesh.h \
    Model.h \
    ModelData.h \
    ModelManager.h \
    Node.h \
    RendererManager.h \
    ShaderManager.h \
    Texture.h \
    TexturedMaterial.h \
    TexturedModel.h \
    TexturedModelData.h \
    TexturedModelDataNode.h \
    Window.h

SOURCES += \
    Camera.cpp \
    CameraManager.cpp \
    Controller.cpp \
    DummyCamera.cpp \
    FreeCamera.cpp \
    Helper.cpp \
    Light.cpp \
    LightManager.cpp \
    Main.cpp \
    Material.cpp \
    Mesh.cpp \
    Model.cpp \
    ModelData.cpp \
    ModelManager.cpp \
    Node.cpp \
    RendererManager.cpp \
    ShaderManager.cpp \
    Texture.cpp \
    TexturedMaterial.cpp \
    TexturedModel.cpp \
    TexturedModelData.cpp \
    TexturedModelDataNode.cpp \
    Window.cpp

INCLUDEPATH += $$PWD/Dependencies/Eigen/
INCLUDEPATH += $$PWD/Dependencies/Assimp/include
DEPENDPATH += $$PWD/Dependencies/Assimp/include

LIBS += -L$$PWD/Dependencies/Assimp/lib/ -lassimp-vc142-mt


