#include "NodeManager.h"
#include "CameraManager.h"
#include "DummyCamera.h"
#include "FreeCamera.h"
#include "LightManager.h"
#include "Model.h"

NodeManager::NodeManager(QObject *parent)
    : QObject(parent)
    , mNumberOfNodes(0)
    , mSelectedNode(nullptr)
{
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
}

Node *NodeManager::create(Node::NodeType type, const QString &name)
{
    switch (type)
    {
    case Node::NodeType::DummyNode: {
        Node *node = new Node;
        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);
        mNodes << node;
        mNumberOfNodes++;
        return node;
    }
    case Node::NodeType::Model: {
        Model *node = new Model(name);
        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);
        mNodes << node;
        mNumberOfNodes++;
        return node;
    }
    case Node::NodeType::FreeCamera: {
        FreeCamera *node = new FreeCamera;
        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);
        mNodes << node;
        mNumberOfNodes++;
        mCameraManager->addCamera(node);
        return node;
    }
    case Node::NodeType::DummyCamera: {
        DummyCamera *node = new DummyCamera;
        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);
        mNodes << node;
        mNumberOfNodes++;
        mCameraManager->addCamera(node);
        return node;
    }
    case Node::NodeType::DirectionalLight: {
        DirectionalLight *node = new DirectionalLight;
        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);
        mNodes << node;
        mNumberOfNodes++;
        mLightManager->setDirectionalLight(node);

        return node;
    }
    case Node::NodeType::PointLight: {
        PointLight *node = new PointLight;
        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);
        mNodes << node;
        mNumberOfNodes++;
        mLightManager->addLight(node);
        return node;
    }
    case Node::NodeType::SpotLight: {
        SpotLight *node = new SpotLight;
        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);
        mNodes << node;
        mNumberOfNodes++;
        mLightManager->addLight(node);
        return node;
    }
    break;
    }
}

void NodeManager::removeNode(Node *node)
{
    // FIXME: If a node is removed, remove also its all children
    Node *parent = dynamic_cast<Node *>(node->parent());

    switch (node->nodeType())
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
    case Node::NodeType::DirectionalLight: {
        if (parent)
            parent->removeChild(node);
        DirectionalLight *light = dynamic_cast<DirectionalLight *>(node);
        mLightManager->setDirectionalLight(nullptr);
        mNodes.removeAll(node);
        light->deleteLater();
    }
    case Node::NodeType::PointLight:
    case Node::NodeType::SpotLight: {
        if (parent)
            parent->removeChild(node);
        Light *light = dynamic_cast<Light *>(node);
        mLightManager->removeLight(light);
        mNodes.removeAll(node);
        light->deleteLater();
        break;
    }
    }
}

NodeManager *NodeManager::instance()
{
    static NodeManager instance;
    return &instance;
}

const QList<Node *> &NodeManager::nodes() const
{
    return mNodes;
}

void NodeManager::drawGui()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Nodes");

    QString preview = mSelectedNode ? mSelectedNode->name() : "-";
    if (ImGui::BeginCombo("Nodes", preview.toStdString().c_str()))
    {
        for (int i = 0; i < mNodes.size(); ++i)
            populateComboBox(mNodes[i]);

        ImGui::EndCombo();
    }

    if (mSelectedNode)
    {
        ImGui::Text("Type: %s", mSelectedNode->nodeTypeString().toStdString().c_str());
        ImGui::Text("Parent: 0x%p", static_cast<void *>(mSelectedNode->parent()));

        mSelectedNode->drawGui();
    }
}

void NodeManager::populateComboBox(Node *node)
{
    switch (node->nodeType())
    {
    case Node::NodeType::FreeCamera:
    case Node::NodeType::DirectionalLight:
        return;
    default:
        break;
    }

    if (ImGui::Selectable(node->name().toStdString().c_str()))
        mSelectedNode = node;
}
