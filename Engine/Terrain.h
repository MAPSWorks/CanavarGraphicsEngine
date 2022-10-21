#ifndef TERRAIN_H
#define TERRAIN_H

#include "Camera.h"
#include "Common.h"
#include "Sun.h"
#include "TileGenerator.h"

#include <QObject>
#include <QOpenGLExtraFunctionsPrivate>
#include <QOpenGLTexture>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

namespace Canavar {
namespace Engine {

class ShaderManager;
class CameraManager;
class LightManager;
class Haze;

class Terrain : public Node, protected QOpenGLExtraFunctions
{
    Q_OBJECT
private:
    explicit Terrain(QObject *parent = nullptr);

public:
    static Terrain *instance();

    void render(const RenderParameters &parameters);
    void reset();

    float mAmplitude;
    float mFrequency;
    int mOctaves;
    float mPower;
    QVector3D mSeed;
    float mTessellationMultiplier;
    float mGrassCoverage;

    float mAmbient;
    float mDiffuse;
    float mSpecular;
    float mShininess;

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    Haze *mHaze;
    TileGenerator *mTileGenerator;
    Camera *mCamera;
    Sun *mSun;

    QMap<QString, QOpenGLTexture *> mTextures;

    QVector2D mPreviousTilePosition;
};

} // namespace Engine
} // namespace Canavar

#endif // TERRAIN_H
