#ifndef SKY_H
#define SKY_H

#include "CameraManager.h"
#include "LightManager.h"
#include "Node.h"
#include "ShaderManager.h"

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class Sky : public Node, protected QOpenGLExtraFunctions
{
private:
    friend class Canavar::Engine::NodeManager;

    explicit Sky(QObject *parent = nullptr);
    virtual ~Sky();

    void create();

public:
    void render();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;

    QVector<unsigned int> mVertices;
    unsigned int mVAO;
    unsigned int mVBO;
};

} // namespace Engine
} // namespace Canavar

#endif // SKY_H
