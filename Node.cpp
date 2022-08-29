#include "Node.h"
#include "Helper.h"

#include <QtMath>

Node::Node(QObject *parent)
    : QObject{parent}
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)
    , mNodeType(NodeType::DummyNode)
    , mVisible(true)
    , mRenderable(true)
    , mNodeIndex(0)
    , mSelected(false)

{}

Node::~Node() {}

const QVector3D &Node::position() const
{
    return mPosition;
}

void Node::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;
}

QVector3D Node::worldPosition() const
{
    Node *parent = dynamic_cast<Node *>(this->parent());

    if (parent)
        return parent->worldPosition() + mPosition;
    else
        return mPosition;
}

void Node::setWorldPosition(const QVector3D &newWorldPosition)
{
    Node *parent = dynamic_cast<Node *>(this->parent());

    if (parent)
    {
        QVector3D parentWorldPosition = parent->worldPosition();
        mPosition = newWorldPosition - parentWorldPosition;
    } else
    {
        mPosition = newWorldPosition;
    }
}

const QQuaternion &Node::rotation() const
{
    return mRotation;
}

void Node::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation;
}

QQuaternion Node::worldRotation() const
{
    Node *parent = dynamic_cast<Node *>(this->parent());

    if (parent)
        return parent->worldRotation() * mRotation;
    else
        return mRotation;
}

void Node::setWorldRotation(const QQuaternion &newWorldRotation)
{
    Node *parent = dynamic_cast<Node *>(this->parent());

    if (parent)
    {
        QQuaternion parentWorldRotation = parent->worldRotation();
        mRotation = parentWorldRotation.inverted() * newWorldRotation;
    } else
    {
        mRotation = newWorldRotation;
    }
}

const QVector3D &Node::scale() const
{
    return mScale;
}

void Node::setScale(const QVector3D &newScale)
{
    mScale = newScale;
}

QMatrix4x4 Node::transformation() const
{
    QMatrix4x4 transformation;
    transformation.scale(mScale);
    transformation.rotate(mRotation);
    transformation.setColumn(3, QVector4D(mPosition, 1.0f));

    return transformation;
}

QMatrix4x4 Node::worldTransformation() const
{
    Node *parent = dynamic_cast<Node *>(this->parent());

    if (parent)
    {
        QMatrix4x4 parentWorldTransformation = parent->worldTransformation();
        float x = parent->scale().x();
        float y = parent->scale().y();
        float z = parent->scale().z();

        parentWorldTransformation.scale(1 / x, 1 / y, 1 / z);

        return parentWorldTransformation * transformation();
    } else
    {
        return transformation();
    }
}

const QString &Node::name() const
{
    return mName;
}

void Node::setName(const QString &newName)
{
    mName = newName;
}

void Node::addChild(Node *child)
{
    child->setParent(this);
    mChildren << child;
}

void Node::removeChild(Node *child)
{
    mChildren.removeAll(child);
    child->setParent(nullptr);
}

const QList<Node *> &Node::children() const
{
    return mChildren;
}

Node::NodeType Node::nodeType() const
{
    return mNodeType;
}

QString Node::nodeTypeString()
{
    switch (mNodeType)
    {
    case NodeType::DummyNode:
        return "DummyNode";
    case NodeType::Model:
        return "Model";
    case NodeType::FreeCamera:
        return "Free Camera";
    case NodeType::DummyCamera:
        return "Dummy Camera";
    case NodeType::OrthographicCamera:
        return "Orthographic Camera";
    case NodeType::DirectionalLight:
        return "Directional Light";
    case NodeType::PointLight:
        return "Point Light";
    case NodeType::SpotLight:
        return "Spot Light";
    case NodeType::NozzleParticles:
        return "Nozzle Particles";
    default:
        return "Unknown Node";
    }
}

bool Node::visible() const
{
    return mVisible;
}

void Node::setVisible(bool newVisible)
{
    mVisible = newVisible;
}

bool Node::renderable() const
{
    return mRenderable;
}

QMatrix3x3 Node::normalMatrix() const
{
    return worldTransformation().normalMatrix();
}

void Node::drawGUI()
{
    // Position
    if (!ImGui::CollapsingHeader("Position##Node"))
    {
        float x = mPosition.x();
        float y = mPosition.y();
        float z = mPosition.z();

        if (ImGui::DragFloat("x##NodePosition", &x, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            mPosition = QVector3D(x, y, z);
        if (ImGui::DragFloat("y##NodePosition", &y, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            mPosition = QVector3D(x, y, z);
        if (ImGui::DragFloat("z##NodePosition", &z, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            mPosition = QVector3D(x, y, z);
    }

    // Rotation
    if (ImGui::CollapsingHeader("Rotation##Node"))
    {
        float yaw, pitch, roll;

        Helper::getEulerDegrees(mRotation, yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 359.999f, "%.3f"))
            mRotation = Helper::constructFromEulerDegrees(yaw, pitch, roll);
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            mRotation = Helper::constructFromEulerDegrees(yaw, pitch, roll);
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -179.999f, 179.999f, "%.3f"))
            mRotation = Helper::constructFromEulerDegrees(yaw, pitch, roll);
    }

    // World Position
    if (ImGui::CollapsingHeader("World Position##Node"))
    {
        QVector3D position = worldPosition();
        float x = position.x();
        float y = position.y();
        float z = position.z();

        if (ImGui::DragFloat("x##NodeWorldPosition", &x, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeWorldPosition", &y, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeWorldPosition", &z, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            setWorldPosition(QVector3D(x, y, z));
    }

    // World rotation
    if (ImGui::CollapsingHeader("World Rotation##Node"))
    {
        QQuaternion rotation = worldRotation();
        float yaw, pitch, roll;

        Helper::getEulerDegrees(rotation, yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 359.999f, "%.3f"))
            setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -179.999f, 179.999f, "%.3f"))
            setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
    }

    // Scale
    if (ImGui::CollapsingHeader("Scale##Node"))
    {
        QVector3D scale = mScale;
        float x = scale.x();
        float y = scale.y();
        float z = scale.z();
        float all = qMin(x, qMin(y, z));

        if (ImGui::DragFloat("All##NodeScale", &all, 0.01f, 0.0001f, 100.0f, "%.3f"))
            mScale = QVector3D(all, all, all);
        if (ImGui::DragFloat("x##NodeScale", &x, 0.01f, 0.0001f, 100.0f, "%.3f"))
            mScale = QVector3D(x, y, z);
        if (ImGui::DragFloat("y##NodeScale", &y, 0.01f, 0.0001f, 100.0f, "%.3f"))
            mScale = QVector3D(x, y, z);
        if (ImGui::DragFloat("z##NodeScale", &z, 0.01f, 0.0001f, 100.0f, "%.3f"))
            mScale = QVector3D(x, y, z);
    }
}

unsigned int Node::nodeIndex() const
{
    return mNodeIndex;
}

void Node::setNodeIndex(unsigned int newNodeIndex)
{
    mNodeIndex = newNodeIndex;
}

bool Node::selected() const
{
    return mSelected;
}

void Node::setSelected(bool newSelected)
{
    mSelected = newSelected;
}
