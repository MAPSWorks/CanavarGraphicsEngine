#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "Camera.h"
#include "Manager.h"
#include "ModelData.h"
#include "Quad.h"

#include <QOpenGLFramebufferObjectFormat>

namespace Canavar {
namespace Engine {

class NodeManager;
class CameraManager;
class LightManager;
class ShaderManager;
class Haze;
class Sky;
class Terrain;
class PointLight;

class RendererManager : public Manager, protected QOpenGLExtraFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    static RendererManager *instance();

    enum class FramebufferType { //
        Default,
        Temporary,
        Ping,
        Pong
    };

    bool init() override;
    void resize(int width, int height) override;

    void render(float ifps);

private:
    void loadModels(const QString &path, const QStringList &formats);
    void setCommonUniforms();

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
    Terrain *mTerrain;
    Quad *mQuad;

    QVector<PointLight *> mClosePointLights;

    QMap<FramebufferType, QOpenGLFramebufferObject *> mFBOs;
    QMap<FramebufferType, QOpenGLFramebufferObjectFormat *> mFBOFormats;

    GLuint *mColorAttachments;

    int mWidth;
    int mHeight;

    DECLARE_MEMBER(int, BlurPass)
    DECLARE_MEMBER(float, Exposure)
    DECLARE_MEMBER(float, Gamma)
};

} // namespace Engine
} // namespace Canavar

#endif // RENDERERMANAGER_H
