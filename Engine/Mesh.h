#ifndef MESH_H
#define MESH_H

#include "AABB.h"
#include "Common.h"
#include "Material.h"

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>

namespace Canavar {
namespace Engine {

class CameraManager;
class ShaderManager;
class LightManager;

class Camera;
class Sun;

class Mesh : public QObject, protected QOpenGLExtraFunctions
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

    Mesh();
    virtual ~Mesh();

    void addVertex(const Vertex &vertex);
    void addIndex(unsigned int index);
    void setMaterial(Material *material);
    void create();
    void render(RenderModes modes, Model *model, GLenum primitive = GL_TRIANGLES);

private:
    QOpenGLVertexArrayObject *mVAO;
    unsigned int mEBO;
    unsigned int mVBO;

    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;
    Material *mMaterial;

    DEFINE_MEMBER(AABB, AABB)
    DEFINE_MEMBER(QString, Name)
    DEFINE_MEMBER(unsigned int, ID)

    // For rendering
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
};

} // namespace Engine
} // namespace Canavar

#endif // MESH_H
