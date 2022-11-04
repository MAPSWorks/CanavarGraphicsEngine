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
    Sky();

public:
    static Sky *instance();

    virtual void toJson(QJsonObject &object) override;
    virtual void fromJson(const QJsonObject &object) override;

    void render();

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

    DEFINE_MEMBER(bool, Enabled)
    DEFINE_MEMBER(float, Albedo)
    DEFINE_MEMBER(float, Turbidity)
    DEFINE_MEMBER(float, NormalizedSunY)
};

} // namespace Engine
} // namespace Canavar

#endif // SKY_H
