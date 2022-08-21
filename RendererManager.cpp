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
    , mWindowWidth(1600)
    , mWindowHeight(900)
    , mFlag(false)
{
    mNodeManager = NodeManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mShaderManager = ShaderManager::instance();
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
    //glEnable(GL_CULL_FACE);

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

    mScreenRenderer = new ScreenRenderer;
    mScreenRenderer->create();

    createFramebuffers();

    return true;
}

void RendererManager::resize(int w, int h)
{
    mWindowWidth = w;
    mWindowHeight = h;

    if (!mFlag)
    {
        mFlag = true;

        QTimer::singleShot(2000, [=]() {
            deleteFramebuffers();
            createFramebuffers();
            mFlag = false;
        });
    }
}

void RendererManager::render(float ifps)
{
    // First pass
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[0].framebuffer);
    glClearColor(mHaze->color().x(), mHaze->color().y(), mHaze->color().z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    renderModels(ifps);
    renderTerrain(ifps);
    renderParticles(ifps);

    // Second pass for stencil test
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[1].framebuffer);
    glStencilMask(0x00);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    renderModels(ifps);
    renderTerrain(ifps);
    renderParticles(ifps);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    mNozzleEffect->renderBlurEffect();

    // Apply blur
    applyBlur(mFramebuffers[1], mFramebuffers[0], BlurDirection::Horizontal);

    // Fill: 1 ----> 2
    fillFramebuffer(mFramebuffers[1], mFramebuffers[2]);

    // Apply blur
    applyBlur(mFramebuffers[1], mFramebuffers[2], BlurDirection::Vertical);

    // Fill: 1 ----> 2
    fillFramebuffer(mFramebuffers[1], mFramebuffers[2]);

    // Render to default framebuffer now
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFramebuffers[2].texture);
    mShaderManager->bind(ShaderManager::ShaderType::ScreenShader);
    mShaderManager->setUniformValue("screenTexture", 0);
    mShaderManager->setUniformValue("windowWidth", mWindowWidth);
    mShaderManager->setUniformValue("windowHeight", mWindowHeight);
    mScreenRenderer->render();
    mShaderManager->release();
}

void RendererManager::renderModels(float)
{
    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();

    if (mCamera)
    {
        mShaderManager->bind(ShaderManager::ShaderType::ModelShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::ShaderType::WireframeShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::ShaderType::NormalsShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::ShaderType::SkyBoxShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());

        QMatrix4x4 transformation = mCamera->worldTransformation();
        transformation.setColumn(3, QVector4D(0, 0, 0, 1));
        mShaderManager->setUniformValue("viewMatrix", transformation);
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::ShaderType::TerrainShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
        mShaderManager->release();
    }

    if (mSun)
    {
        mShaderManager->bind(ShaderManager::ShaderType::ModelShader);
        mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
        mShaderManager->setUniformValue("directionalLight.color", mSun->color());
        mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
        mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
        mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::ShaderType::TerrainShader);
        mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
        mShaderManager->setUniformValue("directionalLight.color", mSun->color());
        mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
        mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
        mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
        mShaderManager->release();
    }

    if (mHaze)
    {
        mShaderManager->setUniformValue("haze.enabled", mHaze->enabled());
        mShaderManager->setUniformValue("haze.color", mHaze->color());
        mShaderManager->setUniformValue("haze.density", mHaze->density());
        mShaderManager->setUniformValue("haze.gradient", mHaze->gradient());
    }

    mShaderManager->bind(ShaderManager::ShaderType::ModelShader);
    mShaderManager->setUniformValue("useBlinnShading", mUseBlinnShading);
    mShaderManager->release();

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
    mShaderManager->setUniformValue("skybox", 0);
    mSkyBox->render();
    mShaderManager->release();
}

void RendererManager::renderTerrain(float)
{
    //glEnable(GL_CLIP_DISTANCE0);

    mShaderManager->bind(ShaderManager::ShaderType::TerrainShader);
    mShaderManager->setUniformValue("nodeMatrix", mTerrain->transformation());
    mShaderManager->setUniformValue("terrain.amplitude", mTerrain->properties().amplitude);
    mShaderManager->setUniformValue("terrain.clipPlane", mTerrain->properties().clipPlane);
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
    mShaderManager->setUniformValue("waterHeight", 1.0f);
    mTerrain->render();
    mShaderManager->release();

    //glDisable(GL_CLIP_DISTANCE0);
}

void RendererManager::renderParticles(float ifps)
{
    mShaderManager->bind(ShaderManager::ShaderType::NozzleParticlesShader);
    mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
    mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
    mShaderManager->setUniformValue("modelMatrix", mNozzleEffect->getParticlesWorldTransformation());
    mShaderManager->setUniformValue("radius", mNozzleEffect->radius());
    mNozzleEffect->renderParticles(ifps);
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
            mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
            data->render(GL_TRIANGLES);
            mShaderManager->release();
        }

        if (mRenderNormals)
        {
            mShaderManager->bind(ShaderManager::ShaderType::NormalsShader);
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

void RendererManager::fillFramebuffer(Framebuffer read, Framebuffer draw)
{
    glBindFramebuffer(GL_FRAMEBUFFER, draw.framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, read.texture);
    mShaderManager->bind(ShaderManager::ShaderType::ScreenShader);
    mShaderManager->setUniformValue("screenTexture", 0);
    mShaderManager->setUniformValue("windowWidth", mWindowWidth);
    mShaderManager->setUniformValue("windowHeight", mWindowHeight);
    mScreenRenderer->render();
    mShaderManager->release();
}

void RendererManager::applyBlur(Framebuffer stencil, Framebuffer read, BlurDirection direction)
{
    glBindFramebuffer(GL_FRAMEBUFFER, stencil.framebuffer);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    mShaderManager->bind(ShaderManager::ShaderType::NozzleEffectShader);
    mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
    mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
    mShaderManager->setUniformValue("modelMatrix", mNozzleEffect->worldTransformation());
    mShaderManager->setUniformValue("screenTexture", 0);
    mShaderManager->setUniformValue("applyHorizontalBlur", direction == BlurDirection::Horizontal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, read.texture);
    mNozzleEffect->renderBlurEffect();
    mShaderManager->release();

    glDisable(GL_STENCIL_TEST);
}

bool RendererManager::createFramebuffers()
{
    for (int i = 0; i < 3; i++)
    {
        // Creat framebuffer
        glGenFramebuffers(1, &mFramebuffers[i].framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[i].framebuffer);

        // Create multisampled texture
        glGenTextures(1, &mFramebuffers[i].texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFramebuffers[i].texture);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, mWindowWidth, mWindowHeight, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mFramebuffers[i].texture, 0);

        // Create depth buffer
        glGenRenderbuffers(1, &mFramebuffers[i].renderObject);
        glBindRenderbuffer(GL_RENDERBUFFER, mFramebuffers[i].renderObject);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mFramebuffers[i].renderObject);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            qCritical() << "Framebuffer is could not be created.";
            return false;
        }
    }

    return true;
}

void RendererManager::deleteFramebuffers()
{
    for (int i = 0; i < 3; i++)
    {
        glDeleteFramebuffers(1, &mFramebuffers[i].framebuffer);
        glDeleteTextures(1, &mFramebuffers[i].texture);
        glDeleteBuffers(1, &mFramebuffers[i].renderObject);
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
