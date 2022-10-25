#ifndef MODELDATAMANAGER_H
#define MODELDATAMANAGER_H

#include "Manager.h"
#include "ModelData.h"

#include <QMap>

namespace Canavar {
namespace Engine {

class ModelDataManager : public Manager, protected QOpenGLExtraFunctions
{
private:
    explicit ModelDataManager(QObject *parent = nullptr);

public:
    static ModelDataManager *instance();

    bool init();
    void renderQuad();

    ModelData *getModelData(const QString &modelName);

private:
    void loadModels(const QString &path, const QStringList &formats);

private:
    QMap<QString, ModelData *> mModelsData;
    unsigned int mQuadVAO;
    unsigned int mQuadVBO;
};

} // namespace Engine
} // namespace Canavar

#endif // MODELDATAMANAGER_H
