#ifndef NOZZLEEFFECT_H
#define NOZZLEEFFECT_H

#include "Node.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QRandomGenerator>

class NozzleEffect : public Node, protected QOpenGLExtraFunctions
{
public:
    struct Particle {
        QVector3D initialPosition;
        QVector3D velocityDirection;
        float life;
    };

    explicit NozzleEffect(QObject *parent = nullptr);
    virtual ~NozzleEffect();

    void create();
    void renderParticles(float ifps);

    float radius() const;
    float velocity() const;
    float maxLife() const;

    virtual void drawGui() override;

private:
    float getRandomFloat(float bound);
    NozzleEffect::Particle generateParticle();

private:
    QVector<Particle> mParticles;
    QRandomGenerator mRandomGenerator;
    int mNumberOfParticles;

    float mRadius;
    float mMaxLife;
    float mVelocity;

    // OpenGL stuff
    QVector<QVector3D> mVertices;
    QOpenGLVertexArrayObject *mVAO;
    unsigned int mVBO;
    unsigned int mPBO;

    static const float CUBE_VERTICES[108];
};

#endif // NOZZLEEFFECT_H
