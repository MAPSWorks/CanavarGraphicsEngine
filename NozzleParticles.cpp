#include "NozzleParticles.h"
#include "RandomGenerator.h"

#include <QDateTime>
#include <QtMath>

NozzleParticles::NozzleParticles(Node *parent)
    : Node(parent)
    , mNumberOfParticles(10000)
    , mRadius(0.9f)
    , mMaxLife(0.25f)
    , mVelocity(5.0f)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();

    mNodeType = Node::NodeType::NozzleParticles;
    mName = "Nozzle Particles";
}

NozzleParticles::~NozzleParticles() {}

void NozzleParticles::create()
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
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

void NozzleParticles::render(const RenderSettings &settings)
{
    for (int i = 0; i < mParticles.size(); i++)
    {
        float r = mParticles[i].initialPosition.length();
        mParticles[i].life += settings.ifps;
        if (mParticles[i].life >= mMaxLife + RandomGenerator::getRandomFloat(mMaxLife * (mRadius - r)))
            mParticles[i] = generateParticle();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mShaderManager->bind(ShaderManager::ShaderType::NozzleParticlesShader);
    mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getVP() * worldTransformation());
    mShaderManager->setUniformValue("radius", mRadius);
    mShaderManager->setUniformValue("velocity", mVelocity);

    mVAO->bind();
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mParticles.size() * sizeof(Particle), mParticles.constData());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mNumberOfParticles);
    mVAO->release();

    mShaderManager->release();

    glDisable(GL_BLEND);
}

NozzleParticles::Particle NozzleParticles::generateParticle()
{
    float r = RandomGenerator::getRandomFloat(mRadius);
    float theta = RandomGenerator::getRandomFloat(2.0f * M_PI);
    Particle particle;
    particle.initialPosition = QVector3D(r * cos(theta), r * sin(theta), 0);
    particle.velocityDirection = QVector3D(0, 0, 0.5 + RandomGenerator::getRandomFloat(0.5));
    particle.life = 0.0f;
    return particle;
}

void NozzleParticles::drawGUI()
{
    if (!ImGui::CollapsingHeader("Parameters##NozzleParticles"))
    {
        ImGui::SliderFloat("Radius##NozzleParticles", &mRadius, 0.001f, 4.0f, "%.4f");
        ImGui::SliderFloat("Max Life##NozzleParticles", &mMaxLife, 0.001f, 10.0f, "%.4f");
        ImGui::SliderFloat("Velocity##NozzleParticles", &mVelocity, 0.0f, 50.0f, "%.3f");
    }

    Node::drawGUI();
}

const float NozzleParticles::CUBE_VERTICES[108] = {-0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  0.0175f,  0.0175f,  0.0175f,  -0.0175f, -0.0175f, -0.0175f,
                                                   -0.0175f, -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  -0.0175f, -0.0175f, -0.0175f, 0.0175f,  -0.0175f, -0.0175f, 0.0175f,
                                                   0.0175f,  -0.0175f, 0.0175f,  -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, 0.0175f,  0.0175f,
                                                   -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  -0.0175f, -0.0175f, 0.0175f,  -0.0175f, -0.0175f, -0.0175f, -0.0175f, 0.0175f,
                                                   0.0175f,  -0.0175f, -0.0175f, 0.0175f,  0.0175f,  -0.0175f, 0.0175f,  0.0175f,  0.0175f,  0.0175f,  0.0175f,  -0.0175f, -0.0175f, 0.0175f,
                                                   0.0175f,  -0.0175f, 0.0175f,  -0.0175f, -0.0175f, 0.0175f,  0.0175f,  0.0175f,  0.0175f,  -0.0175f, 0.0175f,  0.0175f,  0.0175f,  0.0175f,
                                                   0.0175f,  0.0175f,  -0.0175f, -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  0.0175f,  0.0175f,  -0.0175f, 0.0175f,  -0.0175f, -0.0175f, 0.0175f,
                                                   0.0175f,  0.0175f,  0.0175f,  0.0175f,  -0.0175f, 0.0175f,  0.0175f,  0.0175f,  -0.0175f, 0.0175f};
