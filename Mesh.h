#ifndef MESH_H
#define MESH_H

#include "Common.h"
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

        QVector3D getCenter() const { return (min + max) / 2.0f; }
    };

    explicit Mesh(QObject *parent = nullptr);
    virtual ~Mesh();

    void addVertex(const Vertex &vertex);
    void addIndex(unsigned int index);

    int materialIndex() const;
    void setMaterialIndex(int newMaterialIndex);

    bool create();
    void render(Model *model);
    void render(Primitive primitive);
    void renderForNodeSelector(Model *model);

    const QString &name() const;
    void setName(const QString &newName);

    ModelData *data() const;
    void setData(ModelData *newData);

    const Mesh::AABB &getAABB() const;
    void setAABB(const Mesh::AABB &newAABB);

    unsigned int index() const;
    void setIndex(unsigned int newIndex);

    bool selected() const;
    void setSelected(bool newSelected);

    int selectedVertex() const;
    void setSelectedVertex(int newSelectedVertex);

    void drawGUI();

private:
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    QString mName;
    int mMaterialIndex;
    unsigned int mIndex;

    QOpenGLVertexArrayObject mVertexArray;
    unsigned int mEBO;
    unsigned int mVBO;

    ModelData *mData;
    ShaderManager *mShaderManager;

    AABB mAABB;

    bool mSelected;

    int mSelectedVertex;
};

#endif // MESH_H
