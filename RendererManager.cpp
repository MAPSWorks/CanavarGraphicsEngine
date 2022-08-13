#include "RendererManager.h"
#include "Camera.h"
#include "Helper.h"
#include "Light.h"

#include <QDir>
#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
{
    mModelManager = ModelManager::instance();
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
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glLineWidth(2.5f);

    qInfo() << Q_FUNC_INFO << "Initializing ShaderManager...";

    if (!mShaderManager->init())
    {
        qWarning() << Q_FUNC_INFO << "ShaderManager could not be initialized.";
        return false;
    }

    // Models
    qInfo() << Q_FUNC_INFO << "Loading and creating all models...";

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

    qInfo() << Q_FUNC_INFO << "All models are loaded.";

    // Textured Models
    qInfo() << Q_FUNC_INFO << "Loading and creating all textured models...";

    QDir dir("Resources/Objects");
    auto dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    for (const auto &dirName : qAsConst(dirs))
    {
        TexturedModelData *data = Helper::loadTexturedModel(dirName, //
                                                            "Resources/Objects/" + dirName + "/" + dirName + ".obj");

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

            mNameToTexturedModelData.insert(data->name(), data);
        }
    }

    qInfo() << Q_FUNC_INFO << "All textured models are loaded.";

    return true;
}

void RendererManager::render(float ifps)
{
    Q_UNUSED(ifps);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCamera = mCameraManager->activeCamera();
    mLight = mLightManager->activeLight();

    renderModels(ifps);
    renderTexturedModels(ifps);
}

void RendererManager::renderModels(float ifps)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::BasicShader);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight)
    {
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
    }

    QList<Model *> models = mModelManager->models();

    for (Model *model : qAsConst(models))
    {
        if (!model->visible())
            continue;

        ModelData *data = mTypeToModelData.value(model->type(), nullptr);

        if (data)
        {
            mShaderManager->setUniformValue("node.transformation", model->transformation());
            mShaderManager->setUniformValue("node.color", model->material().color());
            mShaderManager->setUniformValue("node.ambient", model->material().ambient());
            mShaderManager->setUniformValue("node.diffuse", model->material().diffuse());
            mShaderManager->setUniformValue("node.specular", model->material().specular());
            mShaderManager->setUniformValue("node.shininess", model->material().shininess());

            data->render();
        }
    }

    mShaderManager->release();
}

void RendererManager::renderTexturedModels(float ifps)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight)
    {
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
    }

    for (TexturedModel *model : mModelManager->texturedModel())
    {
        TexturedModelData *data = mNameToTexturedModelData.value(model->name(), nullptr);

        if (data)
        {
            mShaderManager->setUniformValue("node.transformation", model->transformation());
            mShaderManager->setUniformValue("node.shininess", model->shininess());

            data->render();
        }
    }

    mShaderManager->release();
}
