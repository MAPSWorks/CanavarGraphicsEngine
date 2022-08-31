#ifndef NOZZLEPARTICLES_H
#define NOZZLEPARTICLES_H

#include "CameraManager.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QRandomGenerator>

class NozzleParticles : public Node, protected QOpenGLExtraFunctions
{
public:
    struct Particle {
        QVector3D initialPosition;
        QVector3D velocityDirection;
        float life;
    };

private:
    friend class NodeManager;
    explicit NozzleParticles(Node *parent = nullptr);
    virtual ~NozzleParticles();

public:
    virtual void create();
    virtual void render(const RenderSettings &settings);
    virtual void drawGUI() override;

private:
    NozzleParticles::Particle generateParticle();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;

    QVector<Particle> mParticles;
    int mNumberOfParticles;

    float mRadius;
    float mMaxLife;
    float mVelocity;
    float mScale;

    QMatrix4x4 mNozzleParticleTransformation; // For scaling

    // OpenGL stuff
    QVector<QVector3D> mVertices;
    QOpenGLVertexArrayObject *mVAO;
    unsigned int mVBO;
    unsigned int mPBO;

    static const float CUBE_VERTICES[108];
};

#endif // NOZZLEPARTICLES_H
