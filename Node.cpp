#include "Node.h"

Node::Node(QObject *parent)
    : QObject{parent}
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)
    , mNodeType(NodeType::DummyNode)
    , mVisible(true)
    , mRenderable(true)

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
    case NodeType::DirectionalLight:
        return "Directional Light";
    case NodeType::PointLight:
        return "Point Light";
    case NodeType::SpotLight:
        return "Spot Light";
    case NodeType::NozzleEffect:
        return "Nozzle Effect";
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

void Node::drawGui()
{
    // Position
    if (!ImGui::CollapsingHeader("Position##NodePosition"))
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
    if (ImGui::CollapsingHeader("Rotation##NodeRotation"))
    {
        float w = mRotation.scalar();
        float x = mRotation.x();
        float y = mRotation.y();
        float z = mRotation.z();

        if (ImGui::DragFloat("w##NodeRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("x##NodeRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("y##NodeRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("z##NodeRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
    }

    // World Position
    if (ImGui::CollapsingHeader("World Position##NodeRotation"))
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
    if (ImGui::CollapsingHeader("World Rotation##NodeRotation"))
    {
        QQuaternion rotation = worldRotation();
        float w = rotation.scalar();
        float x = rotation.x();
        float y = rotation.y();
        float z = rotation.z();

        if (ImGui::DragFloat("w##NodeWorldRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
            setWorldRotation(QQuaternion(w, x, y, z).normalized());
        if (ImGui::DragFloat("x##NodeWorldRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            setWorldRotation(QQuaternion(w, x, y, z).normalized());
        if (ImGui::DragFloat("y##NodeWorldRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
            setWorldRotation(QQuaternion(w, x, y, z).normalized());
        if (ImGui::DragFloat("z##NodeWorldRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            setWorldRotation(QQuaternion(w, x, y, z).normalized());
    }

    // Scale
    if (ImGui::CollapsingHeader("Scale##NodeRotation"))
    {
        QVector3D scale = mScale;
        float x = scale.x();
        float y = scale.y();
        float z = scale.z();

        if (ImGui::DragFloat("x##NodeScale", &x, 0.01f, 0.0001f, 100.0f, "%.3f"))
            mScale = QVector3D(x, y, z);
        if (ImGui::DragFloat("y##NodeScale", &y, 0.01f, 0.0001f, 100.0f, "%.3f"))
            mScale = QVector3D(x, y, z);
        if (ImGui::DragFloat("z##NodeScale", &z, 0.01f, 0.0001f, 100.0f, "%.3f"))
            mScale = QVector3D(x, y, z);
    }
}
