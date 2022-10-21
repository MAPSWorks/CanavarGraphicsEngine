#ifndef NODE_H
#define NODE_H

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
        Terrain
    };

    const QMatrix4x4 &worldTransformation() const;
    void setWorldTransformation(const QMatrix4x4 &newWorldTransformation);

    const QMatrix4x4 &transformation() const;
    void setTransformation(const QMatrix4x4 &newTransformation);

    const QQuaternion &worldRotation() const;
    void setWorldRotation(const QQuaternion &newWorldRotation);

    const QQuaternion &rotation() const;
    void setRotation(const QQuaternion &newRotation);

    const QVector3D &worldPosition() const;
    void setWorldPosition(const QVector3D &newWorldPosition);

    const QVector3D &position() const;
    void setPosition(const QVector3D &newPosition);

    const QVector3D &scale() const;
    void setScale(const QVector3D &newScale);

    const QString &name() const;
    void setName(const QString &newName);

    bool visible() const;
    void setVisible(bool newVisible);

    bool renderable() const;
    void setRenderable(bool newRenderable);

    Node *parent() const;
    void setParent(Node *newParent);

    void addChild(Node *node);
    void removeChild(Node *node);

    NodeType type() const;

private:
    virtual void updateTransformation();

protected:
    QMatrix4x4 mWorldTransformation;
    QMatrix4x4 mTransformation;
    QQuaternion mWorldRotation;
    QQuaternion mRotation;
    QVector3D mWorldPosition;
    QVector3D mPosition;
    QVector3D mScale;
    QString mName;
    NodeType mType;

    bool mVisible;
    bool mRenderable;

    Node *mParent;
    QList<Node *> mChildren;
};

} // namespace Engine
} // namespace Canavar
#endif // NODE_H
