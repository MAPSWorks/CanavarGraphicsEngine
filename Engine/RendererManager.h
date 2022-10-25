#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "Camera.h"
#include "Manager.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObjectFormat>

namespace Canavar {
namespace Engine {

class NodeManager;
class CameraManager;
class LightManager;
class ShaderManager;
class ModelDataManager;
class Haze;
class Sky;
class Terrain;
class PointLight;
class Sun;

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
        Pong,
    };

    struct OpenGLFramebuffer {
        unsigned int fbo = 0;
        unsigned int rbo = 0;
        unsigned int texture = 0;
    };

    bool init() override;
    void resize(int width, int height) override;

    Node *getNodeByScreenPosition(int x, int y);

    void render(float ifps);

private:
    void setCommonUniforms();

    void deleteFramebuffers();
    void createFramebuffers(int width, int height);

private:
    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;
    ModelDataManager *mModelDataManager;

    Camera *mCamera;
    Sun *mSun;
    Sky *mSky;
    Haze *mHaze;
    Terrain *mTerrain;

    QVector<PointLight *> mClosePointLights;

    QMap<FramebufferType, QOpenGLFramebufferObject *> mFBOs;
    QMap<FramebufferType, QOpenGLFramebufferObjectFormat *> mFBOFormats;

    GLuint *mColorAttachments;

    int mWidth;
    int mHeight;

    OpenGLFramebuffer mMeshInfoFBO;

    DECLARE_MEMBER(int, BlurPass)
    DECLARE_MEMBER(float, Exposure)
    DECLARE_MEMBER(float, Gamma)
    DECLARE_MEMBER(bool, NodeSelectionEnabled)
};

} // namespace Engine
} // namespace Canavar

#endif // RENDERERMANAGER_H
