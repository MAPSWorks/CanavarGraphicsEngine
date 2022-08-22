#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QObject>
#include <QQuaternion>
#include <QVector3D>

#include <imgui.h>
#include <QtImGui.h>

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
        DirectionalLight,
        PointLight,
        SpotLight,
        NozzleEffect
    };

    virtual QMatrix4x4 transformation() const;
    virtual QMatrix4x4 worldTransformation() const;

    virtual const QVector3D &position() const;
    virtual void setPosition(const QVector3D &newPosition);

    virtual QVector3D worldPosition() const;
    virtual void setWorldPosition(const QVector3D &newWorldPosition);

    virtual const QQuaternion &rotation() const;
    virtual void setRotation(const QQuaternion &newRotation);

    virtual QQuaternion worldRotation() const;
    virtual void setWorldRotation(const QQuaternion &newWorldRotation);

    virtual const QVector3D &scale() const;
    virtual void setScale(const QVector3D &newScale);

    const QString &name() const;
    void setName(const QString &newName);

    virtual void addChild(Node *child);
    virtual void removeChild(Node *child);

    const QList<Node *> &children() const;

    NodeType nodeType() const;
    QString nodeTypeString();

    bool visible() const;
    void setVisible(bool newVisible);

    bool renderable() const;

    virtual QMatrix4x4 normalMatrix() const;

    virtual void drawGui();

protected:
    QVector3D mPosition;
    QQuaternion mRotation;
    QVector3D mScale;
    QString mName;
    NodeType mNodeType;
    QList<Node *> mChildren;
    bool mVisible;
    bool mRenderable;
};

#endif // NODE_H
