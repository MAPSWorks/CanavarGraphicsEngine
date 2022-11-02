#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "Camera.h"
#include "Manager.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class CameraManager : public Manager
{
    Q_OBJECT

private:
    friend class Canavar::Engine::NodeManager;

    CameraManager();

    void addCamera(Camera *camera);
    void removeCamera(Camera *camera);

public:
    static CameraManager *instance();

    bool init() override;
    void mouseDoubleClicked(QMouseEvent *event) override;
    void wheelMoved(QWheelEvent *event) override;
    void mousePressed(QMouseEvent *event) override;
    void mouseReleased(QMouseEvent *event) override;
    void mouseMoved(QMouseEvent *event) override;
    void keyPressed(QKeyEvent *event) override;
    void keyReleased(QKeyEvent *event) override;
    void resize(int width, int height) override;
    void update(float ifps) override;
    void reset() override;

    Camera *activeCamera() const;
    void setActiveCamera(Camera *newActiveCamera);

private:
    Camera *mActiveCamera;
    QList<Camera *> mCameras;

    int mWidth;
    int mHeight;
};
} // namespace Engine
} // namespace Canavar

#endif // CAMERAMANAGER_H
