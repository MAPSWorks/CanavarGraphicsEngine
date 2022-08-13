#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "Light.h"
#include "LightManager.h"
#include "ModelData.h"
#include "NodeManager.h"
#include "ShaderManager.h"
#include "TexturedModelData.h"

#include <QMap>
#include <QMatrix4x4>
#include <QObject>

class RendererManager : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    static RendererManager *instance();

    bool init();
    void render(float ifps);

private slots:
    void renderNode(Node *node);

private:
    QMap<Model::Type, ModelData *> mTypeToModelData;
    QMap<QString, TexturedModelData *> mNameToTexturedModelData;

    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    Light *mLight;
};

#endif // RENDERERMANAGER_H
