#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Manager.h"
#include "Node.h"
#include "OpenGLFramebuffer.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class CameraManager;
class LightManager;
class ModelDataManager;

class NodeManager : public Manager, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    explicit NodeManager(QObject *parent = nullptr);

public:
    static NodeManager *instance();

    // Manager interface
    bool init() override;
    void resize(int width, int height) override;

    Node *createNode(Node::NodeType type, const QString &name = QString());
    Model *createModel(const QString &modelName, const QString &name = QString());

    void removeNode(Node *node);

    Node *getNodeByID(int ID);
    Node *getNodeByName(const QString &name);
    Node *getNodeByScreenPosition(int x, int y);

    const QList<Node *> &nodes() const;

    void renderNodeInfoStuff();

private:
    void assignName(Node *node, const QString &name);

private:
    QList<Node *> mNodes;
    int mNumberOfNodes;

    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ModelDataManager *mModelDataManager;

    QMap<Node::NodeType, QString> mTypeToName;
    QMap<Node::NodeType, unsigned int> mTypeToCount;

    QMap<QString, int> mNames;

    OpenGLFramebuffer mNodeInfoFBO;
    int mWidth;
    int mHeight;
};

} // namespace Engine
} // namespace Canavar

#endif // NODEMANAGER_H
