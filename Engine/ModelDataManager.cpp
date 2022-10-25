#include "ModelDataManager.h"
#include "Common.h"
#include "Helper.h"

#include <QDir>

Canavar::Engine::ModelDataManager::ModelDataManager(QObject *parent)
    : Manager(parent)
{}

Canavar::Engine::ModelDataManager *Canavar::Engine::ModelDataManager::instance()
{
    static ModelDataManager instance;

    return &instance;
}

bool Canavar::Engine::ModelDataManager::init()
{
    // TODO: Read these paths from .json file

    loadModels("Resources/Models",
               QStringList() << "*.obj"
                             << "*.blend"
                             << "*.dae"
                             << "*.glb"
                             << "*.gltf");

    loadModels("Resources/Models", QStringList() << "*.fbx");

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mQuadVAO);
    glBindVertexArray(mQuadVAO);

    glGenBuffers(1, &mQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::QUAD), Canavar::Engine::QUAD, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    return true;
}

void Canavar::Engine::ModelDataManager::renderQuad()
{
    glBindVertexArray(mQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

Canavar::Engine::ModelData *Canavar::Engine::ModelDataManager::getModelData(const QString &modelName)
{
    return mModelsData.value(modelName, nullptr);
}

void Canavar::Engine::ModelDataManager::loadModels(const QString &path, const QStringList &formats)
{
    qInfo() << "Loading and creating all models at" << path << "whose extensions are" << formats;

    QDir dir(path);
    auto dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    for (const auto &dirName : qAsConst(dirs))
    {
        QDir childDir(dir.path() + "/" + dirName);
        childDir.setNameFilters(formats);
        auto files = childDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto &file : qAsConst(files))
        {
            auto path = childDir.path() + "/" + file;

            qInfo() << "Loading model" << dirName << "at" << path;

            ModelData *data = Canavar::Engine::Helper::loadModel(dirName, path);

            if (data)
            {
                auto meshes = data->meshes();

                for (auto mesh : meshes)
                    mesh->create();

                mModelsData.insert(data->name(), data);

                qInfo() << "Model" << dirName << "at" << path << "is loadded.";
            } else
                qWarning() << "Model" << dirName << "at" << path << "could not be loadded.";
        }
    }

    qInfo() << "All models are loaded at" << path;
}
