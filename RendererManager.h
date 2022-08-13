#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "Light.h"
#include "LightManager.h"
#include "ModelData.h"
#include "ModelManager.h"
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
    void renderModels(float ifps);
    void renderTexturedModels(float ifps);
    void renderTexturedModelDataNode(TexturedModelData *data, TexturedModelDataNode *node);

private:
    QMap<Model::Type, ModelData *> mTypeToModelData;
    QMap<QString, TexturedModelData *> mPathToTexturedModelData;

    ModelManager *mModelManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    Light *mLight;
};

#endif // RENDERERMANAGER_H
