#include "Water.h"
#include "LightManager.h"

Water::Water(QObject *parent)
    : QObject(parent)
    , mWaterHeight(100.0f)
    , mWaveSpeed(0.25f)
    , mTimeElapsed(0.0f)
    , mReflectionFramebuffer(nullptr)
    , mRefractionFramebuffer(nullptr)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();

    mFramebufferFormat.setSamples(0);
    mFramebufferFormat.addColorAttachment(0, FramebufferFormat::TextureTarget::Texture2D, FramebufferFormat::TextureInternalFormat::RGBA8);
    mFramebufferFormat.setWidth(1600);
    mFramebufferFormat.setHeight(900);

    mTransformation.translate(QVector3D(0, mWaterHeight, 0));
    mTransformation.scale(128, 1, 128);

    createFramebuffers();
}

Water *Water::instance()
{
    static Water water;
    return &water;
}

void Water::create(int numberOfVerticesOnEdge, float width)
{
    mNumberOfVerticesOnEdge = numberOfVerticesOnEdge;
    const int n = numberOfVerticesOnEdge;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            float x = j * width / (n - 1) - width / 2.0f;
            float y = 0;
            float z = i * width / (n - 1) - width / 2.0f;
            float u = float(j) / (n - 1);
            float v = float(n - i - 1) / (n - 1);

            Vertex vertex;
            vertex.position = QVector3D(x, y, z);
            vertex.normal = QVector3D(0, 1, 0);
            vertex.texture = QVector2D(u, v);

            mVertices << vertex;
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - 1; j++)
        {
            mIndices << n * i + j;
            mIndices << n * (i + 1) + j;
            mIndices << n * i + j + 1;

            mIndices << n * (i + 1) + j;
            mIndices << n * (i + 1) + j + 1;
            mIndices << n * i + j + 1;
        }
    }

    initializeOpenGLFunctions();
    mVAO = new QOpenGLVertexArrayObject;
    mVAO->create();
    mVAO->bind();

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);
}

void Water::render(float ifps)
{
    mTimeElapsed += ifps;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_MULTISAMPLE);

    mShaderManager->bind(ShaderManager::ShaderType::WaterShader);
    mShaderManager->setUniformValue("nodeMatrix", mTransformation);

    mShaderManager->setUniformValue("cameraPosition", mCameraManager->activeCamera()->worldPosition());
    mShaderManager->setUniformValue("VP", mCameraManager->activeCamera()->getVP());

    mShaderManager->setUniformValue("lightColor", mLightManager->directionalLight()->color().toVector3D());
    mShaderManager->setUniformValue("lightDirection", mLightManager->directionalLight()->direction());

    mShaderManager->setUniformValue("moveFactor", mWaveSpeed * mTimeElapsed);

    mShaderManager->setSampler("reflectionTex", 0, mReflectionFramebuffer->texture());
    mShaderManager->setSampler("refractionTex", 1, mRefractionFramebuffer->texture());
    mShaderManager->setSampler("depthMap", 2, mRefractionFramebuffer->depth());

    mVAO->bind();
    glDrawElements(GL_TRIANGLES, (mNumberOfVerticesOnEdge - 1) * (mNumberOfVerticesOnEdge - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
    mShaderManager->release();
    mVAO->release();

    glDisable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
}

void Water::resize(int width, int height)
{
    mFramebufferFormat.setWidth(width);
    mFramebufferFormat.setHeight(height);
    deleteFramebuffers();
    createFramebuffers();
}

void Water::createFramebuffers()
{
    mReflectionFramebuffer = new Framebuffer(mFramebufferFormat);
    mRefractionFramebuffer = new Framebuffer(mFramebufferFormat);
}

void Water::deleteFramebuffers()
{
    if (mReflectionFramebuffer)
        mReflectionFramebuffer->deleteLater();

    if (mRefractionFramebuffer)
        mRefractionFramebuffer->deleteLater();
}

Framebuffer *Water::refractionFramebuffer() const
{
    return mRefractionFramebuffer;
}

Framebuffer *Water::reflectionFramebuffer() const
{
    return mReflectionFramebuffer;
}

float Water::waterHeight() const
{
    return mWaterHeight;
}

void Water::setHaze(Haze *newHaze)
{
    mHaze = newHaze;
}
