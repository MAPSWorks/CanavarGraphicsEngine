#include "NozzleEffect.h"

#include <QDateTime>
#include <QtMath>

NozzleEffect::NozzleEffect(QObject *parent)
    : Node(parent)
    , mNumberOfParticles(50000)
    , mRadius(0.7f)
    , mMaxLife(0.075f)
    , mMinVelocity(2.0f)
    , mMaxVelocity(20.0f)
{
    mName = "Nozzle Effect";
    mNodeType = Node::NodeType::NozzleEffect;
    mRenderable = false;
    mRandomGenerator = QRandomGenerator::securelySeeded();
}

NozzleEffect::~NozzleEffect() {}

void NozzleEffect::create()
{
    for (int i = 0; i < mNumberOfParticles; i++)
        mParticles << generateParticle();

    initializeOpenGLFunctions();
    mVAO = new QOpenGLVertexArrayObject;
    mVAO->create();
    mVAO->bind();

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.constData(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) 0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, velocity));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, life));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    mVAO->release();
}

void NozzleEffect::renderParticles(float ifps)
{
    for (int i = 0; i < mParticles.size(); i++)
    {
        mParticles[i].life += ifps;
        if (mParticles[i].life >= mMaxLife + getRandomFloat(2 * mMaxLife))
            mParticles[i] = generateParticle();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mVAO->bind();
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mParticles.size() * sizeof(Particle), mParticles.constData());
    glDrawArraysInstanced(GL_POINTS, 0, 36, mNumberOfParticles);
    mVAO->release();

    glDisable(GL_BLEND);
}

void NozzleEffect::renderBlurEffect()
{
    mModelData->render(GL_TRIANGLES);
}

QMatrix4x4 NozzleEffect::getParticlesWorldTransformation()
{
    QMatrix4x4 transformation;
    transformation.rotate(mParticlesRotation);
    transformation.setColumn(3, QVector4D(mParticlesPosition, 1.0f));

    Node *node = dynamic_cast<Node *>(parent());

    if (node)
        return node->worldTransformation() * transformation;
    else
        return transformation;
}

ModelData *NozzleEffect::modelData() const
{
    return mModelData;
}

void NozzleEffect::setModelData(ModelData *newModelData)
{
    mModelData = newModelData;
}

float NozzleEffect::getRandomFloat(float bound)
{
    return mRandomGenerator.bounded(bound);
}

NozzleEffect::Particle NozzleEffect::generateParticle()
{
    double r = getRandomFloat(mRadius);
    double theta = getRandomFloat(2.0f * M_PI);
    Particle particle;
    particle.initialPosition = QVector3D(r * cos(theta), r * sin(theta), 0);
    particle.velocity = QVector3D(0, 0, mMinVelocity + getRandomFloat(mMaxVelocity - mMinVelocity));
    particle.life = 0.0f;
    return particle;
}

float NozzleEffect::radius() const
{
    return mRadius;
}

void NozzleEffect::setRadius(float newRadius)
{
    mRadius = newRadius;
}

float NozzleEffect::maxVelocity() const
{
    return mMaxVelocity;
}

void NozzleEffect::setMaxVelocity(float newMaxVelocity)
{
    mMaxVelocity = newMaxVelocity;
}

float NozzleEffect::minVelocity() const
{
    return mMinVelocity;
}

void NozzleEffect::setMinVelocity(float newMinVelocity)
{
    mMinVelocity = newMinVelocity;
}

const QQuaternion &NozzleEffect::particlesRotation() const
{
    return mParticlesRotation;
}

void NozzleEffect::setParticlesRotation(const QQuaternion &newParticlesRotation)
{
    mParticlesRotation = newParticlesRotation;
}

const QVector3D &NozzleEffect::particlesPosition() const
{
    return mParticlesPosition;
}

void NozzleEffect::setParticlesPosition(const QVector3D &newParticlesPosition)
{
    mParticlesPosition = newParticlesPosition;
}

float NozzleEffect::maxLife() const
{
    return mMaxLife;
}

void NozzleEffect::setMaxLife(float newMaxLife)
{
    mMaxLife = newMaxLife;
}

const float NozzleEffect::CUBE_VERTICES[108] = {-0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, -0.0075f, -0.0075f,
                                                -0.0075f, -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, 0.0075f,  0.0075f,
                                                -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,  -0.0075f, -0.0075f, -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  -0.0075f, -0.0075f, 0.0075f,  0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,
                                                0.0075f,  0.0075f,  -0.0075f, -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f};
