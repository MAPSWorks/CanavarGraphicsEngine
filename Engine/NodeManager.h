#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Manager.h"
#include "Node.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class CameraManager;
class LightManager;
class ModelDataManager;

class NodeManager : public Manager
{
    Q_OBJECT
private:
    NodeManager();

public:
    static NodeManager *instance();

    bool init() override;
    void postInit() override;

    Node *createNode(Node::NodeType type, const QString &name = QString());
    Model *createModel(const QString &modelName, const QString &name = QString());

    void removeNode(Node *node);

    Node *getNodeByID(int ID);
    Node *getNodeByUUID(const QString &uuid);
    Node *getNodeByName(const QString &name);
    Node *getNodeByScreenPosition(int x, int y);

    const QList<Node *> &nodes() const;

    void toJson(QJsonObject &object);

signals:
    void nodeCreated(Canavar::Engine::Node *node);

private:
    void assignName(Node *node, const QString &name);

private:
    QList<Node *> mNodes;
    int mNumberOfNodes;

    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ModelDataManager *mModelDataManager;

    QMap<Node::NodeType, QString> mTypeToName;
};

} // namespace Engine
} // namespace Canavar

#endif // NODEMANAGER_H
