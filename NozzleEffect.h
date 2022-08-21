#ifndef NOZZLEEFFECT_H
#define NOZZLEEFFECT_H

#include "ModelData.h"
#include "Node.h"

#include <QOpenGLExtraFunctions>
#include <QRandomGenerator>

class NozzleEffect : public Node, protected QOpenGLExtraFunctions
{
public:
    struct Particle {
        QVector3D initialPosition;
        QVector3D velocity;
        float life;
    };

    explicit NozzleEffect(QObject *parent = nullptr);
    virtual ~NozzleEffect();

    void create();
    void renderParticles();
    void renderBlurEffect();

    QMatrix4x4 getParticlesWorldTransformation();

    ModelData *modelData() const;
    void setModelData(ModelData *newModelData);

    float maxLife() const;
    void setMaxLife(float newMaxLife);

    const QVector3D &particlesPosition() const;
    void setParticlesPosition(const QVector3D &newParticlesPosition);

    const QQuaternion &particlesRotation() const;
    void setParticlesRotation(const QQuaternion &newParticlesRotation);

    float minVelocity() const;
    void setMinVelocity(float newMinVelocity);

    float maxVelocity() const;
    void setMaxVelocity(float newMaxVelocity);

    float radius() const;
    void setRadius(float newRadius);

private:
    float getRandomDouble(float bound);
    NozzleEffect::Particle generateParticle();

private:
    // Blur Effect
    ModelData *mModelData;

    // Particles
    QVector<Particle> mParticles;
    QRandomGenerator mRandomGenerator;
    int mNumberOfParticles;

    float mRadius;
    float mMaxLife;
    float mMinVelocity;
    float mMaxVelocity;

    quint64 mPreviousTime;
    quint64 mCurrentTime;

    QVector3D mParticlesPosition;
    QQuaternion mParticlesRotation;

    // OpenGL stuff
    QOpenGLVertexArrayObject *mVAO;
    unsigned int mVBO;
    unsigned int mPBO;

    static const float CUBE_VERTICES[108];
};

#endif // NOZZLEEFFECT_H
