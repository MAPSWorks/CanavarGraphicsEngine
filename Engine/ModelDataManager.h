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

    enum class RenderMode { //
        Default,
        Instanced
    };

    bool init();
    void render(InternalBasicModel model, RenderMode mode = RenderMode::Default, GLenum primitive = GL_TRIANGLES, QVariant count = QVariant());

    ModelData *getModelData(const QString &modelName);

private:
    void loadModels(const QString &path, const QStringList &formats);
    void loadInternalModels();

private:
    QMap<QString, ModelData *> mModelsData;
    QMap<QString, ModelData *> mInternalModelsData;
    OpenGLVertexArrayObject mQuad;
    OpenGLVertexArrayObject mCube;
};

} // namespace Engine
} // namespace Canavar

#endif // MODELDATAMANAGER_H
