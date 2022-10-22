#include "RendererManager.h"
#include "CameraManager.h"
#include "Haze.h"
#include "Helper.h"
#include "LightManager.h"
#include "Model.h"
#include "NodeManager.h"
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

    mSky = Sky::instance();
    mSun = Sun::instance();
    mHaze = Haze::instance();
    mTerrain = Terrain::instance();

    mQuad = Quad::instance();

    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    loadModels("Resources/Models",
               QStringList() << "*.obj"
                             << "*.blend"
                             << "*.dae"
                             << "*.glb"
                             << "*.gltf");

    loadModels("Resources/Models", QStringList() << "*.fbx");

    mFBOFormats.insert(FramebufferType::Default, new QOpenGLFramebufferObjectFormat);
    mFBOFormats[FramebufferType::Default]->setSamples(4);
    mFBOFormats[FramebufferType::Default]->setAttachment(QOpenGLFramebufferObject::Depth);

    mFBOs.insert(FramebufferType::Default, new QOpenGLFramebufferObject(mWidth, mHeight, *mFBOFormats[FramebufferType::Default]));
    mFBOs[FramebufferType::Default]->addColorAttachment(mWidth, mHeight);

    mColorAttachments = new GLuint[2];
    mColorAttachments[0] = GL_COLOR_ATTACHMENT0;
    mColorAttachments[1] = GL_COLOR_ATTACHMENT1;

    mFBOs[FramebufferType::Default]->bind();
    glDrawBuffers(2, mColorAttachments);
    mFBOs[FramebufferType::Default]->release();

    return true;
}

void Canavar::Engine::RendererManager::resize(int w, int h)
{
    mWidth = w;
    mHeight = h;

    if (auto fbo = mFBOs.value(FramebufferType::Default))
    {
        delete fbo;
        mFBOs[FramebufferType::Default] = new QOpenGLFramebufferObject(w, h, *mFBOFormats[FramebufferType::Default]);
        mFBOs[FramebufferType::Default]->addColorAttachment(w, w);
        mFBOs[FramebufferType::Default]->bind();
        glDrawBuffers(2, mColorAttachments);
        mFBOs[FramebufferType::Default]->release();
    }
}

void Canavar::Engine::RendererManager::render(float ifps)
{
    mCamera = mCameraManager->activeCamera();
    mClosePointLights = Helper::getClosePointLights(mLightManager->getPointLights(), mCamera->worldPosition(), 8);

    mFBOs[FramebufferType::Default]->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    auto nodes = mNodeManager->nodes();

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

    // Render Models
    for (const auto &node : nodes)
    {
        if (!node->getVisible())
            continue;

        if (!node->getRenderable())
            continue;

        if (auto model = dynamic_cast<Model *>(node))
        {
            ModelData *data = mModelsData.value(model->getModelName(), nullptr);
            if (data)
                data->render(RenderPass::Default, model);
        }

        if (auto light = dynamic_cast<PointLight *>(node))
        {
            ModelData *data = mModelsData.value(light->getModelName(), nullptr);
            if (data)
                data->render(RenderPass::Default, light);
        }
    }

    mTerrain->render(RenderPass::Default);

    // Render Sky
    if (mSky->getEnabled())
        mSky->render();

    mFBOs[FramebufferType::Default]->release();

    QOpenGLFramebufferObject::blitFramebuffer(nullptr, //
                                              QRect(0, 0, mWidth, mHeight),
                                              mFBOs[FramebufferType::Default],
                                              QRect(0, 0, mWidth, mHeight),
                                              GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                                              GL_NEAREST,
                                              0,
                                              0);
}

void Canavar::Engine::RendererManager::loadModels(const QString &path, const QStringList &formats)
{
    // Models
    qInfo() << "Loading and creating all models at" << path << "whose extensions are" << formats;

    QDir dir(path);
    auto dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    for (const auto &dirName : qAsConst(dirs))
    {
        QDir childDir(dir.path() + "/" + dirName);
        childDir.setNameFilters(formats);
        auto files = childDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto &file : qAsConst(files))
        {
            ModelData *data = Canavar::Engine::Helper::loadModel(dirName, childDir.path() + "/" + file);

            if (data)
            {
                auto meshes = data->meshes();

                for (auto mesh : meshes)
                    mesh->create();

                mModelsData.insert(data->name(), data);
            }
        }
    }

    qInfo() << "All models are loaded at" << path;
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
