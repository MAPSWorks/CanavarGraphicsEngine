#include "FirecrackerEffect.h"

#include "Helper.h"

Canavar::Engine::FirecrackerEffect::FirecrackerEffect(QObject *parent)
    : Node(parent)
    , mNumberOfParticles(10000)
    , mSpanAngle(140.0f)
    , mGravityDirection(0, -1, 0)
    , mGravity(9.8f)
    , mMaxLife(12.0f)
    , mMinLife(1.0f)
    , mInitialSpeed(250.0f)
    , mLoop(false)
    , mScale(1.0f)
    , mDamping(1.0f)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();

    mType = Node::NodeType::FirecrackerEffect;
    mName = "Firecracker Effect";
}

Canavar::Engine::FirecrackerEffect::~FirecrackerEffect()
{
    // TODO
}

void Canavar::Engine::FirecrackerEffect::create()
{
    initializeOpenGLFunctions();

    for (int i = 0; i < mNumberOfParticles; i++)
        mParticles << generateParticle();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), Canavar::Engine::CUBE, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.constData(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) 0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, life));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, deadAfter));
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
}

void Canavar::Engine::FirecrackerEffect::render(float ifps)
{
    for (int i = 0; i < mParticles.size(); i++)
    {
        if (!mLoop && mParticles[i].dead)
            continue;

        auto dampingFactor = mDamping * mParticles[i].velocity.length() * mParticles[i].velocity.normalized() * ifps;

        mParticles[i].velocity += mGravity * mGravityDirection * ifps - dampingFactor;
        mParticles[i].position += mParticles[i].velocity * ifps;
        mParticles[i].life += ifps;

        if (mParticles[i].life >= mParticles[i].deadAfter)
        {
            if (mLoop)
                mParticles[i] = generateParticle();
            else
                mParticles[i].dead = true;
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mShaderManager->bind(ShaderType::FirecrackerEffectShader);
    mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getViewProjectionMatrix() * worldTransformation());
    mShaderManager->setUniformValue("scale", mScale);
    mShaderManager->setUniformValue("maxLife", mMaxLife);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mParticles.size() * sizeof(Particle), mParticles.constData());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mNumberOfParticles);
    glBindVertexArray(0);

    mShaderManager->release();

    glDisable(GL_BLEND);
}

Canavar::Engine::FirecrackerEffect::Particle Canavar::Engine::FirecrackerEffect::generateParticle()
{
    Particle p;

    float theta = Helper::generateFloat(M_PI * mSpanAngle / 180.0f);
    float phi = Helper::generateFloat(2 * M_PI);

    float x = sin(theta) * cos(phi);
    float z = sin(theta) * sin(phi);
    float y = cos(theta);

    p.position = QVector3D(0, 0, 0);
    p.velocity = Helper::generateBetween(0.5 * mInitialSpeed, mInitialSpeed) * QVector3D(x, y, z);
    p.life = 0.0f;
    p.deadAfter = mMinLife + Helper::generateFloat(mMaxLife - mMinLife);
    p.dead = false;
    return p;
}
