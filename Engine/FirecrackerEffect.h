#ifndef FIRECRACKEREFFECT_H
#define FIRECRACKEREFFECT_H

#include "CameraManager.h"
#include "Node.h"
#include "Selectable.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class FirecrackerEffect : public Node, public Selectable, protected QOpenGLExtraFunctions
{
public:
    explicit FirecrackerEffect(QObject *parent = nullptr);
    ~FirecrackerEffect();

    struct Particle {
        QVector3D position;
        QVector3D velocity;
        float life;
        float deadAfter;
    };

    void create();

public:
    void render(float ifps);

private:
    Particle generateParticle();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;

    QVector<Particle> mParticles;
    int mNumberOfParticles;

    // OpenGL stuff
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int mPBO;

    DECLARE_MEMBER(float, SpanAngle)
    DECLARE_MEMBER(QVector3D, GravityDirection)
    DECLARE_MEMBER(float, Gravity)
    DECLARE_MEMBER(float, MaxLife)
    DECLARE_MEMBER(float, MinLife)
    DECLARE_MEMBER(float, InitialSpeed)
    DECLARE_MEMBER(bool, Loop)
    DECLARE_MEMBER(float, Scale)
};

} // namespace Engine
} // namespace Canavar

#endif // FIRECRACKEREFFECT_H
