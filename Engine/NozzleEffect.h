#ifndef NOZZLEEFFECT_H
#define NOZZLEEFFECT_H

#include "CameraManager.h"
#include "Node.h"
#include "Selectable.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class NozzleEffect : public Node, public Selectable, protected QOpenGLExtraFunctions
{
protected:
    friend class NodeManager;
    explicit NozzleEffect(QObject *parent = nullptr);
    ~NozzleEffect();

    struct Particle {
        QVector3D initialPosition;
        QVector3D direction;
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

    DECLARE_MEMBER(float, MaxRadius)
    DECLARE_MEMBER(float, MaxLife)
    DECLARE_MEMBER(float, MinLife)
    DECLARE_MEMBER(float, MaxDistance)
    DECLARE_MEMBER(float, MinDistance)
    DECLARE_MEMBER(float, Velocity)
    DECLARE_MEMBER(float, Scale)
};

} // namespace Engine
} // namespace Canavar

#endif // NOZZLEEFFECT_H
