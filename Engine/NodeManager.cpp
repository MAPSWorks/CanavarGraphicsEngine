#include "NodeManager.h"
#include "CameraManager.h"
#include "Config.h"
#include "DummyCamera.h"
#include "DummyNode.h"
#include "FirecrackerEffect.h"
#include "FreeCamera.h"
#include "Haze.h"
#include "Helper.h"
#include "LightManager.h"
#include "Model.h"
#include "ModelDataManager.h"
#include "NozzleEffect.h"
#include "PersecutorCamera.h"
#include "SelectableNodeRenderer.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Canavar::Engine::NodeManager::NodeManager()
    : Manager()
    , mNumberOfNodes(0)

{
    mTypeToName.insert(Node::NodeType::DummyCamera, "Dummy Camera");
    mTypeToName.insert(Node::NodeType::DummyNode, "Dummy Node");
    mTypeToName.insert(Node::NodeType::FreeCamera, "Free Camera");
    mTypeToName.insert(Node::NodeType::Model, "Model");
    mTypeToName.insert(Node::NodeType::PointLight, "Point Light");
    mTypeToName.insert(Node::NodeType::NozzleEffect, "Nozzle Effect");
    mTypeToName.insert(Node::NodeType::FirecrackerEffect, "Firecracker Effect");
    mTypeToName.insert(Node::NodeType::PersecutorCamera, "Persecutor Camera");
}

bool Canavar::Engine::NodeManager::init()
{
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mModelDataManager = ModelDataManager::instance();

    mNodes << Sun::instance();
    Sun::instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    mNodes << Sky::instance();
    Sky::instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    mNodes << Haze::instance();
    Haze::instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    mNodes << Terrain::instance();
    Terrain::instance()->mID = mNumberOfNodes;
    mNumberOfNodes++;

    return true;
}

void Canavar::Engine::NodeManager::postInit()
{
    auto doc = Helper::loadJson(Config::instance()->getWorldFilePath());

    if (doc.isNull() || doc.isNull())
    {
        qCritical() << "World json at" << Config::instance()->getWorldFilePath() << "is not valid.";
        return;
    }

    auto array = doc["nodes"].toArray();

    QMap<QString, QString> childToParentMap;

    for (const auto &e : array)
    {
        auto object = e.toObject();

        if (!object["parent"].toString().isEmpty())
            childToParentMap.insert(object["uuid"].toString(), object["parent"].toString());

        Node::NodeType type = (Node::NodeType) object["type"].toInt();

        switch (type)
        {
        case Node::NodeType::DummyNode:
        case Node::NodeType::FreeCamera:
        case Node::NodeType::DummyCamera:
        case Node::NodeType::PointLight:
        case Node::NodeType::NozzleEffect:
        case Node::NodeType::FirecrackerEffect:
        case Node::NodeType::PersecutorCamera:
            createNode(type, object["name"].toString())->fromJson(object);
            break;

        case Node::NodeType::Model:
            createModel(object["model_name"].toString(), object["name"].toString())->fromJson(object);
            break;
        case Node::NodeType::Sun:
            Sun::instance()->fromJson(object);
            break;
        case Node::NodeType::Sky:
            Sky::instance()->fromJson(object);
            break;
        case Node::NodeType::Haze:
            Haze::instance()->fromJson(object);
            break;
        case Node::NodeType::Terrain:
            Terrain::instance()->fromJson(object);
            break;
        default:
            qWarning() << Q_FUNC_INFO << "Unknown node type: " << (int) type;
            break;
        }
    }

    // Set parents
    auto childUUIDs = childToParentMap.keys();
    for (const auto &childUUID : childUUIDs)
    {
        Node *node = getNodeByUUID(childUUID);
        node->setParent(getNodeByUUID(childToParentMap.value(childUUID)));
    }
}

Canavar::Engine::Node *Canavar::Engine::NodeManager::createNode(Node::NodeType type, const QString &name)
{
    Node *node = nullptr;
    switch (type)
    {
    case Node::NodeType::DummyNode: {
        node = new DummyNode;
        break;
    }
    case Node::NodeType::Model: {
        qWarning() << "Use createModel() method instead!";
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
    case Node::NodeType::NozzleEffect: {
        NozzleEffect *effect = new NozzleEffect;
        effect->create();
        node = effect;
        break;
    }
    case Node::NodeType::FirecrackerEffect: {
        FirecrackerEffect *effect = new FirecrackerEffect;
        effect->create();
        node = effect;
        break;
    }
    case Node::NodeType::PersecutorCamera: {
        node = new PersecutorCamera;
        mCameraManager->addCamera(dynamic_cast<PersecutorCamera *>(node));
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Implement construction algorithm for this NodeType:" << (int) type;
        return nullptr;
    }
    }

    if (node)
    {
        assignName(node, name);
        node->mID = mNumberOfNodes;
        mNodes << node;
        mNumberOfNodes++;

        emit nodeCreated(node);
    }

    return node;
}

Canavar::Engine::Model *Canavar::Engine::NodeManager::createModel(const QString &modelName, const QString &name)
{
    Model *model = new Model(modelName);

    assignName(model, name);
    model->mID = mNumberOfNodes;
    mNodes << model;
    mNumberOfNodes++;

    emit nodeCreated(model);

    return model;
}

void Canavar::Engine::NodeManager::removeNode(Node *node)
{
    if (node == nullptr)
        return;

    switch (node->getType())
    {
    case Node::NodeType::Model:
    case Node::NodeType::DummyNode:
    case Node::NodeType::NozzleEffect:
    case Node::NodeType::FirecrackerEffect: {
        // TODO: What will happen to node's children?
        if (node->mParent)
            node->mParent->removeChild(node);

        for (auto &child : node->children())
            child->setParent(nullptr);

        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::PersecutorCamera:
    case Node::NodeType::DummyCamera:
    case Node::NodeType::FreeCamera: {
        if (node->mParent)
            node->mParent->removeChild(node);

        for (auto &child : node->children())
            child->setParent(nullptr);

        mCameraManager->removeCamera(dynamic_cast<Camera *>(node));
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    case Node::NodeType::PointLight: {
        if (node->mParent)
            node->mParent->removeChild(node);

        for (auto &child : node->children())
            child->setParent(nullptr);

        mLightManager->removeLight(dynamic_cast<Light *>(node));
        mNodes.removeAll(node);
        node->deleteLater();
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Unknown Node. Implement deletion algorithm for this NodeType:" << (int) node->getType();
        break;
    }
    }
}

Canavar::Engine::Node *Canavar::Engine::NodeManager::getNodeByID(int ID)
{
    for (const auto &node : mNodes)
        if (node->getID() == ID)
            return node;

    return nullptr;
}

Canavar::Engine::Node *Canavar::Engine::NodeManager::getNodeByUUID(const QString &uuid)
{
    for (const auto &node : mNodes)
        if (node->getUUID() == uuid)
            return node;

    return nullptr;
}

Canavar::Engine::Node *Canavar::Engine::NodeManager::getNodeByName(const QString &name)
{
    for (const auto &node : mNodes)
        if (node->getName() == name)
            return node;

    return nullptr;
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

void Canavar::Engine::NodeManager::toJson(QJsonObject &object)
{
    QJsonArray array;

    for (auto &node : mNodes)
    {
        QJsonObject nodeJson;
        node->toJson(nodeJson);
        array.append(nodeJson);
    }

    object.insert("nodes", array);
}

void Canavar::Engine::NodeManager::assignName(Node *node, const QString &name)
{
    QString newName = name;

    if (newName.isEmpty())
    {
        newName = mTypeToName.value(node->mType);

        if (auto model = dynamic_cast<Model *>(node))
            newName += " " + model->getModelName();

        node->setName(newName);
    } else
        node->setName(name);
}

Canavar::Engine::Node *Canavar::Engine::NodeManager::getNodeByScreenPosition(int x, int y)
{
    auto info = SelectableNodeRenderer::instance()->getNodeInfoByScreenPosition(x, y);

    return getNodeByID(info.nodeID);
}
