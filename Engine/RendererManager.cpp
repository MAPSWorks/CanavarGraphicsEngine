#include "RendererManager.h"
#include "CameraManager.h"
#include "FirecrackerEffect.h"
#include "Haze.h"
#include "Helper.h"
#include "LightManager.h"
#include "Model.h"
#include "ModelDataManager.h"
#include "NodeManager.h"
#include "NozzleEffect.h"
#include "PointLight.h"
#include "ShaderManager.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"

#include <QDir>

Canavar::Engine::RendererManager::RendererManager(QObject *parent)
    : Manager(parent)
    , mWidth(1600)
    , mHeight(900)
    , mBlurPass(4)
    , mExposure(1.0f)
    , mGamma(1.0f)
{}

Canavar::Engine::RendererManager *Canavar::Engine::RendererManager::instance()
{
    static RendererManager instance;
    return &instance;
}

bool Canavar::Engine::RendererManager::init()
{
    mNodeManager = NodeManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mShaderManager = ShaderManager::instance();
    mModelDataManager = ModelDataManager::instance();

    mSky = Sky::instance();
    mSun = Sun::instance();
    mHaze = Haze::instance();
    mTerrain = Terrain::instance();

    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    mColorAttachments = new GLuint[2];
    mColorAttachments[0] = GL_COLOR_ATTACHMENT0;
    mColorAttachments[1] = GL_COLOR_ATTACHMENT1;

    // Default FBO format
    mFBOFormats.insert(FramebufferType::Default, new QOpenGLFramebufferObjectFormat);
    mFBOFormats[FramebufferType::Default]->setSamples(4);
    mFBOFormats[FramebufferType::Default]->setAttachment(QOpenGLFramebufferObject::Depth);

    // Other formats
    mFBOFormats.insert(FramebufferType::Temporary, new QOpenGLFramebufferObjectFormat);
    mFBOFormats.insert(FramebufferType::Ping, new QOpenGLFramebufferObjectFormat);
    mFBOFormats.insert(FramebufferType::Pong, new QOpenGLFramebufferObjectFormat);

    createFramebuffers(mWidth, mHeight);

    // Quad
    glGenVertexArrays(1, &mQuad.mVAO);
    glBindVertexArray(mQuad.mVAO);
    glGenBuffers(1, &mQuad.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuad.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::QUAD), Canavar::Engine::QUAD, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector2D), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector2D), (void *) sizeof(QVector2D));

    // Cube
    glGenVertexArrays(1, &mCube.mVAO);
    glBindVertexArray(mCube.mVAO);
    glGenBuffers(1, &mCube.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mCube.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), CUBE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *) 0);
    glEnableVertexAttribArray(0);

    // Cube Strip
    glGenVertexArrays(1, &mCubeStrip.mVAO);
    glBindVertexArray(mCubeStrip.mVAO);
    glGenBuffers(1, &mCubeStrip.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mCubeStrip.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE_STRIP), CUBE_STRIP, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *) 0);
    glEnableVertexAttribArray(0);

    return true;
}

void Canavar::Engine::RendererManager::resize(int w, int h)
{
    mWidth = w;
    mHeight = h;

    deleteFramebuffers();
    createFramebuffers(mWidth, mHeight);
}

void Canavar::Engine::RendererManager::render(float ifps)
{
    mCamera = mCameraManager->activeCamera();
    mClosePointLights = Helper::getClosePointLights(mLightManager->getPointLights(), mCamera->worldPosition(), 8);

    // Common uniforms
    mShaderManager->bind(ShaderType::ModelColoredShader);
    setCommonUniforms();
    mShaderManager->release();

    mShaderManager->bind(ShaderType::ModelTexturedShader);
    setCommonUniforms();
    mShaderManager->release();

    mShaderManager->bind(ShaderType::TerrainShader);
    setCommonUniforms();
    mShaderManager->release();

    mFBOs[FramebufferType::Default]->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    // Render Sky
    mSky->render();

    // Render terrain
    mTerrain->render();

    auto nodes = mNodeManager->nodes();

    // Render Models
    for (const auto &node : nodes)
    {
        if (!node->getVisible())
            continue;

        if (auto model = dynamic_cast<Model *>(node))
            if (auto data = mModelDataManager->getModelData(model->getModelName()))
                data->render(RenderMode::Default, model);
    }

    // Render Effects
    for (const auto &node : nodes)
    {
        if (!node->getVisible())
            continue;

        if (auto effect = dynamic_cast<NozzleEffect *>(node))
            effect->render(ifps);

        if (auto effect = dynamic_cast<FirecrackerEffect *>(node))
            effect->render(ifps);
    }

    // Selectables
    mShaderManager->bind(ShaderType::BasicShader);
    for (const auto &node : mSelectableRenderList)
    {
        if (auto model = dynamic_cast<Model *>(node))
        {
            if (auto data = mModelDataManager->getModelData(model->getModelName()))
                mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getViewProjectionMatrix() * node->worldTransformation() * data->getAABB().getTransformation());
            else
                mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getViewProjectionMatrix() * node->worldTransformation() * node->getAABB().getTransformation());
        } else if (node)
            mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getViewProjectionMatrix() * node->worldTransformation() * node->getAABB().getTransformation());

        glBindVertexArray(mCubeStrip.mVAO);
        glDrawArrays(GL_LINE_STRIP, 0, 17);
    }

    mShaderManager->release();

    // Default render pass is done
    mFBOs[FramebufferType::Default]->release();

    // Blur
    QOpenGLFramebufferObject::blitFramebuffer(mFBOs[FramebufferType::Ping], //
                                              QRect(0, 0, mWidth, mHeight),
                                              mFBOs[FramebufferType::Default],
                                              QRect(0, 0, mWidth, mHeight),
                                              GL_COLOR_BUFFER_BIT,
                                              GL_NEAREST,
                                              1,
                                              0);

    for (int i = 0; i < qMax(0, mBlurPass); i++)
    {
        mFBOs[i % 2 == 0 ? FramebufferType::Pong : FramebufferType::Ping]->bind();
        mShaderManager->bind(ShaderType::BlurShader);
        mShaderManager->setUniformValue("horizontal", i % 2 == 0);
        mShaderManager->setSampler("screenTexture", 0, mFBOs[i % 2 == 0 ? FramebufferType::Ping : FramebufferType::Pong]->texture());
        glBindVertexArray(mQuad.mVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        mShaderManager->release();
    }

    // Post process (combine blur and scene)
    QOpenGLFramebufferObject::blitFramebuffer(mFBOs[FramebufferType::Temporary], mFBOs[FramebufferType::Default]);

    QOpenGLFramebufferObject::bindDefault();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    mShaderManager->bind(ShaderType::PostProcessShader);
    mShaderManager->setSampler("sceneTexture", 0, mFBOs[FramebufferType::Temporary]->texture());
    mShaderManager->setSampler("bloomBlurTexture", 1, mFBOs[qMax(0, mBlurPass) % 2 == 0 ? FramebufferType::Ping : FramebufferType::Pong]->texture());
    mShaderManager->setUniformValue("exposure", mExposure);
    mShaderManager->setUniformValue("gamma", mGamma);
    glBindVertexArray(mQuad.mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mShaderManager->release();
}

void Canavar::Engine::RendererManager::setCommonUniforms()
{
    mShaderManager->setUniformValue("cameraPos", mCamera->worldPosition());
    mShaderManager->setUniformValue("VP", mCamera->getViewProjectionMatrix());

    mShaderManager->setUniformValue("sun.direction", -mSun->getDirection().normalized());
    mShaderManager->setUniformValue("sun.color", mSun->getColor());
    mShaderManager->setUniformValue("sun.ambient", mSun->getAmbient());
    mShaderManager->setUniformValue("sun.diffuse", mSun->getDiffuse());
    mShaderManager->setUniformValue("sun.specular", mSun->getSpecular());

    mShaderManager->setUniformValue("haze.enabled", mHaze->getEnabled());
    mShaderManager->setUniformValue("haze.color", mHaze->getColor());
    mShaderManager->setUniformValue("haze.density", mHaze->getDensity());
    mShaderManager->setUniformValue("haze.gradient", mHaze->getGradient());

    mShaderManager->setUniformValue("numberOfPointLights", (int) mClosePointLights.size());

    for (int i = 0; i < mClosePointLights.size(); i++)
    {
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].color", mClosePointLights[i]->getColor());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].position", mClosePointLights[i]->worldPosition());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].ambient", mClosePointLights[i]->getAmbient());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].diffuse", mClosePointLights[i]->getDiffuse());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].specular", mClosePointLights[i]->getSpecular());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].constant", mClosePointLights[i]->getConstant());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].linear", mClosePointLights[i]->getLinear());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].quadratic", mClosePointLights[i]->getQuadratic());
    }
}

void Canavar::Engine::RendererManager::deleteFramebuffers()
{
    auto keys = mFBOs.keys();

    for (auto type : keys)
        if (mFBOs[type])
            delete mFBOs[type];
}

void Canavar::Engine::RendererManager::createFramebuffers(int width, int height)
{
    auto keys = mFBOFormats.keys();

    for (auto type : keys)
    {
        mFBOs.insert(type, new QOpenGLFramebufferObject(width, height, *mFBOFormats[type]));

        if (type == FramebufferType::Default)
        {
            mFBOs[FramebufferType::Default]->addColorAttachment(width, height, GL_RGBA16F);

            mFBOs[FramebufferType::Default]->bind();
            glDrawBuffers(2, mColorAttachments);
            mFBOs[FramebufferType::Default]->release();
        }
    }
}

void Canavar::Engine::RendererManager::addSelectable(Node *node)
{
    if (node)
    {
        if (!mSelectableRenderList.contains(node))
        {
            mSelectableRenderList << node;
            connect(node, &QObject::destroyed, this, [=]() { mSelectableRenderList.removeAll(node); });
        }
    }
}

void Canavar::Engine::RendererManager::removeSelectable(Node *node)
{
    if (node)
    {
        if (mSelectableRenderList.contains(node))
        {
            node->disconnect(this);
            mSelectableRenderList.removeAll(node);
        }
    }
}
