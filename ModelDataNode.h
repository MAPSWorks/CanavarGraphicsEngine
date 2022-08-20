#ifndef MODELDATANODE_H
#define MODELDATANODE_H

#include "Model.h"

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLShaderProgram>

class ModelData;

class ModelDataNode : public QObject
{
    Q_OBJECT
public:
    explicit ModelDataNode(ModelData *data, QObject *parent = nullptr);

    void addChild(ModelDataNode *child);
    void addMeshIndex(int index);

    const QVector<ModelDataNode *> &children() const;

    const QString &name() const;
    void setName(const QString &newName);

    const QMatrix4x4 &transformation() const;
    void setTransformation(const QMatrix4x4 &newTransformation);

    void render(Model *model);
    QMatrix4x4 getWorldTransformation() const;
    ModelDataNode *getChildNode(const QString &meshName);

private slots:
    ModelDataNode *getChildNode(const QString &nodeName, ModelDataNode *node);

private:
    QVector<ModelDataNode *> mChildren;
    QVector<int> mMeshIndices;
    QString mName;
    QMatrix4x4 mTransformation;
    ModelData *mData;
};

#endif // MODELDATANODE_H
