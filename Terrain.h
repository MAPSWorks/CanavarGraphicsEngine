#ifndef TERRAIN_H
#define TERRAIN_H

#include "Material.h"
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

    void create();
    void render();

    QMatrix4x4 transformation() const;

    const Properties &properties() const;
    void setProperties(const Properties &newProperties);

    const Material &material() const;
    void setMaterial(const Material &newMaterial);

    void reset();

    static Terrain *instance();

private:
    Properties mProperties;
    QVector<QVector2D> mPositions;
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
    ShaderManager *mShaderManager;
    Material mMaterial;

    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;
};

#endif // TERRAIN_H
