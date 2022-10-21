#ifndef SKY_H
#define SKY_H

#include "CameraManager.h"
#include "LightManager.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class Sky : public Node, protected QOpenGLExtraFunctions
{
private:
    explicit Sky(QObject *parent = nullptr);

public:
    static Sky *instance();

    void render();

    bool enabled() const;
    void setEnabled(bool newEnabled);

public:
    float mAlbedo;
    float mTurbidity;
    float mNormalizedSunY;

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;

    // OpenGL Stuff
    QVector<unsigned int> mVertices;
    unsigned int mVAO;
    unsigned int mVBO;

    // Model
    QVector3D A, B, C, D, E, F, G, H, I;
    QVector3D Z;

    bool mEnabled;
};

} // namespace Engine
} // namespace Canavar

#endif // SKY_H
