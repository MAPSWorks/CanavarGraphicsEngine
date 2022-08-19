#ifndef MODELDATA_H
#define MODELDATA_H

#include "Mesh.h"
#include "ModelDataNode.h"
#include "TextureMaterial.h"

#include <QObject>

class ModelData : public QObject
{
    Q_OBJECT
public:
    explicit ModelData(const QString &modelName, QObject *parent = nullptr);

    void addMesh(Mesh *mesh);
    void addMaterial(TextureMaterial *material);

    ModelDataNode *rootNode() const;
    void setRootNode(ModelDataNode *newRootNode);

    const QVector<Mesh *> &meshes() const;
    const QString &modelName() const;
    const QVector<TextureMaterial *> &materials() const;

    void render();

private:
    QString mModelName;
    ModelDataNode *mRootNode;
    QVector<Mesh *> mMeshes;
    QVector<TextureMaterial *> mMaterials;
};

#endif // MODELDATA_H
