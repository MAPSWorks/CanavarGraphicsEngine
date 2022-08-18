#include "RendererManager.h"
#include "Camera.h"
#include "Helper.h"
#include "Light.h"
#include "TexturedModel.h"

#include <QDir>
#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mRenderObjects(true)
    , mRenderWireframe(false)
    , mRenderNormals(false)
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

    qInfo() << "Initializing ShaderManager...";

    if (!mShaderManager->init())
    {
        qWarning() << Q_FUNC_INFO << "ShaderManager could not be initialized.";
        return false;
    }

    // Models
    qInfo() << "Loading and creating all models...";

    for (Model::Type type : Model::ALL_MODEL_TYPES)
    {
        ModelData *data = Helper::loadModel(type, ModelData::MODEL_TO_PATH[type]);

        if (data)
        {
            if (!data->create())
            {
                data->deleteLater();
                continue;
            }
        }

        mTypeToModelData.insert(type, data);
    }

    qInfo() << "All models are loaded.";

    // Textured Models
    qInfo() << "Loading and creating all textured models...";

    QDir dir("Resources/Objects");
    auto dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList extensions;
    extensions << "*.obj"
               << "*.blend"
               << "*.dae";

    for (const auto &dirName : qAsConst(dirs))
    {
        QDir childDir(dir.path() + "/" + dirName);
        childDir.setNameFilters(extensions);
        auto files = childDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto &file : qAsConst(files))
        {
            TexturedModelData *data = Helper::loadTexturedModel(dirName, childDir.path() + "/" + file);

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

                mNameToTexturedModelData.insert(data->modelName(), data);
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

    mSkyBox = new SkyBox;
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "Resources/Sky/2/right.bmp");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "Resources/Sky/2/left.bmp");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "Resources/Sky/2/top.bmp");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "Resources/Sky/2/bottom.bmp");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "Resources/Sky/2/front.bmp");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "Resources/Sky/2/back.bmp");
    mSkyBox->create();

    mTerrain = Terrain::instance();
    mTerrain->create();

    return true;
}

void RendererManager::render(float ifps)
{
    Q_UNUSED(ifps);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(mFog.color.x(), mFog.color.y(), mFog.color.z(), 1.0f);

    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();

    if (mCamera)
    {
        mShaderManager->bind(ShaderManager::Shader::BasicShader);
        mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
        mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
        mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);
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
        mShaderManager->bind(ShaderManager::Shader::BasicShader);
        mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
        mShaderManager->setUniformValue("directionalLight.color", mSun->color());
        mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
        mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
        mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);
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

    renderTerrain();

    auto nodes = mNodeManager->nodes();

    for (auto node : nodes)
    {
        renderNode(node);
    }

    renderSkyBox();
}

void RendererManager::renderNode(Node *node)
{
    Model *model = dynamic_cast<Model *>(node);
    TexturedModel *texturedModel = dynamic_cast<TexturedModel *>(node);

    if (model)
    {
        ModelData *data = mTypeToModelData.value(model->type(), nullptr);

        if (data)
        {
            if (mRenderObjects)
            {
                mShaderManager->bind(ShaderManager::Shader::BasicShader);
                mShaderManager->setUniformValue("node.transformation", model->worldTransformation());
                mShaderManager->setUniformValue("node.color", model->material().color);
                mShaderManager->setUniformValue("node.ambient", model->material().ambient);
                mShaderManager->setUniformValue("node.diffuse", model->material().diffuse);
                mShaderManager->setUniformValue("node.specular", model->material().specular);
                mShaderManager->setUniformValue("node.shininess", model->material().shininess);

                QVector<PointLight *> pointLights = Helper::getClosePointLights(mLightManager->pointLights(), node);

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

                QVector<SpotLight *> spotLights = Helper::getCloseSpotLights(mLightManager->spotLights(), node);

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

                data->render();
                mShaderManager->release();
            }

            if (mRenderWireframe)
            {
                mShaderManager->bind(ShaderManager::Shader::WireframeShader);
                mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
                data->render();
                mShaderManager->release();
            }

            if (mRenderNormals)
            {
                mShaderManager->bind(ShaderManager::Shader::NormalsShader);
                mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
                data->render();
                mShaderManager->release();
            }
        }
    }

    if (texturedModel)
    {
        TexturedModelData *data = mNameToTexturedModelData.value(texturedModel->modelName(), nullptr);

        if (data)
        {
            if (mRenderObjects)
            {
                mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);
                mShaderManager->setUniformValue("nodeMatrix", texturedModel->worldTransformation());
                mShaderManager->setUniformValue("nodeShininess", texturedModel->shininess());

                QVector<PointLight *> pointLights = Helper::getClosePointLights(mLightManager->pointLights(), node);

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

                QVector<SpotLight *> spotLights = Helper::getCloseSpotLights(mLightManager->spotLights(), node);

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

                data->render();
                mShaderManager->release();
            }

            if (mRenderWireframe)
            {
                mShaderManager->bind(ShaderManager::Shader::WireframeShader);
                mShaderManager->setUniformValue("nodeMatrix", texturedModel->worldTransformation());
                data->render();
                mShaderManager->release();
            }

            if (mRenderNormals)
            {
                mShaderManager->bind(ShaderManager::Shader::NormalsShader);
                mShaderManager->setUniformValue("nodeMatrix", texturedModel->worldTransformation());
                data->render();
                mShaderManager->release();
            }
        }
    }

    if (node)
    {
        auto children = node->children();

        for (auto child : children)
            renderNode(child);
    }
}

void RendererManager::renderSkyBox()
{
    //    mShaderManager->bind(ShaderManager::Shader::SkyBoxShader);
    //    mShaderManager->setUniformValue("skybox", 0);
    //    mSkyBox->render();
    //    mShaderManager->release();
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
