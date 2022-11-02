#ifndef SELECTABLENODERENDERER_H
#define SELECTABLENODERENDERER_H

#include "Manager.h"
#include "Node.h"
#include "OpenGLFramebuffer.h"

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
    explicit SelectableNodeRenderer(QObject *parent = nullptr);

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

    void add(Node *node);
    void remove(Node *node);

    const QList<Node *> &renderList() const;

private:
    void onObjectDestroyed(QObject *object);

private:
    ShaderManager *mShaderManager;
    ModelDataManager *mModelDataManager;
    NodeManager *mNodeManager;
    CameraManager *mCameraManager;

    QList<Node *> mRenderList; // Nodes whose AABB to be rendered

    OpenGLFramebuffer mNodeInfoFBO;
    int mWidth;
    int mHeight;
};

} // namespace Engine
} // namespace Canavar
#endif // SELECTABLENODERENDERER_H
