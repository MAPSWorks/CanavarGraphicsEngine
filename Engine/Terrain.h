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

    DECLARE_MEMBER(float, Amplitude)
    DECLARE_MEMBER(float, Frequency)
    DECLARE_MEMBER(int, Octaves)
    DECLARE_MEMBER(float, Power)
    DECLARE_MEMBER(QVector3D, Seed)
    DECLARE_MEMBER(float, TessellationMultiplier)

    DECLARE_MEMBER(float, GrassCoverage)
    DECLARE_MEMBER(float, Ambient)
    DECLARE_MEMBER(float, Diffuse)
    DECLARE_MEMBER(float, Specular)
    DECLARE_MEMBER(float, Shininess)
};

} // namespace Engine
} // namespace Canavar

#endif // TERRAIN_H
