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
    QMatrix4x4 getParticleTransformation();

    ModelData *modelData() const;
    void setModelData(ModelData *newModelData);

    float maxLife() const;
    void setMaxLife(float newMaxLife);

private slots:
    float getRandomDouble(float bound);
    Particle generateParticle();

private:
    // Blur Effect
    ModelData *mModelData;

    // Particles
    QVector<Particle> mParticles;
    QRandomGenerator mRandomGenerator;
    float mRadius;
    int mNumberOfParticles;
    float mMaxLife;
    quint64 mPreviousTime;
    quint64 mCurrentTime;

    // OpenGL stuff
    QOpenGLVertexArrayObject *mVAO;
    unsigned int mVBO;
    unsigned int mPBO;

    static const float CUBE_VERTICES[108];
};

#endif // NOZZLEEFFECT_H
