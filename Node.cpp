#include "Node.h"

Node::Node(QObject *parent)
    : QObject{parent}
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)
    , mNodeType(NodeType::DummyNode)
    , mVisible(true)

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
