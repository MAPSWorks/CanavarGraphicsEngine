#ifndef MESH_H
#define MESH_H

#include "Model.h"
#include "ShaderManager.h"

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

class ModelData;

class Mesh : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
        QVector3D tangent;
        QVector3D bitangent;
        int boneIDs[4];
        float weights[4];
    };

    struct AABB {
        QVector3D min;
        QVector3D max;
    };

    explicit Mesh(QObject *parent = nullptr);
    virtual ~Mesh();

    void addVertex(const Vertex &vertex);
    void addIndex(unsigned int index);

    int materialIndex() const;
    void setMaterialIndex(int newMaterialIndex);

    bool create();
    void render(Model *model);
    void renderWireframe();
    void renderNormals();

    const QString &name() const;
    void setName(const QString &newName);

    ModelData *data() const;
    void setData(ModelData *newData);

    const AABB &aABB() const;
    void setAABB(const AABB &newAABB);

private:
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    QString mName;
    int mMaterialIndex;

    QOpenGLVertexArrayObject mVertexArray;
    unsigned int mEBO;
    unsigned int mVBO;

    ModelData *mData;
    ShaderManager *mShaderManager;

    AABB mAABB;
};

#endif // MESH_H
