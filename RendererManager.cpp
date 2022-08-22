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

    mQuad = new Quad;
    mQuad->create();

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

    // Render
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[0].framebuffer);
    glClearColor(mHaze->color().x(), mHaze->color().y(), mHaze->color().z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    renderModels(ifps);
    renderTerrain(ifps);
    renderParticles(ifps);

    // Nozzle effect
    // Fill: 0 ----> 1
    fillFramebuffer(mFramebuffers[0], mFramebuffers[1]);
    fillStencilBuffer(mFramebuffers[0], ifps);
    applyBlur(mFramebuffers[0], mFramebuffers[1]);

    // Render to default framebuffer now
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFramebuffers[0].texture);
    mShaderManager->bind(ShaderManager::ShaderType::ScreenShader);
    mShaderManager->setUniformValue("screenTexture", 0);
    mQuad->render();
    mShaderManager->release();
}

void RendererManager::fillStencilBuffer(Framebuffer framebuffer, float ifps)
{
    // Fill stencil buffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.framebuffer);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not draw any pixels on the back buffer
    glEnable(GL_STENCIL_TEST);                           // Enables testing AND writing functionalities
    glStencilFunc(GL_ALWAYS, 1, 0xFF);                   // Do not test the current value in the stencil buffer, always accept any value on there for drawing
    glStencilMask(0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_REPLACE);
    renderParticles(ifps);
    glDisable(GL_STENCIL_TEST);
}

void RendererManager::applyBlur(Framebuffer stencilSource, Framebuffer textureSource)
{
    // Blur pass
    glBindFramebuffer(GL_FRAMEBUFFER, stencilSource.framebuffer);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);          // Make sure you will no longer (over)write stencil values, even if any test succeeds
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.
    glStencilFunc(GL_EQUAL, 1, 0xFF);                // Now we will only draw pixels where the corresponding stencil buffer value equals 1
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureSource.texture);
    mShaderManager->bind(ShaderManager::ShaderType::NozzleBlurShader);
    mShaderManager->setUniformValue("screenTexture", 0);
    mQuad->render();
    mShaderManager->release();
    glDisable(GL_STENCIL_TEST);
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
    mShaderManager->setUniformValue("width", mWidth);
    mShaderManager->setUniformValue("height", mHeight);
    mQuad->render();
    mShaderManager->release();
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
    //glEnable(GL_CLIP_DISTANCE0);

    mShaderManager->bind(ShaderManager::ShaderType::TerrainShader);
    mShaderManager->setUniformValue("VP", mCamera->getVP());
    mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
    mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
    mShaderManager->setUniformValue("directionalLight.color", mSun->color());
    mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
    mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
    mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
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
    mShaderManager->bind(ShaderManager::ShaderType::NozzleEffectShader);
    mShaderManager->setUniformValue("MVP", mCamera->getVP() * mNozzleEffect->worldTransformation());
    mShaderManager->setUniformValue("radius", mNozzleEffect->radius());
    mShaderManager->setUniformValue("velocity", mNozzleEffect->velocity());
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

bool RendererManager::createFramebuffers()
{
    for (int i = 0; i < 2; i++)
    {
        // Creat framebuffer
        glGenFramebuffers(1, &mFramebuffers[i].framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffers[i].framebuffer);

        // Create multisampled texture
        glGenTextures(1, &mFramebuffers[i].texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFramebuffers[i].texture);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, mWidth, mHeight, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mFramebuffers[i].texture, 0);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Create depth buffer
        glGenRenderbuffers(1, &mFramebuffers[i].renderObject);
        glBindRenderbuffer(GL_RENDERBUFFER, mFramebuffers[i].renderObject);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, mWidth, mHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mFramebuffers[i].renderObject);

        // Create depth "texture" in order to implement motion blur
        glGenTextures(1, &mFramebuffers[i].depthTexture);
        glBindTexture(GL_TEXTURE_2D, mFramebuffers[i].depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

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
    for (int i = 0; i < 2; i++)
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
