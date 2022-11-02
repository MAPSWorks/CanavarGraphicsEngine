#ifndef TERRAIN_H
#define TERRAIN_H

#include "Common.h"
#include "Node.h"
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
    Terrain();

public:
    static Terrain *instance();

    void render();
    void reset();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;

    TileGenerator *mTileGenerator;

    QMap<QString, QOpenGLTexture *> mTextures;

    QVector2D mPreviousTilePosition;

    DEFINE_MEMBER(float, Amplitude)
    DEFINE_MEMBER(float, Frequency)
    DEFINE_MEMBER(int, Octaves)
    DEFINE_MEMBER(float, Power)
    DEFINE_MEMBER(QVector3D, Seed)
    DEFINE_MEMBER(float, TessellationMultiplier)

    DEFINE_MEMBER(float, GrassCoverage)
    DEFINE_MEMBER(float, Ambient)
    DEFINE_MEMBER(float, Diffuse)
    DEFINE_MEMBER(float, Specular)
    DEFINE_MEMBER(float, Shininess)
};

} // namespace Engine
} // namespace Canavar

#endif // TERRAIN_H
