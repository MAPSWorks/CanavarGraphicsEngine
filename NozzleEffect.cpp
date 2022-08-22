#include "NozzleEffect.h"

#include <QDateTime>
#include <QtMath>

NozzleEffect::NozzleEffect(QObject *parent)
    : Node(parent)
    , mNumberOfParticles(50000)
    , mRadius(0.7f)
    , mMaxLife(0.25f)
    , mVelocity(5.0f)
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
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) offsetof(Particle, velocityDirection));

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
        float r = mParticles[i].initialPosition.length();
        mParticles[i].life += ifps;
        if (mParticles[i].life >= mMaxLife + getRandomFloat(mMaxLife * (mRadius - r)))
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
    particle.velocityDirection = QVector3D(0, 0, 0.5 + getRandomFloat(0.5));
    particle.life = 0.0f;
    return particle;
}

float NozzleEffect::maxLife() const
{
    return mMaxLife;
}

float NozzleEffect::velocity() const
{
    return mVelocity;
}

float NozzleEffect::radius() const
{
    return mRadius;
}

void NozzleEffect::drawGui()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Nozzle Effect");
    ImGui::Text("Parameters:");
    ImGui::SliderFloat("Radius##NozzleEffect", &mRadius, 0.001f, 4.0f, "%.4f");
    ImGui::SliderFloat("Max Life##NozzleEffect", &mMaxLife, 0.001f, 10.0f, "%.4f");
    ImGui::SliderFloat("Velocity##NozzleEffect", &mVelocity, 0.0f, 50.0f, "%.3f");

    Node::drawGui();
}

const float NozzleEffect::CUBE_VERTICES[108] = {-0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, -0.0075f, -0.0075f,
                                                -0.0075f, -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, -0.0075f, 0.0075f,  0.0075f,
                                                -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,  -0.0075f, -0.0075f, -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  -0.0075f, -0.0075f, 0.0075f,  0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  0.0075f,
                                                0.0075f,  0.0075f,  -0.0075f, -0.0075f, 0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f,  -0.0075f, -0.0075f, 0.0075f,
                                                0.0075f,  0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f,  0.0075f,  0.0075f,  -0.0075f, 0.0075f};
