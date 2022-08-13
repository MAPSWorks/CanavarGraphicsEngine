#ifndef TEXTUREDMODELDATA_H
#define TEXTUREDMODELDATA_H

#include "Mesh.h"
#include "TextureMaterial.h"
#include "TexturedModelDataNode.h"

#include <QObject>

class TexturedModelData : public QObject
{
    Q_OBJECT
public:
    explicit TexturedModelData(QString path, QObject *parent = nullptr);

    void addMesh(Mesh *mesh);
    void addTextureMaterial(TextureMaterial *material);

    const QVector<Mesh *> &meshes() const;
    const QVector<TextureMaterial *> &materials() const;

    const QString &name() const;
    void setName(const QString &newName);

    TexturedModelDataNode *rootNode() const;
    void setRootNode(TexturedModelDataNode *newRootNode);

    const QString &path() const;

private:
    QString mPath;
    QString mName;

    TexturedModelDataNode *mRootNode;
    QVector<Mesh *> mMeshes;
    QVector<TextureMaterial *> mMaterials;
};

#endif // TEXTUREDMODELDATA_H
