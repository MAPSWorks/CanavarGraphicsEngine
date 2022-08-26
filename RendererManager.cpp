#include "RendererManager.h"
#include "Camera.h"
#include "Helper.h"
#include "Light.h"
#include "Model.h"

#include <QDir>
#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mRenderObjects(true)
    , mRenderWireframe(false)
    , mRenderNormals(false)
    , mUseBlinnShading(true)
    , mWidth(1600)
    , mHeight(900)
    , mFlag(false)
{
    mNodeManager = NodeManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mShaderManager = ShaderManager::instance();

    mFramebufferFormat.setSamples(4);
    mFramebufferFormat.addColorAttachment(0, FramebufferFormat::TextureTarget::Texture2DMultisample, FramebufferFormat::TextureInternalFormat::RGBA8);
    mFramebufferFormat.setWidth(1600);
    mFramebufferFormat.setHeight(900);
}

RendererManager *RendererManager::instance()
{
    static RendererManager instance;
    return &instance;
}

bool RendererManager::init()
{
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);

    qInfo() << "Initializing ShaderManager...";

    if (!mShaderManager->init())
    {
        qWarning() << Q_FUNC_INFO << "ShaderManager could not be initialized.";
        return false;
    }

    loadModels();

    //    mSkyBox = new SkyBox;
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "Resources/Sky/1/right.png");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "Resources/Sky/1/left.png");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "Resources/Sky/1/top.png");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "Resources/Sky/1/bottom.png");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "Resources/Sky/1/front.png");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "Resources/Sky/1/back.png");
    //    mSkyBox->create();

    //    mSkyBox = new SkyBox;
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "Resources/Sky/2/right.bmp");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "Resources/Sky/2/left.bmp");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "Resources/Sky/2/top.bmp");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "Resources/Sky/2/bottom.bmp");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "Resources/Sky/2/front.bmp");
    //    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "Resources/Sky/2/back.bmp");
    //    mSkyBox->create();

    mHaze = new Haze;

    mTerrain = Terrain::instance();
    mTerrain->create();

    mQuad = new Quad;
    mQuad->create();

    mSun = dynamic_cast<DirectionalLight *>(mNodeManager->create(Node::NodeType::DirectionalLight));
    mSun->setDirection(QVector3D(1, -1, 1));

    mSun = dynamic_cast<DirectionalLight *>(mNodeManager->create(Node::NodeType::DirectionalLight));
    mSun->setDirection(QVector3D(1, -1, 1));

    mSky = Sky::instance();

    mWater = Water::instance();
    mWater->create();
    mWater->setHaze(mHaze);

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

        QTimer::singleShot(1000, [=]() {
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

    // For Water
    {
        glDisable(GL_MULTISAMPLE);

        QVector3D originalPosition = mCamera->worldPosition();
        QQuaternion originalRotation = mCamera->worldRotation();

        mCamera->setWorldPosition(originalPosition);
        mCamera->setWorldRotation(Helper::invertPitchAndRoll(originalRotation));

        // Render Weather
        mSky->renderWeather(ifps);

        // Render objects and sky
        mWater->reflectionFramebuffer()->bind();

        glClearColor(mHaze->color().x(), mHaze->color().y(), mHaze->color().z(), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderModels(ifps);
        renderTerrain(ifps);
        mSky->render(ifps);

        mCamera->setWorldPosition(originalPosition);
        mCamera->setWorldRotation(originalRotation);

        // Refraction
        mWater->refractionFramebuffer()->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderModels(ifps);
        renderTerrain(ifps);

        glEnable(GL_MULTISAMPLE);
    }

    // Scene
    {
        // Render Weather
        mSky->renderWeather(ifps);

        // Render objects and sky
        mFramebuffers[0]->bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(mHaze->color().x(), mHaze->color().y(), mHaze->color().z(), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderModels(ifps);
        renderTerrain(ifps);
        renderParticles(ifps);
        mWater->render(ifps);
        mSky->render(ifps);

        // Nozzle effect
        fillFramebuffer(mFramebuffers[0], mFramebuffers[1]);
        fillStencilBuffer(mFramebuffers[0]);
        applyNozzleBlur(mFramebuffers[0], mFramebuffers[1], 4);

        // Render to default framebuffer now
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_STENCIL_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mShaderManager->bind(ShaderManager::ShaderType::ScreenShader);
        mShaderManager->setSampler("screenTexture", 0, mFramebuffers[0]->texture(0), GL_TEXTURE_2D_MULTISAMPLE);
        mQuad->render();
        mShaderManager->release();
    }

    mCamera->updateVP();
}

void RendererManager::fillStencilBuffer(Framebuffer *framebuffer)
{
    // Fill stencil buffer
    framebuffer->bind();
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not draw any pixels on the back buffer
    glEnable(GL_STENCIL_TEST);                           // Enables testing AND writing functionalities
    glStencilFunc(GL_ALWAYS, 1, 0xFF);                   // Do not test the current value in the stencil buffer, always accept any value on there for drawing
    glStencilMask(0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_REPLACE);
    renderParticlesForStencilTest();
    glDisable(GL_STENCIL_TEST);
}

void RendererManager::applyNozzleBlur(Framebuffer *stencilSource, Framebuffer *textureSource, int times)
{
    // Blur pass
    for (int i = 0; i < times; i++)
    {
        stencilSource->bind();
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);          // Make sure you will no longer (over)write stencil values, even if any test succeeds
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.
        glStencilFunc(GL_EQUAL, 1, 0xFF);                // Now we will only draw pixels where the corresponding stencil buffer value equals 1
        mShaderManager->bind(ShaderManager::ShaderType::BlurShader);
        mShaderManager->setSampler("screenTexture", 0, textureSource->texture(), GL_TEXTURE_2D_MULTISAMPLE);
        mShaderManager->setUniformValue("horizontal", true);
        mQuad->render();
        mShaderManager->release();

        fillFramebuffer(stencilSource, textureSource);

        stencilSource->bind();
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);          // Make sure you will no longer (over)write stencil values, even if any test succeeds
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.
        glStencilFunc(GL_EQUAL, 1, 0xFF);                // Now we will only draw pixels where the corresponding stencil buffer value equals 1
        mShaderManager->bind(ShaderManager::ShaderType::BlurShader);
        mShaderManager->setSampler("screenTexture", 0, textureSource->texture(), GL_TEXTURE_2D_MULTISAMPLE);
        mShaderManager->setUniformValue("horizontal", false);
        mQuad->render();
        mShaderManager->release();

        fillFramebuffer(stencilSource, textureSource);

        glDisable(GL_STENCIL_TEST);
    }
}

void RendererManager::fillFramebuffer(Framebuffer *read, Framebuffer *draw)
{
    draw->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    mShaderManager->bind(ShaderManager::ShaderType::ScreenShader);
    mShaderManager->setSampler("screenTexture", 0, read->texture(), GL_TEXTURE_2D_MULTISAMPLE);
    mShaderManager->setUniformValue("width", mWidth);
    mShaderManager->setUniformValue("height", mHeight);
    mQuad->render();
    mShaderManager->release();
    glEnable(GL_DEPTH_TEST);
}

void RendererManager::renderModels(float)
{
    auto nodes = mNodeManager->nodes();

    for (const auto &node : nodes)
    {
        Model *model = dynamic_cast<Model *>(node);

        if (model)
            renderModel(model);
    }
}

void RendererManager::renderSkyBox(float)
{
    mShaderManager->bind(ShaderManager::ShaderType::SkyBoxShader);
    mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
    mShaderManager->setUniformValue("viewMatrix", mCamera->getWorldRotation());
    mShaderManager->setUniformValue("skybox", 0);
    mSkyBox->render();
    mShaderManager->release();
}

void RendererManager::renderTerrain(float)
{
    //    if (up != 0)
    //        glEnable(GL_CLIP_DISTANCE0);

    mShaderManager->bind(ShaderManager::ShaderType::TerrainShader);

    mShaderManager->setUniformValue("clipPlane", QVector4D(0, 1, 0, -mWater->waterHeight()) * 0);
    mShaderManager->setUniformValue("VP", mCamera->getVP());
    mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
    mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
    mShaderManager->setUniformValue("directionalLight.color", mSun->color());
    mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
    mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
    mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
    mShaderManager->setUniformValue("nodeMatrix", mTerrain->transformation());
    mShaderManager->setUniformValue("terrain.amplitude", mTerrain->properties().amplitude);
    mShaderManager->setUniformValue("terrain.seed", mTerrain->properties().seed);
    mShaderManager->setUniformValue("terrain.octaves", mTerrain->properties().octaves);
    mShaderManager->setUniformValue("terrain.frequency", mTerrain->properties().frequency);
    mShaderManager->setUniformValue("terrain.tessellationMultiplier", mTerrain->properties().tessellationMultiplier);
    mShaderManager->setUniformValue("terrain.power", mTerrain->properties().power);
    mShaderManager->setUniformValue("terrain.grassCoverage", mTerrain->properties().grassCoverage);
    mShaderManager->setUniformValue("terrain.ambient", mTerrain->material().ambient);
    mShaderManager->setUniformValue("terrain.diffuse", mTerrain->material().diffuse);
    mShaderManager->setUniformValue("terrain.shininess", mTerrain->material().shininess);
    mShaderManager->setUniformValue("terrain.specular", mTerrain->material().specular);
    mShaderManager->setUniformValue("haze.enabled", mHaze->enabled());
    mShaderManager->setUniformValue("haze.color", mHaze->color());
    mShaderManager->setUniformValue("haze.density", mHaze->density());
    mShaderManager->setUniformValue("haze.gradient", mHaze->gradient());
    mShaderManager->setUniformValue("waterHeight", mWater->waterHeight());
    mTerrain->render();
    mShaderManager->release();

    //    glDisable(GL_CLIP_DISTANCE0);
}

void RendererManager::renderParticles(float ifps)
{
    mShaderManager->bind(ShaderManager::ShaderType::NozzleEffectShader);
    mShaderManager->setUniformValue("MVP", mCamera->getVP() * mNozzleEffect->worldTransformation());
    mShaderManager->setUniformValue("radius", mNozzleEffect->radius());
    mShaderManager->setUniformValue("velocity", mNozzleEffect->velocity());
    mNozzleEffect->renderParticles(ifps);
    mShaderManager->release();
}

void RendererManager::renderParticlesForStencilTest()
{
    mShaderManager->bind(ShaderManager::ShaderType::NozzleEffectShader);
    mShaderManager->setUniformValue("MVP", mCamera->getVP() * mNozzleEffect->worldTransformation());
    mShaderManager->setUniformValue("radius", mNozzleEffect->radius());
    mShaderManager->setUniformValue("velocity", mNozzleEffect->velocity());
    mNozzleEffect->renderParticlesForStencilTest();
    mShaderManager->release();
}

void RendererManager::renderModel(Model *model)
{
    if (!model->visible())
        return;

    if (!model->renderable())
        return;

    ModelData *data = mModelsData.value(model->modelName(), nullptr);

    if (data)
    {
        if (mRenderObjects)
        {
            mShaderManager->bind(ShaderManager::ShaderType::ModelShader);
            mShaderManager->setUniformValue("useBlinnShading", mUseBlinnShading);
            mShaderManager->setUniformValue("VP", mCamera->getVP());
            mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
            mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
            mShaderManager->setUniformValue("directionalLight.color", mSun->color());
            mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
            mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
            mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
            mShaderManager->setUniformValue("haze.enabled", mHaze->enabled());
            mShaderManager->setUniformValue("haze.color", mHaze->color());
            mShaderManager->setUniformValue("haze.density", mHaze->density());
            mShaderManager->setUniformValue("haze.gradient", mHaze->gradient());

            QVector<PointLight *> pointLights = Helper::getClosePointLights(mLightManager->pointLights(), model);
            mShaderManager->setUniformValue("numberOfPointLights", pointLights.size());

            for (int i = 0; i < pointLights.size(); i++)
            {
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].color", pointLights[i]->color());
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].position", pointLights[i]->position());
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].ambient", pointLights[i]->ambient());
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].diffuse", pointLights[i]->diffuse());
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].specular", pointLights[i]->specular());
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].constant", pointLights[i]->constant());
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].linear", pointLights[i]->linear());
                mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].quadratic", pointLights[i]->quadratic());
            }

            QVector<SpotLight *> spotLights = Helper::getCloseSpotLights(mLightManager->spotLights(), model);
            mShaderManager->setUniformValue("numberOfSpotLights", spotLights.size());

            for (int i = 0; i < spotLights.size(); i++)
            {
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].color", spotLights[i]->color());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].position", spotLights[i]->position());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].ambient", spotLights[i]->ambient());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].diffuse", spotLights[i]->diffuse());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].specular", spotLights[i]->specular());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].constant", spotLights[i]->constant());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].linear", spotLights[i]->linear());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].quadratic", spotLights[i]->quadratic());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].direction", spotLights[i]->direction());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].cutOffAngle", spotLights[i]->cutOffAngle());
                mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].outerCutOffAngle", spotLights[i]->outerCutOffAngle());
            }

            data->render(model);
            mShaderManager->release();
        }

        if (mRenderWireframe)
        {
            mShaderManager->bind(ShaderManager::ShaderType::WireframeShader);
            mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
            mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
            mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
            data->render(GL_TRIANGLES);
            mShaderManager->release();
        }

        if (mRenderNormals)
        {
            mShaderManager->bind(ShaderManager::ShaderType::NormalsShader);
            mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
            mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
            mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
            data->render(GL_TRIANGLES);
            mShaderManager->release();
        }
    }

    auto children = model->children();

    for (const auto &child : children)
    {
        Model *model = dynamic_cast<Model *>(child);
        if (model)
            renderModel(model);
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
               << "*.gltf"
               << "*.bin";

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

void RendererManager::setNozzleEffect(NozzleEffect *newNozzleEffect)
{
    mNozzleEffect = newNozzleEffect;
}

void RendererManager::drawGui()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Graphics");

    // Render Settings
    if (!ImGui::CollapsingHeader("Render Settings"))
    {
        ImGui::Checkbox("Render Objects", &mRenderObjects);
        ImGui::Checkbox("Wireframe", &mRenderWireframe);
        ImGui::Checkbox("Render Normals", &mRenderNormals);
        ImGui::Checkbox("Use Blinn Shading", &mUseBlinnShading);
    }

    mSun->drawGui();
    mSky->drawGui();
    mHaze->drawGui();
    mTerrain->drawGui();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Seperate Window
    mNozzleEffect->drawGui();
}

ModelData *RendererManager::getModelData(const QString &modelName)
{
    return mModelsData.value(modelName, nullptr);
}
