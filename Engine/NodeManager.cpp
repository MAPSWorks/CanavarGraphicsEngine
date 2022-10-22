#include "NodeManager.h"
#include "CameraManager.h"
#include "DummyCamera.h"
#include "FreeCamera.h"
#include "LightManager.h"
#include "Model.h"

#include "Haze.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"

Canavar::Engine::NodeManager::NodeManager(QObject *parent)
    : Manager(parent)
    , mNumberOfNodes(0)

{
    mTypeToName.insert(Node::NodeType::DummyCamera, "Dummy Camera");
    mTypeToName.insert(Node::NodeType::DummyNode, "Dummy Node");
    mTypeToName.insert(Node::NodeType::FreeCamera, "Free Camera");
    mTypeToName.insert(Node::NodeType::Model, "Model");
    mTypeToName.insert(Node::NodeType::PointLight, "Point Light");
}

bool Canavar::Engine::NodeManager::init()
{
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();

    mNodes << Sun::instance();
    mNumberOfNodes++;

    mNodes << Sky::instance();
    mNumberOfNodes++;

    mNodes << Haze::instance();
    mNumberOfNodes++;

    mNodes << Terrain::instance();
    mNumberOfNodes++;

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
    case Node::NodeType::PointLight: {
        node = new PointLight;
        mLightManager->addLight(dynamic_cast<Light *>(node));
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Implement construction algorithm for this NodeType:" << (int) type;
        return nullptr;
    }
    }

    if (node)
    {
        mTypeToCount.insert(type, mTypeToCount.value(type, 0) + 1);
        mNumberOfNodes++;
        assignName(node, name);
        mNodes << node;
    }

    return node;
}

void Canavar::Engine::NodeManager::removeNode(Node *node)
{
    // FIXME: If a node is removed, remove also its all children
    switch (node->type())
    {
    case Node::NodeType::Model:
    case Node::NodeType::DummyNode: {
        if (node->mParent)
            node->mParent->removeChild(node);
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::DummyCamera:
    case Node::NodeType::FreeCamera: {
        if (node->mParent)
            node->mParent->removeChild(node);
        mCameraManager->removeCamera(dynamic_cast<Camera *>(node));
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::PointLight: {
        if (node->mParent)
            node->mParent->removeChild(node);
        mLightManager->removeLight(dynamic_cast<Light *>(node));
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Unkown Node. Implement deletion algorithm for this NodeType:" << (int) node->type();
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

void Canavar::Engine::NodeManager::assignName(Node *node, const QString &name)
{
    QString newName = name;

    if (newName.isEmpty())
        newName = mTypeToName.value(node->mType);

    int count = mNames.value(newName, 0);

    if (mNames.value(newName, 0) == 0)
        node->setName(newName);
    else
        node->setName(newName + " " + QString::number(count));

    mNames.insert(newName, ++count);
}
