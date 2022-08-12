#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "Light.h"
#include "LightManager.h"
#include "ModelData.h"
#include "ModelManager.h"
#include "ShaderManager.h"

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

private:
    QMap<Model::Type, ModelData *> mTypeToModelData;

    ModelManager *mModelManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    Light *mLight;
};

#endif // RENDERERMANAGER_H
