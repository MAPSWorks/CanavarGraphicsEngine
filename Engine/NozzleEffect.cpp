#include "NozzleEffect.h"

#include "Helper.h"

Canavar::Engine::NozzleEffect::NozzleEffect()
    : Node()
    , mNumberOfParticles(5000)
    , mMaxRadius(0.8f)
    , mMaxLife(0.0f)
    , mMaxDistance(10.0f)
    , mMinDistance(4.0f)
    , mSpeed(7.0f)
    , mScale(0.04f)
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
    initializeOpenGLFunctions();

    for (int i = 0; i < mNumberOfParticles; i++)
        mParticles << generateParticle();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), Canavar::Engine::CUBE, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *) 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.constData(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) 0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, direction));
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
        mParticles[i].life += ifps;

        if (mParticles[i].life >= mParticles[i].deadAfter)
            mParticles[i] = generateParticle();
    }

    mShaderManager->bind(ShaderType::NozzleEffectShader);
    mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getViewProjectionMatrix() * worldTransformation());
    mShaderManager->setUniformValue("scale", mScale);
    mShaderManager->setUniformValue("maxRadius", mMaxRadius);
    mShaderManager->setUniformValue("maxDistance", mMaxDistance);
    mShaderManager->setUniformValue("speed", mSpeed);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mParticles.size() * sizeof(Particle), mParticles.constData());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mNumberOfParticles);
    glBindVertexArray(0);

    mShaderManager->release();
}

Canavar::Engine::NozzleEffect::Particle Canavar::Engine::NozzleEffect::generateParticle()
{
    Particle p;

    float r = Helper::generateFloat(mMaxRadius);
    float theta = Helper::generateFloat(2.0f * M_PI);
    float distance = mMinDistance + Helper::generateFloat(mMaxDistance - mMinDistance);
    auto end = QVector3D(0, 0, distance);

    p.initialPosition = QVector3D(r * cos(theta), r * sin(theta), 0);
    p.direction = end - p.initialPosition;
    p.life = 0.0f;
    p.deadAfter = Helper::generateFloat(mMaxLife);
    return p;
}
