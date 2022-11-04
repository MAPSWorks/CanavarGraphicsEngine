#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Config.h"

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <QWheelEvent>

namespace Canavar {
namespace Engine {

class ModelDataManager;
class RendererManager;
class ShaderManager;
class NodeManager;
class CameraManager;
class LightManager;
class SelectableNodeRenderer;
class Manager;

class Controller : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    bool init(const QString &configFile = "Config.json");
    void mouseDoubleClicked(QMouseEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void wheelMoved(QWheelEvent *event);
    void keyPressed(QKeyEvent *event);
    void keyReleased(QKeyEvent *event);
    void resize(int w, int h);
    void render(float ifps);

    void setWindow(QOpenGLWindow *newWindow);

private:
    Config *mConfig;
    ModelDataManager *mModelDataManager;
    RendererManager *mRendererManager;
    ShaderManager *mShaderManager;
    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    SelectableNodeRenderer *mSelectableNodeRenderer;

    QVector<Manager *> mManagers;

    QOpenGLWindow *mWindow;

    bool mSuccess;
};

} // namespace Engine
} // namespace Canavar

#endif // CONTROLLER_H
