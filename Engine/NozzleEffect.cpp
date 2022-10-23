#include "NozzleEffect.h"

#include "Helper.h"

Canavar::Engine::NozzleEffect::NozzleEffect(QObject *parent)
    : Node(parent)
    , mNumberOfParticles(10000)
    , mRadius(0.8f)
    , mMaxLife(0.1f)
    , mVelocity(7.0f)
    , mScale(0.015f)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();

    mType = Node::NodeType::NozzleEffect;
    mName = "Nozzle Effect";
}

Canavar::Engine::NozzleEffect::~NozzleEffect()
{
    // TODO
}

void Canavar::Engine::NozzleEffect::create()
{
    for (int i = 0; i < mNumberOfParticles; i++)
        mParticles << generateParticle();

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), CUBE, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.constData(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) 0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, velocityDirection));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, life));
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
}

void Canavar::Engine::NozzleEffect::render(float ifps)
{
    for (int i = 0; i < mParticles.size(); i++)
    {
        float r = mParticles[i].initialPosition.length();
        mParticles[i].life += ifps;
        if (mParticles[i].life >= mMaxLife + Helper::generateFloat(mMaxLife * (mRadius - r)))
            mParticles[i] = generateParticle();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QMatrix4x4 scaling;
    scaling.scale(mScale, mScale, mScale);

    mShaderManager->bind(ShaderType::NozzleEffectShader);
    mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getViewProjectionMatrix() * worldTransformation());
    mShaderManager->setUniformValue("scaling", scaling);
    mShaderManager->setUniformValue("radius", mRadius);
    mShaderManager->setUniformValue("velocity", mVelocity);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mParticles.size() * sizeof(Particle), mParticles.constData());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mNumberOfParticles);
    glBindVertexArray(0);

    mShaderManager->release();

    glDisable(GL_BLEND);
}

Canavar::Engine::NozzleEffect::Particle Canavar::Engine::NozzleEffect::generateParticle()
{
    Particle p;

    float r = Helper::generateFloat(mRadius);
    float theta = Helper::generateFloat(2.0f * M_PI);

    p.initialPosition = QVector3D(r * cos(theta), r * sin(theta), 0);
    p.velocityDirection = QVector3D(0, 0, 1.5 + Helper::generateFloat(0.5));
    p.life = 0.0f;

    return p;
}
