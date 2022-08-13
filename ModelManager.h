#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "Model.h"
#include "TexturedModel.h"
#include <QObject>

class ModelManager : public QObject
{
    Q_OBJECT

private:
    explicit ModelManager(QObject *parent = nullptr);

public:
    void addModel(Model *model);
    void removeModel(Model *model);

    void addTexturedModel(TexturedModel *model);
    void removeTexturedModel(TexturedModel *model);

    const QList<Model *> &models() const;
    const QList<TexturedModel *> &texturedModel() const;

    static ModelManager *instance();

private:
    QList<Model *> mModels;
    QList<TexturedModel *> mTexturedModel;
};

#endif // MODELMANAGER_H
