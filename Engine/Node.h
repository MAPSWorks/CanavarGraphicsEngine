#ifndef NODE_H
#define NODE_H

#include "Common.h"

#include <QMatrix4x4>
#include <QObject>
#include <QQuaternion>
#include <QVector3D>

namespace Canavar {
namespace Engine {

class Node : public QObject
{
    Q_OBJECT
protected:
    friend class NodeManager;

    explicit Node(QObject *parent = nullptr);
    virtual ~Node();

public:
    enum class NodeType { //
        DummyNode,
        Model,
        FreeCamera,
        DummyCamera,
        Sun,
        Sky,
        Haze,
        Terrain,
        PointLight,
        NozzleEffect
    };

    const QMatrix4x4 worldTransformation() const;
    void setWorldTransformation(const QMatrix4x4 &newTransformation);

    const QMatrix4x4 &transformation() const;
    void setTransformation(const QMatrix4x4 &newTransformation);

    QQuaternion worldRotation() const;
    void setWorldRotation(const QQuaternion &newWorldRotation);

    const QQuaternion &rotation() const;
    void setRotation(const QQuaternion &newRotation);

    QVector3D worldPosition() const;
    void setWorldPosition(const QVector3D &newWorldPosition);

    const QVector3D &position() const;
    void setPosition(const QVector3D &newPosition);

    const QVector3D &scale() const;
    void setScale(const QVector3D &newScale);

    Node *parent() const;
    void setParent(Node *newParent);

    void addChild(Node *node);
    void removeChild(Node *node);

    NodeType type() const;

private:
    virtual void updateTransformation();

protected:
    QMatrix4x4 mTransformation;
    QQuaternion mRotation;
    QVector3D mPosition;
    QVector3D mScale;
    NodeType mType;

    Node *mParent;
    QList<Node *> mChildren;

    DECLARE_MEMBER(bool, Visible)
    DECLARE_MEMBER(bool, Renderable)
    DECLARE_MEMBER(QString, Name)
};

} // namespace Engine
} // namespace Canavar
#endif // NODE_H
