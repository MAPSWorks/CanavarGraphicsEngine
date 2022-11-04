#include "ModelDataManager.h"
#include "Helper.h"

#include <QDir>

Canavar::Engine::ModelDataManager::ModelDataManager()
    : Manager()
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
                             << "*.gltf"
                             << "*.fbx");

    mModelNames = mModelsData.keys();

    return true;
}

Canavar::Engine::ModelData *Canavar::Engine::ModelDataManager::getModelData(const QString &modelName)
{
    return mModelsData.value(modelName, nullptr);
}

const QStringList &Canavar::Engine::ModelDataManager::getModelNames() const
{
    return mModelNames;
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

                qInfo() << "Model" << dirName << "at" << path << "is loaded.";
            } else
                qWarning() << "Model" << dirName << "at" << path << "could not be loaded.";
        }
    }

    qInfo() << "All models are loaded at" << path;
}
