#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "Camera.h"
#include "Manager.h"
#include "OpenGLVertexArrayObject.h"
#include "SelectedMeshParameters.h"

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
class Config;

class RendererManager : public Manager, protected QOpenGLExtraFunctions
{
    Q_OBJECT

private:
    RendererManager();

public:
    static RendererManager *instance();

    enum class FramebufferType { //
        Default,
        Temporary,
        Ping,
        Pong,
    };

    bool init() override;
    void resize(int width, int height) override;

    void render(float ifps) override;

    void addSelectableNode(Node *node, QVector4D color = QVector4D(1, 1, 1, 1));
    void removeSelectableNode(Node *node);

    void addSelectedMesh(Model *model, const SelectedMeshParameters &parameters);
    void removeSelectedMesh(Model *model);

    SelectedMeshParameters getSelectedMeshParameters(Model *model) const;
    SelectedMeshParameters &getSelectedMeshParameters_Ref(Model *model);

    const QMap<Model *, SelectedMeshParameters> &getSelectedMeshes() const;

private:
    void setCommonUniforms();
    void deleteFramebuffers();
    void createFramebuffers(int width, int height);

    void onSelectedNodeDestroyed();
    void onSelectedModelDestroyed();

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
    Config *mConfig;

    QVector<PointLight *> mClosePointLights;

    QMap<Node *, QVector4D> mSelectableNodes; // Nodes whose AABB to be rendered -> Line color
    QMap<Model *, SelectedMeshParameters> mSelectedMeshes;

    QMap<FramebufferType, QOpenGLFramebufferObject *> mFBOs;
    QMap<FramebufferType, QOpenGLFramebufferObjectFormat *> mFBOFormats;

    int mWidth;
    int mHeight;

    DEFINE_MEMBER(int, BlurPass)
    DEFINE_MEMBER(float, Exposure)
    DEFINE_MEMBER(float, Gamma)

    OpenGLVertexArrayObject mQuad;
    OpenGLVertexArrayObject mCube;
    OpenGLVertexArrayObject mCubeStrip;

    GLuint mColorAttachments[2];
};

} // namespace Engine
} // namespace Canavar

#endif // RENDERERMANAGER_H
