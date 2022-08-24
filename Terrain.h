#ifndef TERRAIN_H
#define TERRAIN_H

#include "CameraManager.h"
#include "Model.h"
#include "ShaderManager.h"
#include "Texture.h"

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctionsPrivate>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

class Terrain : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT

private:
    explicit Terrain(QObject *parent = nullptr);

public:
    struct Properties {
        int numberOfVerticesOnEdge;
        int grids;
        float width;
        float amplitude;
        float frequency;
        int octaves;
        float power;
        QVector3D seed;
        QVector4D clipPlane;
        float tessellationMultiplier;
        float grassCoverage;
    };

    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
    };

    void create(int numberOfVerticesOnEdge = 4, int grids = 128, float width = 1024.0f);
    void render();
    void reset();
    void drawGui();

    QMatrix4x4 transformation() const;

    static Terrain *instance();

    const Properties &properties() const;
    void setProperties(const Properties &newProperties);

    const Model::Material &material() const;
    void setMaterial(const Model::Material &newMaterial);

private:
    QVector2D getCurrentTilePosition() const;
    void updatePositionVectors(const QVector2D &translation);

private:
    Properties mProperties;
    Model::Material mMaterial;
    QVector<QVector2D> mGridPositions;

    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;

    QOpenGLVertexArrayObject *mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
    unsigned int mPBO;
    Texture *mTextureSand;
    Texture *mTextureGrass;
    Texture *mTextureSnow;
    Texture *mTextureRockDiffuse;
    Texture *mTextureRockNormal;
    Texture *mTextureTerrain;

    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    QVector2D mPreviousTilePosition;
};

#endif // TERRAIN_H
