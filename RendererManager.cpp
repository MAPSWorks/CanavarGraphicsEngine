#include "RendererManager.h"
#include "Camera.h"
#include "Helper.h"
#include "Light.h"
#include "TexturedModel.h"

#include <QDir>
#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
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

    qInfo() << Q_FUNC_INFO << "All textured models are loaded.";

    return true;
}

void RendererManager::render(float ifps)
{
    Q_UNUSED(ifps);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCamera = mCameraManager->activeCamera();
    mLight = mLightManager->activeLight();

    mShaderManager->bind(ShaderManager::Shader::BasicShader);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->worldTransformation());
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

    mShaderManager->release();

    mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);

    if (mCamera)
    {
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->worldTransformation());
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

    mShaderManager->release();

    auto nodes = mNodeManager->nodes();
    for (auto node : nodes)
        renderNode(node);
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
            mShaderManager->bind(ShaderManager::Shader::BasicShader);
            mShaderManager->setUniformValue("node.transformation", model->worldTransformation());
            mShaderManager->setUniformValue("node.color", model->material().color());
            mShaderManager->setUniformValue("node.ambient", model->material().ambient());
            mShaderManager->setUniformValue("node.diffuse", model->material().diffuse());
            mShaderManager->setUniformValue("node.specular", model->material().specular());
            mShaderManager->setUniformValue("node.shininess", model->material().shininess());

            data->render();
            mShaderManager->release();
        }
    }

    if (texturedModel)
    {
        TexturedModelData *data = mNameToTexturedModelData.value(texturedModel->modelName(), nullptr);

        if (data)
        {
            mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);
            mShaderManager->setUniformValue("node.transformation", texturedModel->worldTransformation());
            mShaderManager->setUniformValue("node.shininess", texturedModel->shininess());

            data->render();
            mShaderManager->release();
        }
    }

    auto children = node->children();

    for (auto child : children)
        renderNode(child);
}
