#include "Sky.h"

Canavar::Engine::Sky::Sky(QObject *parent)
    : Node(parent)
{
    mRenderable = true;
    mVisible = true;
    mType = Node::NodeType::Sky;

    mVertices << 0 << 0 << 0 << 0;

    mShaderManager = Canavar::Engine::ShaderManager::instance();
    mCameraManager = Canavar::Engine::CameraManager::instance();
    mLightManager = Canavar::Engine::LightManager::instance();
}

Canavar::Engine::Sky::~Sky()
{
    // TODO
}

void Canavar::Engine::Sky::create()
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices.constData(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned int), (void *) 0);
}

void Canavar::Engine::Sky::render()
{
    glDepthFunc(GL_LEQUAL);

    mShaderManager->bind(ShaderType::SkyShader);
    mShaderManager->setUniformValue("IVP", mCameraManager->activeCamera()->getProjectionMatrix().inverted());
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    mShaderManager->release();

    glDepthFunc(GL_LESS);
}
