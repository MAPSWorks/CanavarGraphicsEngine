#ifndef MODELDATA_H
#define MODELDATA_H

#include "Mesh.h"
#include "ModelDataNode.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class ModelData : public QObject
{
    Q_OBJECT
public:
    explicit ModelData(const QString &name, QObject *parent = nullptr);
    virtual ~ModelData();

    void addMesh(Mesh *mesh);
    void addMaterial(Material *material);
    void setRootNode(ModelDataNode *newRootNode);

    Material *getMaterial(int index);

    const QString &name() const;
    const QVector<Mesh *> &meshes() const;

    void render(RenderPass renderPass, Node *node);

private:
    QString mName;
    QVector<Mesh *> mMeshes;
    QVector<Material *> mMaterials;
    ModelDataNode *mRootNode;
};

} // namespace Engine
} // namespace Canavar
#endif // MODELDATA_H
