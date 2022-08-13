#ifndef MESH_H
#define MESH_H

#include "ShaderManager.h"

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

class TexturedModelData;

class Mesh : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D textureCoord;
        QVector3D tangent;
        QVector3D bitangent;
        int boneIDs[4];
        float weights[4];
    };

    explicit Mesh(QObject *parent = nullptr);
    virtual ~Mesh();

    void addVertex(const Vertex &vertex);
    void addIndex(unsigned int index);

    int materialIndex() const;
    void setMaterialIndex(int newMaterialIndex);

    bool create();
    void render();

    const QString &name() const;
    void setName(const QString &newName);

    TexturedModelData *data() const;
    void setData(TexturedModelData *newData);

private:
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    QString mName;
    int mMaterialIndex;

    QOpenGLVertexArrayObject mVertexArray;
    unsigned int mEBO;
    unsigned int mVBO;

    TexturedModelData *mData;
    ShaderManager *mShaderManager;
};

#endif // MESH_H
