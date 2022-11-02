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

class SelectableNodeRenderer : public Manager, protected QOpenGLExtraFunctions
{
    Q_OBJECT
private:
    SelectableNodeRenderer();

public:
    struct NodeInfo {
        unsigned int nodeID;
        unsigned int meshID;
        unsigned int vertexID;
        unsigned int success;
    };

    static SelectableNodeRenderer *instance();

    bool init() override;
    void render(float ifps) override;
    void resize(int width, int height) override;

    NodeInfo getNodeInfoByScreenPosition(int x, int y);

private:
    ShaderManager *mShaderManager;
    ModelDataManager *mModelDataManager;
    NodeManager *mNodeManager;
    CameraManager *mCameraManager;

    OpenGLVertexArrayObject mCube;

    OpenGLFramebuffer mNodeInfoFBO;
    int mWidth;
    int mHeight;
    bool mResizeLater;
};

} // namespace Engine
} // namespace Canavar
#endif // SELECTABLENODERENDERER_H
