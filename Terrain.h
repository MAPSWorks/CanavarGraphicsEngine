#ifndef TERRAIN_H
#define TERRAIN_H

#include "CameraManager.h"
#include "Model.h"
#include "ShaderManager.h"
#include "Texture.h"
#include "TileGenerator.h"

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
        float amplitude;
        float frequency;
        int octaves;
        float power;
        QVector3D seed;
        float tessellationMultiplier;
        float grassCoverage;
    };

    void create();
    void render();
    void reset();
    void drawGUI();

    QMatrix4x4 transformation() const;

    static Terrain *instance();

    const Properties &properties() const;
    void setProperties(const Properties &newProperties);

    const Model::Material &material() const;
    void setMaterial(const Model::Material &newMaterial);

private:
    TileGenerator *mTileGenerator;
    Properties mProperties;
    Model::Material mMaterial;

    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;

    Texture *mTextureSand;
    Texture *mTextureGrass;
    Texture *mTextureSnow;
    Texture *mTextureRockDiffuse;
    Texture *mTextureRockNormal;
    Texture *mTextureTerrain;

    QVector2D mPreviousTilePosition;
};

#endif // TERRAIN_H
