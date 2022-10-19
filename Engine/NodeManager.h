#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Manager.h"
#include "Node.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class CameraManager;
class LightManager;
class RendererManager;

class NodeManager : public Manager
{
    Q_OBJECT
private:
    explicit NodeManager(QObject *parent = nullptr);

public:
    static NodeManager *instance();

    bool init() override;

    Node *createNode(Node::NodeType type, const QString &name = QString());

    void removeNode(Node *node);

    const QList<Node *> &nodes() const;

private:
    QList<Node *> mNodes;
    int mNumberOfNodes;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    QMap<Node::NodeType, QString> mTypeToName;
};

} // namespace Engine
} // namespace Canavar

#endif // NODEMANAGER_H
