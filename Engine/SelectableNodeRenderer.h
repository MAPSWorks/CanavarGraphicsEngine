#ifndef SELECTABLENODERENDERER_H
#define SELECTABLENODERENDERER_H

#include "Manager.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLVertexArrayObject.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class ShaderManager;
class ModelDataManager;
class NodeManager;
class CameraManager;
class RendererManager;
class Config;

class SelectableNodeRenderer : public Manager, protected QOpenGLExtraFunctions
{
    Q_OBJECT
private:
    SelectableNodeRenderer();

public:
    struct NodeInfo {
        int nodeID;
        int meshID;
        int vertexID;
        int success;
    };

    static SelectableNodeRenderer *instance();

    bool init() override;
    void render(float ifps) override;
    void resize(int width, int height) override;

    NodeInfo getNodeInfoByScreenPosition(int x, int y);
    NodeInfo getVertexInfoByScreenPosition(int x, int y);

private:
    Config *mConfig;
    ShaderManager *mShaderManager;
    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    RendererManager *mRendererManager;

    OpenGLVertexArrayObject mCube;

    OpenGLFramebuffer mNodeInfoFBO;
    int mWidth;
    int mHeight;
    bool mResizeLater;
};

} // namespace Engine
} // namespace Canavar
#endif // SELECTABLENODERENDERER_H
