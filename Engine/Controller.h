#ifndef CONTROLLER_H
#define CONTROLLER_H

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
class Manager;

class Controller : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    bool init();
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
    ModelDataManager *mModelDataManager;
    RendererManager *mRendererManager;
    ShaderManager *mShaderManager;
    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;

    QVector<Manager *> mManagers;

    QOpenGLWindow *mWindow;

    bool mSuccess;
};

} // namespace Engine
} // namespace Canavar

#endif // CONTROLLER_H
