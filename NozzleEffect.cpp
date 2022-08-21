#include "NozzleEffect.h"

#include <QDateTime>
#include <QtMath>

NozzleEffect::NozzleEffect(QObject *parent)
    : Node(parent)
    , mNumberOfParticles(50000)
    , mRadius(0.7f)
    , mMaxLife(0.05f)
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

void NozzleEffect::drawGui()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Nozzle Effect");

    ImGui::SliderFloat("Radius##NozzleEffect", &mRadius, 0.001f, 4.0f, "%.3f");
    ImGui::SliderFloat("Max Life##NozzleEffect", &mMaxLife, 0.001f, 2.0f, "%.3f");
    ImGui::SliderFloat("Min Velocity##NozzleEffect", &mMinVelocity, 0.0f, 20.0f, "%.3f");
    ImGui::SliderFloat("Max Velocity##NozzleEffect", &mMaxVelocity, 20.0f, 100.0f, "%.3f");

    // Particles Position
    {
        ImGui::Text("Particles Position:");
        float x = mParticlesPosition.x();
        float y = mParticlesPosition.y();
        float z = mParticlesPosition.z();

        if (ImGui::DragFloat("x##NozzleEffectParticlesPosition", &x, 0.01f, -50.0f, 50.0f, "%.3f"))
            mParticlesPosition = QVector3D(x, y, z);
        if (ImGui::DragFloat("y##NozzleEffectParticlesPosition", &y, 0.01f, -50.0f, 50.0f, "%.3f"))
            mParticlesPosition = QVector3D(x, y, z);
        if (ImGui::DragFloat("z##NozzleEffectParticlesPosition", &z, 0.01f, -50.0f, 50.0f, "%.3f"))
            mParticlesPosition = QVector3D(x, y, z);
    }

    // Particles Rotation
    {
        ImGui::Text("Particles Rotation:");
        float w = mParticlesRotation.scalar();
        float x = mParticlesRotation.x();
        float y = mParticlesRotation.y();
        float z = mParticlesRotation.z();

        if (ImGui::DragFloat("w##NozzleEffectParticlesRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
            mParticlesRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("x##NozzleEffectParticlesRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            mParticlesRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("y##NozzleEffectParticlesRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
            mParticlesRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("z##NozzleEffectParticlesRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            mParticlesRotation = QQuaternion(w, x, y, z).normalized();
    }

    // Blur Position
    {
        ImGui::Text("Blur Position:");
        float x = mPosition.x();
        float y = mPosition.y();
        float z = mPosition.z();

        if (ImGui::DragFloat("x##NozzleEffectBlurPosition", &x, 0.01f, -50.0f, 50.0f, "%.3f"))
            mPosition = QVector3D(x, y, z);
        if (ImGui::DragFloat("y##NozzleEffectBlurPosition", &y, 0.01f, -50.0f, 50.0f, "%.3f"))
            mPosition = QVector3D(x, y, z);
        if (ImGui::DragFloat("z##NozzleEffectBlurPosition", &z, 0.01f, -50.0f, 50.0f, "%.3f"))
            mPosition = QVector3D(x, y, z);
    }

    // Blur Rotation
    {
        ImGui::Text("Blur Rotation:");
        float w = mRotation.scalar();
        float x = mRotation.x();
        float y = mRotation.y();
        float z = mRotation.z();

        if (ImGui::DragFloat("w##NozzleEffectBlurRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("x##NozzleEffectBlurRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("y##NozzleEffectBlurRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
        if (ImGui::DragFloat("z##NozzleEffectBlurRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
            mRotation = QQuaternion(w, x, y, z).normalized();
    }

    // Blur Scale
    {
        ImGui::Text("Blur Scale:");
        float x = mScale.x();
        float y = mScale.y();
        float z = mScale.z();

        if (ImGui::DragFloat("x##NozzleEffectBlurScale", &x, 0.00001f, 0.00001f, 1.0f, "%.6f"))
            mScale = QVector3D(x, y, z);
        if (ImGui::DragFloat("y##NozzleEffectBlurScale", &y, 0.00001f, 0.00001f, 1.0f, "%.6f"))
            mScale = QVector3D(x, y, z);
        if (ImGui::DragFloat("z##NozzleEffectBlurScale", &z, 0.00001f, 0.00001f, 1.0f, "%.6f"))
            mScale = QVector3D(x, y, z);
    }
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
