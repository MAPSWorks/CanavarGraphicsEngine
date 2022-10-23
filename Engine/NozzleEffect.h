#ifndef NOZZLEEFFECT_H
#define NOZZLEEFFECT_H

#include "CameraManager.h"
#include "Node.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class NozzleEffect : public Node, protected QOpenGLExtraFunctions
{
private:
    friend class NodeManager;
    explicit NozzleEffect(QObject *parent = nullptr);
    virtual ~NozzleEffect();

    struct Particle {
        QVector3D initialPosition;
        QVector3D velocityDirection;
        float life;
    };

public:
    void create();
    void render(float ifps);

    DECLARE_MEMBER(float, Radius)
    DECLARE_MEMBER(float, MaxLife)
    DECLARE_MEMBER(float, Velocity)
    DECLARE_MEMBER(float, Scale)

private:
    Particle generateParticle();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;

    QVector<Particle> mParticles;
    int mNumberOfParticles;

    // OpenGL stuff
    QVector<QVector3D> mVertices;
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int mPBO;
};

} // namespace Engine
} // namespace Canavar
#endif // NOZZLEEFFECT_H
