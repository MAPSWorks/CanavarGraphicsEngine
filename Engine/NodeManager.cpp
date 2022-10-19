#include "NodeManager.h"
#include "CameraManager.h"
#include "DummyCamera.h"
#include "FreeCamera.h"
#include "LightManager.h"
#include "Model.h"

Canavar::Engine::NodeManager::NodeManager(QObject *parent)
    : Manager(parent)
    , mNumberOfNodes(0)

{
    mTypeToName.insert(Node::NodeType::DummyCamera, "Dummy Camera");
    mTypeToName.insert(Node::NodeType::DummyNode, "Dummy Node");
    mTypeToName.insert(Node::NodeType::FreeCamera, "Free Camera");
    mTypeToName.insert(Node::NodeType::Model, "Model");
    mTypeToName.insert(Node::NodeType::Sun, "Sun");
}

bool Canavar::Engine::NodeManager::init()
{
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();

    return true;
}

Canavar::Engine::Node *Canavar::Engine::NodeManager::createNode(Node::NodeType type, const QString &name)
{
    Node *node = nullptr;
    switch (type)
    {
    case Node::NodeType::DummyNode: {
        node = new Node;
        break;
    }
    case Node::NodeType::Model: {
        node = new Model(name);
        break;
    }
    case Node::NodeType::FreeCamera: {
        node = new FreeCamera;
        mCameraManager->addCamera(dynamic_cast<FreeCamera *>(node));
        break;
    }
    case Node::NodeType::DummyCamera: {
        node = new DummyCamera;
        mCameraManager->addCamera(dynamic_cast<DummyCamera *>(node));
        break;
    }
    case Node::NodeType::Sun: {
        node = new Sun;
        mLightManager->setSun(dynamic_cast<Sun *>(node));
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Implement construction algorithm for this NodeType:" << (int) type;
        return nullptr;
    }
    }

    if (node)
    {
        mNumberOfNodes++;

        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(mTypeToName.value(node->mType)).arg(mNumberOfNodes));
        else
            node->setName(name);

        mNodes << node;
    }

    return node;
}

void Canavar::Engine::NodeManager::removeNode(Node *node)
{
    // FIXME: If a node is removed, remove also its all children
    Node *parent = dynamic_cast<Node *>(node->parent());

    switch (node->type())
    {
    case Node::NodeType::Model:
    case Node::NodeType::DummyNode: {
        if (parent)
            parent->removeChild(node);
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::DummyCamera:
    case Node::NodeType::FreeCamera: {
        if (parent)
            parent->removeChild(node);
        mCameraManager->removeCamera(dynamic_cast<Camera *>(node));
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::Sun: {
        if (parent)
            parent->removeChild(node);
        Sun *light = dynamic_cast<Sun *>(node);
        if (mLightManager->sun() == light)
            mLightManager->setSun(nullptr);
        mNodes.removeAll(node);
        light->deleteLater();
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Unkown Node. Implement deletion algorithm for this Node";
    }
    }
}

Canavar::Engine::NodeManager *Canavar::Engine::NodeManager::instance()
{
    static NodeManager instance;
    return &instance;
}

const QList<Canavar::Engine::Node *> &Canavar::Engine::NodeManager::nodes() const
{
    return mNodes;
}
