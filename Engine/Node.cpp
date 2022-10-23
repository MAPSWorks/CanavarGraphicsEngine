#include "Node.h"

#include <QtMath>

Canavar::Engine::Node::Node(QObject *parent)
    : QObject(parent)
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)
    , mType(NodeType::DummyNode)
    , mParent(dynamic_cast<Node *>(parent))
    , mVisible(true)
    , mRenderable(true)
{}

Canavar::Engine::Node::~Node() {}

Canavar::Engine::Node::NodeType Canavar::Engine::Node::type() const
{
    return mType;
}

Canavar::Engine::Node *Canavar::Engine::Node::findChildByNameRecursive(const QString &name)
{
    for (const auto &child : mChildren)
        if (child->getName() == name)
            return child;

    for (const auto &child : mChildren)
        if (Node *node = child->findChildByNameRecursive(name))
            return node;

    return nullptr;
}

const QMatrix4x4 Canavar::Engine::Node::worldTransformation() const
{
    // TODO: Scaling issue

    if (mParent)
        return mParent->worldTransformation() * mTransformation;
    else
        return mTransformation;
}

void Canavar::Engine::Node::setWorldTransformation(const QMatrix4x4 &newTransformation)
{
    setWorldPosition(mTransformation.column(3).toVector3D());
    setWorldRotation(QQuaternion::fromRotationMatrix(mTransformation.normalMatrix()));
}

QQuaternion Canavar::Engine::Node::worldRotation() const
{
    if (mParent)
        return mParent->worldRotation() * mRotation;
    else
        return mRotation;
}

void Canavar::Engine::Node::setWorldRotation(const QQuaternion &newWorldRotation)
{
    if (mParent)
        mRotation = mParent->worldRotation().inverted() * newWorldRotation;
    else
        mRotation = newWorldRotation;

    updateTransformation();
}

const QQuaternion &Canavar::Engine::Node::rotation() const
{
    return mRotation;
}

void Canavar::Engine::Node::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation;

    updateTransformation();
}

QVector3D Canavar::Engine::Node::worldPosition() const
{
    if (mParent)
        return mParent->worldPosition() + mParent->worldRotation() * mPosition;
    else
        return mPosition;
}

void Canavar::Engine::Node::setWorldPosition(const QVector3D &newWorldPosition)
{
    if (mParent)
        mPosition = mParent->worldRotation().inverted() * (newWorldPosition - mParent->worldPosition());
    else
        mPosition = newWorldPosition;

    updateTransformation();
}

const QVector3D &Canavar::Engine::Node::position() const
{
    return mPosition;
}

void Canavar::Engine::Node::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;

    updateTransformation();
}

const QVector3D &Canavar::Engine::Node::scale() const
{
    return mScale;
}

void Canavar::Engine::Node::setScale(const QVector3D &newScale)
{
    mScale = newScale;

    updateTransformation();
}

const QMatrix4x4 &Canavar::Engine::Node::transformation() const
{
    return mTransformation;
}

void Canavar::Engine::Node::setTransformation(const QMatrix4x4 &newTransformation)
{
    mTransformation = newTransformation;

    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
}

void Canavar::Engine::Node::updateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));
}

Canavar::Engine::Node *Canavar::Engine::Node::parent() const
{
    return mParent;
}

void Canavar::Engine::Node::setParent(Node *newParent)
{
    // TODO: Do we need to update transformation?
    mParent = newParent;
}

void Canavar::Engine::Node::addChild(Node *node)
{
    if (node)
    {
        node->setParent(this);
        mChildren << node;
    }
}

void Canavar::Engine::Node::removeChild(Node *node)
{
    if (node)
    {
        node->deleteLater();
        mChildren.removeAll(node);
    }
}
