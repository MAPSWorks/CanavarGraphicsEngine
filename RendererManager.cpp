#include "RendererManager.h"
#include "Camera.h"
#include "Helper.h"
#include "Light.h"

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

    qInfo() << Q_FUNC_INFO << "Loading and creating all models...";

    for (Model::Type type : Model::ALL_MODEL_TYPES)
    {
        ModelData *data = new ModelData(type);
        if (!data->load() || !data->create())
        {
            data->deleteLater();
            continue;
        }

        mTypeToModelData.insert(type, data);
    }

    qInfo() << Q_FUNC_INFO << "All models are loaded.";

    // FIXME
    qInfo() << Q_FUNC_INFO << "Loading and creating all textured models...";
    TexturedModelData *backpack = Helper::loadTexturedModel("backpack/backpack.obj");
    mPathToTexturedModelData.insert(backpack->path(), backpack);

    // Create Texture Model Data
    auto texturedModelData = mPathToTexturedModelData.values();

    for (auto data : qAsConst(texturedModelData))
    {
        auto meshes = data->meshes();

        for (auto mesh : meshes)
            mesh->create();
    }

    qInfo() << Q_FUNC_INFO << "All textured models are loaded.";

    TexturedModel *backpackModel = new TexturedModel;
    backpackModel->setPosition(QVector3D(-15, 15, 0));
    backpackModel->setPath("backpack/backpack.obj");
    mModelManager->addTexturedModel(backpackModel);

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

    mShaderManager->bind(ShaderManager::Shader::Basic);

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
            data->bind();

            mShaderManager->setUniformValue("node.transformation", model->transformation());
            mShaderManager->setUniformValue("node.color", model->material().color());
            mShaderManager->setUniformValue("node.ambient", model->material().ambient());
            mShaderManager->setUniformValue("node.diffuse", model->material().diffuse());
            mShaderManager->setUniformValue("node.specular", model->material().specular());
            mShaderManager->setUniformValue("node.shininess", model->material().shininess());

            glDrawArrays(GL_TRIANGLES, 0, data->count());

            data->release();
        }
    }

    mShaderManager->release();
}

void RendererManager::renderTexturedModels(float ifps)
{
    Q_UNUSED(ifps);

    mShaderManager->bind(ShaderManager::Shader::Basic);

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
        TexturedModelData *data = mPathToTexturedModelData.value(model->path(), nullptr);

        if (data)
        {
            mShaderManager->setUniformValue("node.transformation", model->transformation());
            mShaderManager->setUniformValue("node.color", QVector4D(1, 1, 1, 1));
            mShaderManager->setUniformValue("node.ambient", 0.25f);
            mShaderManager->setUniformValue("node.diffuse", 0.75f);
            mShaderManager->setUniformValue("node.specular", 0.25f);
            mShaderManager->setUniformValue("node.shininess", 32.0f);

            auto meshes = data->meshes();

            for (auto mesh : meshes)
            {
                mesh->bind();
                renderTexturedModelDataNode(data, data->rootNode());
                mesh->release();
            }
        }
    }

    mShaderManager->release();
}

void RendererManager::renderTexturedModelDataNode(TexturedModelData *data, TexturedModelDataNode *node)
{
    auto meshes = data->meshes();
    auto meshIndices = node->meshIndices();

    for (auto index : meshIndices)
    {
        meshes[index]->bind();
        glDrawElements(GL_TRIANGLES, meshes[index]->count(), GL_UNSIGNED_INT, 0);
        meshes[index]->release();
    }

    auto children = node->children();

    for (int i = 0; i < children.size(); i++)
        renderTexturedModelDataNode(data, children[i]);
}
