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

    // Internal models
    // Quad
    glGenVertexArrays(1, &mQuad.mVAO);
    glBindVertexArray(mQuad.mVAO);
    glGenBuffers(1, &mQuad.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuad.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::QUAD), Canavar::Engine::QUAD, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    // Cube
    glGenVertexArrays(1, &mCube.mVAO);
    glBindVertexArray(mCube.mVAO);
    glGenBuffers(1, &mCube.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mCube.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), CUBE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    return true;
}

void Canavar::Engine::ModelDataManager::render(InternalBasicModel model, RenderMode mode, GLenum primitive, QVariant count)
{
    switch (mode)
    {
    case RenderMode::Default:
        switch (model)
        {
        case InternalBasicModel::Quad:
            glBindVertexArray(mQuad.mVAO);
            glDrawArrays(primitive, 0, 6);
            glBindVertexArray(0);
            break;
        case InternalBasicModel::Cube:
            glBindVertexArray(mCube.mVAO);
            glDrawArrays(primitive, 0, 36);
            glBindVertexArray(0);
            break;
        default:
            qCritical() << Q_FUNC_INFO << "Not implemented yet!";
            break;
        }
        break;
    case RenderMode::Instanced:
        switch (model)
        {
        case InternalBasicModel::Quad:
            glBindVertexArray(mQuad.mVAO);
            glDrawArraysInstanced(primitive, 0, 6, count.toInt());
            glBindVertexArray(0);
            break;
        case InternalBasicModel::Cube:
            glBindVertexArray(mCube.mVAO);
            glDrawArraysInstanced(primitive, 0, 36, count.toInt());
            glBindVertexArray(0);
            break;
        default:
            qCritical() << Q_FUNC_INFO << "Not implemented yet!";
            break;
        }
        break;
    }
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

void Canavar::Engine::ModelDataManager::loadInternalModels() {}
