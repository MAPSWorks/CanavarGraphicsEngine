#ifndef MODELDATAMANAGER_H
#define MODELDATAMANAGER_H

#include "Manager.h"
#include "ModelData.h"

#include <QMap>

namespace Canavar {
namespace Engine {

class ModelDataManager : public Manager
{
private:
    ModelDataManager();

public:
    static ModelDataManager *instance();

    bool init();

    ModelData *getModelData(const QString &modelName);
    const QStringList &getModelNames() const;

private:
    void loadModels(const QString &path, const QStringList &formats);

private:
    QMap<QString, ModelData *> mModelsData;
    QStringList mModelNames;
};

} // namespace Engine
} // namespace Canavar

#endif // MODELDATAMANAGER_H
