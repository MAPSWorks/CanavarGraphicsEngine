#ifndef NODE_H
#define NODE_H

#include "AABB.h"
#include "Common.h"

#include <QJsonObject>
#include <QJsonValue>
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

    Node();
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
        NozzleEffect,
        FirecrackerEffect,
        PersecutorCamera
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

    bool isChildOf(Node *node);
    bool isParentOf(Node *node);

    void addChild(Node *node);
    void removeChild(Node *node);

    Node *findChildByNameRecursive(const QString &name);

    const QList<Node *> &children() const;

    virtual void toJson(QJsonObject &object);
    virtual void fromJson(const QJsonObject &object);

private:
    virtual void updateTransformation();

protected:
    QMatrix4x4 mTransformation;
    QQuaternion mRotation;
    QVector3D mPosition;
    QVector3D mScale;

    Node *mParent;
    QList<Node *> mChildren;

    DEFINE_MEMBER(bool, Visible)
    DEFINE_MEMBER(QString, Name)
    DEFINE_MEMBER(AABB, AABB)
    DEFINE_MEMBER(bool, Selectable)

    DEFINE_MEMBER_CONST(QString, UUID)
    DEFINE_MEMBER_CONST(int, ID)
    DEFINE_MEMBER_CONST(NodeType, Type)
};

} // namespace Engine
} // namespace Canavar
#endif // NODE_H
