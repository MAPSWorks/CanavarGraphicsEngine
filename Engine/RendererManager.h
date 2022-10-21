#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "Camera.h"
#include "Haze.h"
#include "Manager.h"
#include "ModelData.h"
#include "Sky.h"

namespace Canavar {
namespace Engine {

class NodeManager;
class CameraManager;
class LightManager;
class ShaderManager;

class RendererManager : public Manager, protected QOpenGLExtraFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    static RendererManager *instance();

    bool init() override;
    void resize(int width, int height) override;

    void render(float ifps);

private:
    void loadModels();
    void renderNodes();

private:
    QMap<QString, ModelData *> mModelsData;

    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    Sun *mSun;
    Sky *mSky;
    Haze *mHaze;

    RenderParameters mRenderParameters;
};

} // namespace Engine
} // namespace Canavar

#endif // RENDERERMANAGER_H
