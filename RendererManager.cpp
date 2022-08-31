#include "RendererManager.h"
#include "Helper.h"
#include "Model.h"
#include "NozzleParticles.h"
#include <QDir>
#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : Manager(parent)
    , mWidth(1600)
    , mHeight(900)
    , mFlag(false)

{
    mFramebufferFormat.setSamples(4);
    mFramebufferFormat.addColorAttachment(0, //
                                          FramebufferFormat::TextureTarget::TEXTURE_2D_MULTISAMPLE,
                                          FramebufferFormat::TextureInternalFormat::RGBA8,
                                          FramebufferFormat::TexturePixelFormat::RGBA,
                                          FramebufferFormat::TextureDataType::UNSIGNED_BYTE);
    mFramebufferFormat.setWidth(1600);
    mFramebufferFormat.setHeight(900);

    mRenderSettings.ifps = 0;
    mRenderSettings.renderFor = RenderFor::Default;
    mRenderSettings.useBlinnShading = false;
    mRenderSettings.renderModels = true;
    mRenderSettings.renderWireframe = false;
    mRenderSettings.renderNormals = false;
}

RendererManager *RendererManager::instance()
{
    static RendererManager instance;
    return &instance;
}

bool RendererManager::init()
{
    mNodeManager = NodeManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mShaderManager = ShaderManager::instance();
    mHaze = Haze::instance();
    mTerrain = Terrain::instance();
    mWater = Water::instance();
    mSky = Sky::instance();
    mQuad = Quad::instance();

    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    loadModels();

    mSun = dynamic_cast<DirectionalLight *>(mNodeManager->create(Node::NodeType::DirectionalLight));
    mSun->setDirection(QVector3D(1, -1, 1));

    createFramebuffers();

    return true;
}

void RendererManager::resize(int w, int h)
{
    mWidth = w;
    mHeight = h;

    if (!mFlag)
    {
        mFlag = true;

        QTimer::singleShot(250, [=]() {
            mSky->resize(mWidth, mHeight);
            mWater->resize(mWidth, mHeight);
            deleteFramebuffers();
            createFramebuffers();
            mFlag = false;
        });
    }
}

void RendererManager::render(float ifps)
{
    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();

    mRenderSettings.ifps = ifps;

    // For Water
    {
        glDisable(GL_MULTISAMPLE);

        QVector3D originalPosition = mCamera->worldPosition();
        QQuaternion originalRotation = mCamera->worldRotation();

        float x = originalPosition.x();
        float y = originalPosition.y() - 2 * (originalPosition.y() - mWater->waterHeight());
        float z = originalPosition.z();

        mCamera->setWorldRotation(Helper::invert(originalRotation));
        mCamera->setWorldPosition(QVector3D(x, y, z));

        // Render Weather
        mSky->renderWeather(mRenderSettings);

        // Reflection
        mRenderSettings.renderFor = RenderFor::Reflection;
        mWater->reflectionFramebuffer()->bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderNodes();
        mTerrain->render(mRenderSettings);
        mSky->render(mRenderSettings);

        mCamera->setWorldPosition(originalPosition);
        mCamera->setWorldRotation(originalRotation);

        // Refraction
        mRenderSettings.renderFor = RenderFor::Refraction;
        mWater->refractionFramebuffer()->bind();
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderNodes();
        mTerrain->render(mRenderSettings);

        glEnable(GL_MULTISAMPLE);
    }

    mRenderSettings.renderFor = RenderFor::Default;

    // Scene
    {
        // Render Weather
        mSky->renderWeather(mRenderSettings);

        // Render objects and sky
        mFramebuffers[0]->bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(mHaze->color().x(), mHaze->color().y(), mHaze->color().z(), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderNodes();
        mTerrain->render(mRenderSettings);
        mWater->render(mRenderSettings);
        mSky->render(mRenderSettings);

        // Render to default framebuffer now
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_STENCIL_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mShaderManager->bind(ShaderManager::ShaderType::ScreenMultisampledShader);
        mShaderManager->setSampler("screenTexture", 0, mFramebuffers[0]->texture(), GL_TEXTURE_2D_MULTISAMPLE);
        mQuad->render();
        mShaderManager->release();
    }
}

void RendererManager::applyBlur(Framebuffer *read, Framebuffer *draw, int times)
{
    mShaderManager->bind(ShaderManager::ShaderType::BlurShader);
    mShaderManager->setUniformValue("width", mWidth);
    mShaderManager->setUniformValue("height", mHeight);
    mShaderManager->release();

    glDisable(GL_DEPTH_TEST);

    // Blur pass
    for (int i = 0; i < times; i++)
    {
        draw->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mShaderManager->bind(ShaderManager::ShaderType::BlurShader);
        mShaderManager->setSampler("screenTexture", 0, read->texture());
        mShaderManager->setUniformValue("horizontal", true);
        mQuad->render();
        mShaderManager->release();

        if (draw->format().samples() == 0)
            fillFramebuffer(draw, read);
        else
            fillFramebufferMultisampled(draw, read);

        draw->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mShaderManager->bind(ShaderManager::ShaderType::BlurShader);
        mShaderManager->setSampler("screenTexture", 0, read->texture());
        mShaderManager->setUniformValue("horizontal", false);
        mQuad->render();
        mShaderManager->release();

        if (draw->format().samples() == 0)
            fillFramebuffer(draw, read);
        else
            fillFramebufferMultisampled(draw, read);
    }

    glEnable(GL_DEPTH_TEST);
}

void RendererManager::applyBlurMultisampled(Framebuffer *read, Framebuffer *draw, int times)
{
    // Blur pass
    for (int i = 0; i < times; i++)
    {
        draw->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mShaderManager->bind(ShaderManager::ShaderType::BlurMultisampledShader);
        mShaderManager->setSampler("screenTexture", 0, read->texture(), GL_TEXTURE_2D_MULTISAMPLE);
        mShaderManager->setUniformValue("horizontal", true);
        mQuad->render();
        mShaderManager->release();

        fillFramebufferMultisampled(draw, read);

        draw->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mShaderManager->bind(ShaderManager::ShaderType::BlurMultisampledShader);
        mShaderManager->setSampler("screenTexture", 0, read->texture(), GL_TEXTURE_2D_MULTISAMPLE);
        mShaderManager->setUniformValue("horizontal", false);
        mQuad->render();
        mShaderManager->release();

        fillFramebufferMultisampled(draw, read);
    }
}

void RendererManager::fillFramebuffer(Framebuffer *source, Framebuffer *target)
{
    target->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    mShaderManager->bind(ShaderManager::ShaderType::ScreenShader);
    mShaderManager->setSampler("screenTexture", 0, source->texture());
    mQuad->render();
    mShaderManager->release();
    glEnable(GL_DEPTH_TEST);
}

void RendererManager::fillFramebufferMultisampled(Framebuffer *source, Framebuffer *target)
{
    target->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    mShaderManager->bind(ShaderManager::ShaderType::ScreenMultisampledShader);
    mShaderManager->setSampler("screenTexture", 0, source->texture(), GL_TEXTURE_2D_MULTISAMPLE);
    mQuad->render();
    mShaderManager->release();
    glEnable(GL_DEPTH_TEST);
}

void RendererManager::renderNodes()
{
    auto nodes = mNodeManager->nodes();

    for (const auto &node : nodes)
    {
        if (!node->visible())
            continue;

        if (!node->renderable())
            continue;

        Model *model = dynamic_cast<Model *>(node);
        NozzleParticles *particles = dynamic_cast<NozzleParticles *>(node);

        if (model)
        {
            ModelData *data = mModelsData.value(model->modelName(), nullptr);
            if (data)
                data->render(model, mRenderSettings);
        }

        if (particles)
            particles->render(mRenderSettings);
    }
}

void RendererManager::createFramebuffers()
{
    mFramebufferFormat.setWidth(mWidth);
    mFramebufferFormat.setHeight(mHeight);
    for (int i = 0; i < 2; i++)
    {
        mFramebuffers[i] = new Framebuffer(mFramebufferFormat);
    }
}

void RendererManager::deleteFramebuffers()
{
    for (int i = 0; i < 2; i++)
    {
        mFramebuffers[i]->deleteLater();
        mFramebuffers[i] = nullptr;
    }
}

void RendererManager::loadModels()
{
    // Models
    qInfo() << "Loading and creating all models...";

    QDir dir("Resources/Models");
    auto dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList extensions;
    extensions << "*.obj"
               << "*.blend"
               << "*.dae"
               << "*.glb"
               << "*.gltf";

    for (const auto &dirName : qAsConst(dirs))
    {
        QDir childDir(dir.path() + "/" + dirName);
        childDir.setNameFilters(extensions);
        auto files = childDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto &file : qAsConst(files))
        {
            ModelData *data = Helper::loadModel(dirName, childDir.path() + "/" + file);

            if (data)
            {
                bool failure = false;
                auto meshes = data->meshes();

                for (auto mesh : meshes)
                {
                    if (!mesh->create())
                    {
                        failure = true;
                        break;
                    }
                }

                if (failure)
                {
                    data->deleteLater();
                    continue;
                }

                mModelsData.insert(data->modelName(), data);
            }
        }
    }

    qInfo() << "All textured models are loaded.";
}

void RendererManager::drawGUI()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Graphics");

    // Render Settings
    if (!ImGui::CollapsingHeader("Render Settings"))
    {
        ImGui::Checkbox("Render Models", &mRenderSettings.renderModels);
        ImGui::Checkbox("Render Wireframe", &mRenderSettings.renderWireframe);
        ImGui::Checkbox("Render Normals", &mRenderSettings.renderNormals);
        ImGui::Checkbox("Use Blinn Shading", &mRenderSettings.useBlinnShading);
    }

    mSun->drawGUI();
    mSky->drawGUI();
    mHaze->drawGUI();
    mWater->drawGUI();
    mTerrain->drawGUI();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}

ModelData *RendererManager::getModelData(const QString &modelName)
{
    return mModelsData.value(modelName, nullptr);
}
