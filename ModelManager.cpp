#include "ModelManager.h"

ModelManager::ModelManager(QObject *parent)
    : QObject(parent)
{}

void ModelManager::addModel(Model *model)
{
    mModels << model;
}

void ModelManager::removeModel(Model *model)
{
    if (model)
    {
        mModels.removeAll(model);
        model->deleteLater();
    }
}

void ModelManager::addTexturedModel(TexturedModel *model)
{
    mTexturedModel << model;
}

void ModelManager::removeTexturedModel(TexturedModel *model)
{
    if (model)
    {
        mTexturedModel.removeAll(model);
        model->deleteLater();
    }
}

const QList<Model *> &ModelManager::models() const
{
    return mModels;
}

ModelManager *ModelManager::instance()
{
    static ModelManager instance;
    return &instance;
}

const QList<TexturedModel *> &ModelManager::texturedModel() const
{
    return mTexturedModel;
}
