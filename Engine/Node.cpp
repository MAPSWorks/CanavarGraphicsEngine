#include "Node.h"

#include <QtMath>

Canavar::Engine::Node::Node(QObject *parent)
    : QObject(parent)
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)
    , mType(NodeType::DummyNode)
    , mVisible(true)
    , mRenderable(false)
    , mParent(dynamic_cast<Node *>(parent))
{}

Canavar::Engine::Node::~Node() {}

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

const QQuaternion &Canavar::Engine::Node::rotation() const
{
    return mRotation;
}

void Canavar::Engine::Node::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation;

    if (mParent)
        mWorldRotation = mParent->worldRotation() * mRotation;
    else
        mWorldRotation = mRotation;

    updateTransformation();
}

const QVector3D &Canavar::Engine::Node::position() const
{
    return mPosition;
}

void Canavar::Engine::Node::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;

    if (mParent)
        mWorldPosition = mParent->worldPosition() + mParent->worldRotation() * mPosition;
    else
        mWorldPosition = mPosition;

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

void Canavar::Engine::Node::updateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));

    mWorldTransformation.setToIdentity();
    mWorldTransformation.scale(mScale);
    mWorldTransformation.rotate(mWorldRotation);
    mWorldTransformation.setColumn(3, QVector4D(mWorldPosition, 1.0f));
}

Canavar::Engine::Node::NodeType Canavar::Engine::Node::type() const
{
    return mType;
}

const QVector3D &Canavar::Engine::Node::worldPosition() const
{
    return mWorldPosition;
}

void Canavar::Engine::Node::setWorldPosition(const QVector3D &newWorldPosition)
{
    mWorldPosition = newWorldPosition;

    if (mParent)
        mPosition = mParent->worldRotation().inverted() * (newWorldPosition - mParent->worldPosition());
    else
        mPosition = newWorldPosition;

    updateTransformation();
}

const QQuaternion &Canavar::Engine::Node::worldRotation() const
{
    return mWorldRotation;
}

void Canavar::Engine::Node::setWorldRotation(const QQuaternion &newWorldRotation)
{
    mWorldRotation = newWorldRotation;

    if (mParent)
        mRotation = mParent->worldRotation().inverted() * newWorldRotation;
    else
        mRotation = newWorldRotation;
}

const QMatrix4x4 &Canavar::Engine::Node::worldTransformation() const
{
    return mWorldTransformation;
}

void Canavar::Engine::Node::setWorldTransformation(const QMatrix4x4 &newWorldTransformation)
{
    mWorldTransformation = newWorldTransformation;

    setWorldPosition(mWorldTransformation.column(3).toVector3D());
    setWorldRotation(QQuaternion::fromRotationMatrix(mWorldTransformation.normalMatrix()));
}

Canavar::Engine::Node *Canavar::Engine::Node::parent() const
{
    return mParent;
}

void Canavar::Engine::Node::setParent(Node *newParent)
{
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

bool Canavar::Engine::Node::renderable() const
{
    return mRenderable;
}

void Canavar::Engine::Node::setRenderable(bool newRenderable)
{
    mRenderable = newRenderable;
}

bool Canavar::Engine::Node::visible() const
{
    return mVisible;
}

void Canavar::Engine::Node::setVisible(bool newVisible)
{
    mVisible = newVisible;
}

const QString &Canavar::Engine::Node::name() const
{
    return mName;
}

void Canavar::Engine::Node::setName(const QString &newName)
{
    mName = newName;
}
