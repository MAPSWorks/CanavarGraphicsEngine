#ifndef TERRAIN_H
#define TERRAIN_H

#include "Common.h"
#include "DirectionalLight.h"
#include "Model.h"
#include "PerspectiveCamera.h"
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

class ShaderManager;
class CameraManager;
class LightManager;
class Haze;
class Water;

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

    void render(const RenderSettings &settings);
    void reset();
    void drawGUI();

    static Terrain *instance();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    Haze *mHaze;
    Water *mWater;
    TileGenerator *mTileGenerator;

    DirectionalLight *mSun;
    PerspectiveCamera *mCamera;

    Properties mProperties;
    Model::Material mMaterial;

    Texture *mTextureSand;
    Texture *mTextureGrass;
    Texture *mTextureSnow;
    Texture *mTextureRockDiffuse;
    Texture *mTextureRockNormal;
    Texture *mTextureTerrain;

    QVector2D mPreviousTilePosition;

    QMatrix4x4 mTransformation;
};

#endif // TERRAIN_H
