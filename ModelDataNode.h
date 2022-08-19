#ifndef MODELDATANODE_H
#define MODELDATANODE_H

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

    const QMatrix4x4 &initialTransformation() const;
    void setInitialTransformation(const QMatrix4x4 &newInitialTransformation);

    void render();

private:
    QVector<ModelDataNode *> mChildren;
    QVector<int> mMeshIndices;
    QString mName;
    QMatrix4x4 mInitialTransformation;
    ModelData *mData;
};

#endif // MODELDATANODE_H
