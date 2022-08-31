#ifndef MESH_H
#define MESH_H

#include "Common.h"
#include "DirectionalLight.h"
#include "Model.h"
#include "PerspectiveCamera.h"
#include "PointLight.h"
#include "SpotLight.h"

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

class ModelData;
class ShaderManager;
class CameraManager;
class LightManager;
class Haze;
class Water;

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
    void render(Model *model, const RenderSettings &settings);

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
    void pointLights();
    void spotLights();
    void materials();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    PerspectiveCamera *mCamera;
    DirectionalLight *mSun;
    Haze *mHaze;
    Water *mWater;
    ModelData *mData;

    QString mName;
    int mMaterialIndex;
    unsigned int mIndex;

    AABB mAABB;
    bool mSelected;
    int mSelectedVertex;

    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    QOpenGLVertexArrayObject *mVAO;
    unsigned int mEBO;
    unsigned int mVBO;

    QVector<PointLight *> mClosePointLights;
    QVector<SpotLight *> mCloseSpotLights;

    // Vertex rendering for NodeSelector
    QOpenGLVertexArrayObject *mVerticesVAO;
    unsigned int mVerticesVBO;
    unsigned int mVerticesPBO;
    static const float CUBE_VERTICES[108];
    QMatrix4x4 mVertexModelTransformation;
};

#endif // MESH_H
