#include "NodeManager.h"
#include "CameraManager.h"
#include "DummyCamera.h"
#include "FreeCamera.h"
#include "LightManager.h"
#include "Model.h"
#include "NozzleParticles.h"

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
    case Node::NodeType::DirectionalLight: {
        node = new DirectionalLight;
        mLightManager->setDirectionalLight(dynamic_cast<DirectionalLight *>(node));
        break;
    }
    case Node::NodeType::PointLight: {
        node = new PointLight;
        mLightManager->addLight(dynamic_cast<PointLight *>(node));
        break;
    }
    case Node::NodeType::SpotLight: {
        node = new SpotLight;
        mLightManager->addLight(dynamic_cast<SpotLight *>(node));
        break;
    }
    case Node::NodeType::NozzleParticles: {
        node = new NozzleParticles;
        dynamic_cast<NozzleParticles *>(node)->create();
        break;
    }
    default: {
        qWarning() << Q_FUNC_INFO << "Implement creation algorithm for this NodeType:" << (int) type;
        return nullptr;
    }
    }

    if (node)
    {
        mNumberOfNodes++;

        if (name.isEmpty())
            node->setName(QString("%1 #%2").arg(node->nodeTypeString()).arg(mNumberOfNodes));
        else
            node->setName(name);

        node->setNodeIndex(mNumberOfNodes);
        mNodes << node;
    }

    return node;
}

void NodeManager::removeNode(Node *node)
{
    // FIXME: If a node is removed, remove also its all children
    Node *parent = dynamic_cast<Node *>(node->parent());

    switch (node->nodeType())
    {
    case Node::NodeType::Model:
    case Node::NodeType::DummyNode:
    case Node::NodeType::NozzleParticles: {
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

void NodeManager::setSelectedNode(Node *node)
{
    if (mSelectedNode == node)
        return;

    if (node)
        node->setSelected(true);

    if (mSelectedNode)
        mSelectedNode->setSelected(false);

    mSelectedNode = node;
}

void NodeManager::setSelectedNode(unsigned int nodeIndex)
{
    for (auto node : qAsConst(mNodes))
    {
        if (node->nodeIndex() == nodeIndex)
        {
            setSelectedNode(node);
            return;
        }
    }

    setSelectedNode(nullptr);
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
    if (ImGui::BeginCombo("Select a node", preview.toStdString().c_str()))
    {
        for (int i = 0; i < mNodes.size(); ++i)
            populateComboBox(mNodes[i]);

        ImGui::EndCombo();
    }

    if (mSelectedNode)
    {
        ImGui::Text("Type: %s", mSelectedNode->nodeTypeString().toStdString().c_str());
        ImGui::Text("Index: %d", mSelectedNode->nodeIndex());
        Node *parent = dynamic_cast<Node *>(mSelectedNode->parent());
        ImGui::Text("Parent: 0x%p", parent);

        if (parent)
            ImGui::Text("Parent Name: %s", parent->name().toStdString().c_str());

        mSelectedNode->drawGui();
    }
}

void NodeManager::populateComboBox(Node *node)
{
    switch (node->nodeType())
    {
    case Node::NodeType::DirectionalLight:
        return;
    default:
        break;
    }

    if (ImGui::Selectable(node->name().toStdString().c_str()))
        mSelectedNode = node;
}
