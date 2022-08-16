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

    mSkyBox = new SkyBox;
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "Resources/Sky/1/right.png");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "Resources/Sky/1/left.png");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "Resources/Sky/1/top.png");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "Resources/Sky/1/bottom.png");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "Resources/Sky/1/front.png");
    mSkyBox->setPath(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "Resources/Sky/1/back.png");
    mSkyBox->create();

    return true;
}

void RendererManager::render(float ifps)
{
    Q_UNUSED(ifps);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCamera = mCameraManager->activeCamera();
    mLight = mLightManager->activeLight();

    if (mCamera)
    {
        mShaderManager->bind(ShaderManager::Shader::BasicShader);
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->worldTransformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->worldTransformation());
        mShaderManager->setUniformValue("camera_position", mCamera->position());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::WireframeShader);
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->worldTransformation());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::NormalsShader);
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue("view_matrix", mCamera->worldTransformation());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::SkyBoxShader);
        mShaderManager->setUniformValue("projection_matrix", mCamera->projection());

        QMatrix4x4 transformation = mCamera->worldTransformation();
        transformation.setColumn(3, QVector4D(0, 0, 0, 1));
        mShaderManager->setUniformValue("view_matrix", transformation);
        mShaderManager->release();
    }

    if (mLight)
    {
        mShaderManager->bind(ShaderManager::Shader::BasicShader);
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
        mShaderManager->release();

        mShaderManager->bind(ShaderManager::Shader::TexturedModelShader);
        mShaderManager->setUniformValue("light.position", mLight->position());
        mShaderManager->setUniformValue("light.color", mLight->color());
        mShaderManager->setUniformValue("light.ambient", mLight->ambient());
        mShaderManager->setUniformValue("light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue("light.specular", mLight->specular());
        mShaderManager->release();
    }

    auto nodes = mNodeManager->nodes();

    for (auto node : nodes)
    {
        renderNode(node);
    }

    mShaderManager->bind(ShaderManager::Shader::SkyBoxShader);
    mShaderManager->setUniformValue("skybox", 0);
    mSkyBox->render();
    mShaderManager->release();
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
                mShaderManager->setUniformValue("node.color", model->material().color());
                mShaderManager->setUniformValue("node.ambient", model->material().ambient());
                mShaderManager->setUniformValue("node.diffuse", model->material().diffuse());
                mShaderManager->setUniformValue("node.specular", model->material().specular());
                mShaderManager->setUniformValue("node.shininess", model->material().shininess());
                data->render();
                mShaderManager->release();
            }

            if (mRenderWireframe)
            {
                mShaderManager->bind(ShaderManager::Shader::WireframeShader);
                mShaderManager->setUniformValue("node_matrix", model->worldTransformation());
                data->renderWireframe();
                mShaderManager->release();
            }

            if (mRenderNormals)
            {
                mShaderManager->bind(ShaderManager::Shader::NormalsShader);
                mShaderManager->setUniformValue("node_matrix", model->worldTransformation());
                data->renderNormals();
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
                mShaderManager->setUniformValue("node.transformation", texturedModel->worldTransformation());
                mShaderManager->setUniformValue("node.shininess", texturedModel->shininess());
                data->render();
                mShaderManager->release();
            }

            if (mRenderWireframe)
            {
                mShaderManager->bind(ShaderManager::Shader::WireframeShader);
                mShaderManager->setUniformValue("node_matrix", texturedModel->worldTransformation());
                data->renderWireframe();
                mShaderManager->release();
            }

            if (mRenderNormals)
            {
                mShaderManager->bind(ShaderManager::Shader::NormalsShader);
                mShaderManager->setUniformValue("node_matrix", texturedModel->worldTransformation());
                data->renderNormals();
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
