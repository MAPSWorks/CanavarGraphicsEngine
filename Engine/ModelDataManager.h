#ifndef MODELDATAMANAGER_H
#define MODELDATAMANAGER_H

#include "Manager.h"
#include "ModelData.h"
#include "OpenGLVertexArrayObject.h"

#include <QMap>

namespace Canavar {
namespace Engine {

class ModelDataManager : public Manager, protected QOpenGLExtraFunctions
{
private:
    explicit ModelDataManager(QObject *parent = nullptr);

public:
    static ModelDataManager *instance();

    enum class InternalBasicModel { //
        Quad,
        Cube
    };

    bool init();

    ModelData *getModelData(const QString &modelName);

    OpenGLVertexArrayObject mQuad;
    OpenGLVertexArrayObject mCube;

private:
    void loadModels(const QString &path, const QStringList &formats);

private:
    QMap<QString, ModelData *> mModelsData;
};

} // namespace Engine
} // namespace Canavar

#endif // MODELDATAMANAGER_H
