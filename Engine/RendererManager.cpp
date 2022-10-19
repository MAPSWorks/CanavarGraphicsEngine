#include "RendererManager.h"
#include "CameraManager.h"
#include "Helper.h"
#include "LightManager.h"
#include "NodeManager.h"
#include "ShaderManager.h"

#include <QDir>

Canavar::Engine::RendererManager::RendererManager(QObject *parent)
    : Manager(parent)
{}

Canavar::Engine::RendererManager *Canavar::Engine::RendererManager::instance()
{
    static RendererManager instance;
    return &instance;
}

bool Canavar::Engine::RendererManager::init()
{
    mNodeManager = NodeManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mShaderManager = ShaderManager::instance();

    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    loadModels();

    return true;
}

void Canavar::Engine::RendererManager::resize(int width, int height) {}

void Canavar::Engine::RendererManager::render(float ifps)
{
    mRenderParameters.ifps = ifps;

    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->sun();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    auto nodes = mNodeManager->nodes();

    for (const auto &node : nodes)
    {
        if (!node->visible())
            continue;

        if (!node->renderable())
            continue;

        if (Model *model = dynamic_cast<Model *>(node))
        {
            mRenderParameters.model = model;

            ModelData *data = mModelsData.value(model->modelName(), nullptr);
            if (data)
                data->render(mRenderParameters);
        }
    }
}

void Canavar::Engine::RendererManager::loadModels()
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
            ModelData *data = Canavar::Engine::Helper::loadModel(dirName, childDir.path() + "/" + file);

            if (data)
            {
                auto meshes = data->meshes();

                for (auto mesh : meshes)
                    mesh->create();

                mModelsData.insert(data->name(), data);
            }
        }
    }

    qInfo() << "All textured models are loaded.";
}

void Canavar::Engine::RendererManager::renderNodes() {}
