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
{
    mNodeManager = NodeManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mShaderManager = ShaderManager::instance();

    resetFog();
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

    mTerrain = Terrain::instance();
    mTerrain->create();

    mScreenRenderer = new ScreenRenderer;
    mScreenRenderer->create();

    // Create first pass framebuffer
    {
        glGenFramebuffers(1, &mFirstPassFramebuffer.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFirstPassFramebuffer.framebuffer);

        // Create multisampled texture
        glGenTextures(1, &mFirstPassFramebuffer.texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFirstPassFramebuffer.texture);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, mWindowWidth, mWindowHeight, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mFirstPassFramebuffer.texture, 0);

        // Create depth buffer
        glGenRenderbuffers(1, &mFirstPassFramebuffer.renderObject);
        glBindRenderbuffer(GL_RENDERBUFFER, mFirstPassFramebuffer.renderObject);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mFirstPassFramebuffer.renderObject);
    }

    // Create second pass framebuffer
    {
        glGenFramebuffers(1, &mFinalFramebuffer.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFinalFramebuffer.framebuffer);

        // Create multisampled texture
        glGenTextures(1, &mFinalFramebuffer.texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFinalFramebuffer.texture);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, mWindowWidth, mWindowHeight, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mFinalFramebuffer.texture, 0);

        // Create depth buffer
        glGenRenderbuffers(1, &mFinalFramebuffer.renderObject);
        glBindRenderbuffer(GL_RENDERBUFFER, mFinalFramebuffer.renderObject);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mFinalFramebuffer.renderObject);
    }

    return true;
}

void RendererManager::resize(int w, int h)
{
    mWindowWidth = w;
    mWindowHeight = h;

    glDeleteFramebuffers(1, &mFirstPassFramebuffer.framebuffer);
    glDeleteTextures(1, &mFirstPassFramebuffer.texture);
    glDeleteBuffers(1, &mFirstPassFramebuffer.renderObject);

    glDeleteFramebuffers(1, &mFinalFramebuffer.framebuffer);
    glDeleteTextures(1, &mFinalFramebuffer.texture);
    glDeleteBuffers(1, &mFinalFramebuffer.renderObject);

    // Create first pass framebuffer
    {
        glGenFramebuffers(1, &mFirstPassFramebuffer.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFirstPassFramebuffer.framebuffer);

        // Create multisampled texture
        glGenTextures(1, &mFirstPassFramebuffer.texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFirstPassFramebuffer.texture);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, mWindowWidth, mWindowHeight, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mFirstPassFramebuffer.texture, 0);

        // Create depth buffer
        glGenRenderbuffers(1, &mFirstPassFramebuffer.renderObject);
        glBindRenderbuffer(GL_RENDERBUFFER, mFirstPassFramebuffer.renderObject);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mFirstPassFramebuffer.renderObject);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            qCritical() << "Framebuffer is could not be created.";
    }

    // Create second pass framebuffer
    {
        glGenFramebuffers(1, &mFinalFramebuffer.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFinalFramebuffer.framebuffer);

        // Create multisampled texture
        glGenTextures(1, &mFinalFramebuffer.texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFinalFramebuffer.texture);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, mWindowWidth, mWindowHeight, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mFinalFramebuffer.texture, 0);

        // Create depth buffer
        glGenRenderbuffers(1, &mFinalFramebuffer.renderObject);
        glBindRenderbuffer(GL_RENDERBUFFER, mFinalFramebuffer.renderObject);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mFinalFramebuffer.renderObject);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            qCritical() << "Framebuffer is could not be created.";
    }
}

void RendererManager::render(float ifps)
{
    // First pass
    glBindFramebuffer(GL_FRAMEBUFFER, mFirstPassFramebuffer.framebuffer);
    glClearColor(mFog.color.x(), mFog.color.y(), mFog.color.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    renderModels(ifps);
    renderTerrain();

    mShaderManager->bind(ShaderManager::Shader::NozzleParticlesShader);
    mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
    mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
    mShaderManager->setUniformValue("nodeMatrix", mNozzleEffect->getParticlesWorldTransformation());
    mShaderManager->setUniformValue("radius", mNozzleEffect->radius());
    mNozzleEffect->renderParticles();
    mShaderManager->release();

    // ---- First pass is done ----

    // Second pass
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFinalFramebuffer.framebuffer);
    glStencilMask(0x00);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    renderModels(ifps);
    renderTerrain();
    mShaderManager->bind(ShaderManager::Shader::NozzleParticlesShader);
    mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
    mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
    mShaderManager->setUniformValue("modelMatrix", mNozzleEffect->getParticlesWorldTransformation());
    mNozzleEffect->renderParticles();
    mShaderManager->release();

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    mNozzleEffect->renderBlurEffect();

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

    mShaderManager->bind(ShaderManager::Shader::NozzleEffectShader);
    mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
    mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
    mShaderManager->setUniformValue("modelMatrix", mNozzleEffect->worldTransformation());
    mShaderManager->setUniformValue("firstPassTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFirstPassFramebuffer.texture);
    mNozzleEffect->renderBlurEffect();
    mShaderManager->release();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mFinalFramebuffer.texture);
    mShaderManager->bind(ShaderManager::Shader::ScreenShader);
    mShaderManager->setUniformValue("screenTexture", 0);
    mShaderManager->setUniformValue("windowWidth", mWindowWidth);
    mShaderManager->setUniformValue("windowHeight", mWindowHeight);
    mScreenRenderer->render();
    mShaderManager->release();

    glDisable(GL_STENCIL_TEST);
}

void RendererManager::renderModels(float ifps)
{
    Q_UNUSED(ifps);

    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();

    if (mCamera)
    {
        mShaderManager->bind(ShaderManager::Shader::ModelShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::WireframeShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::NormalsShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::SkyBoxShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());

        QMatrix4x4 transformation = mCamera->worldTransformation();
        transformation.setColumn(3, QVector4D(0, 0, 0, 1));
        mShaderManager->setUniformValue("viewMatrix", transformation);
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::TerrainShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
        mShaderManager->release();
    }

    if (mSun)
    {
        mShaderManager->bind(ShaderManager::Shader::ModelShader);
        mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
        mShaderManager->setUniformValue("directionalLight.color", mSun->color());
        mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
        mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
        mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::TerrainShader);
        mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
        mShaderManager->setUniformValue("directionalLight.color", mSun->color());
        mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
        mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
        mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
        mShaderManager->release();
    }

    // Fog
    {
        mShaderManager->bind(ShaderManager::Shader::ModelShader);
        mShaderManager->setUniformValue("fog.enabled", mFog.enabled);
        mShaderManager->setUniformValue("fog.color", mFog.color);
        mShaderManager->setUniformValue("fog.density", mFog.density);
        mShaderManager->setUniformValue("fog.gradient", mFog.gradient);
        mShaderManager->setUniformValue("useBlinnShading", mUseBlinnShading);
        mShaderManager->release();
    }

    auto nodes = mNodeManager->nodes();

    for (const auto &node : nodes)
    {
        Model *model = dynamic_cast<Model *>(node);

        if (model)
            renderModel(model);
    }
}

void RendererManager::renderSkyBox()
{
    mShaderManager->bind(ShaderManager::Shader::SkyBoxShader);
    mShaderManager->setUniformValue("skybox", 0);
    mSkyBox->render();
    mShaderManager->release();
}

void RendererManager::renderTerrain()
{
    //glEnable(GL_CLIP_DISTANCE0);

    mShaderManager->bind(ShaderManager::Shader::TerrainShader);
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
    mShaderManager->setUniformValue("fog.enabled", mFog.enabled);
    mShaderManager->setUniformValue("fog.color", mFog.color);
    mShaderManager->setUniformValue("fog.density", mFog.density);
    mShaderManager->setUniformValue("fog.gradient", mFog.gradient);
    mShaderManager->setUniformValue("waterHeight", 1.0f);
    mTerrain->render();
    mShaderManager->release();

    //glDisable(GL_CLIP_DISTANCE0);
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
            mShaderManager->bind(ShaderManager::Shader::ModelShader);

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
            mShaderManager->bind(ShaderManager::Shader::WireframeShader);
            mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
            data->render(GL_TRIANGLES);
            mShaderManager->release();
        }

        if (mRenderNormals)
        {
            mShaderManager->bind(ShaderManager::Shader::NormalsShader);
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

NozzleEffect *RendererManager::nozzleEffect() const
{
    return mNozzleEffect;
}

void RendererManager::setNozzleEffect(NozzleEffect *newNozzleEffect)
{
    mNozzleEffect = newNozzleEffect;
}

bool RendererManager::useBlinnShading() const
{
    return mUseBlinnShading;
}

void RendererManager::setUseBlinnShading(bool newUseBlinnShading)
{
    mUseBlinnShading = newUseBlinnShading;
}

ModelData *RendererManager::getModelData(const QString &modelName)
{
    return mModelsData.value(modelName, nullptr);
}

const RendererManager::Fog &RendererManager::fog() const
{
    return mFog;
}

void RendererManager::setFog(const Fog &newFog)
{
    mFog = newFog;
}

void RendererManager::resetFog()
{
    mFog.enabled = true;
    mFog.color = QVector3D(0.33f, 0.38f, 0.47f);
    mFog.density = 1.0f;
    mFog.gradient = 1.5f;
}

bool RendererManager::renderObjects() const
{
    return mRenderObjects;
}

void RendererManager::setRenderObjects(bool newRenderObjects)
{
    mRenderObjects = newRenderObjects;
}

bool RendererManager::renderNormals() const
{
    return mRenderNormals;
}

void RendererManager::setRenderNormals(bool newRenderNormals)
{
    mRenderNormals = newRenderNormals;
}

bool RendererManager::renderWireframe() const
{
    return mRenderWireframe;
}

void RendererManager::setRenderWireframe(bool newRenderWireframe)
{
    mRenderWireframe = newRenderWireframe;
}
