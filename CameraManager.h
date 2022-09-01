#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "Camera.h"
#include "Manager.h"
#include "NodeManager.h"
#include "PerspectiveCamera.h"

#include <QObject>

class CameraManager : public Manager
{
    Q_OBJECT

private:
    friend class NodeManager;

    explicit CameraManager(QObject *parent = nullptr);

    void addCamera(Camera *camera);
    void removeCamera(Camera *camera);

public:
    bool init() override;

    void mouseDoubleClicked(QMouseEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void wheelMoved(QWheelEvent *event);
    void keyPressed(QKeyEvent *event);
    void keyReleased(QKeyEvent *event);
    void resize(int width, int height);
    void update(float ifps);
    void reset();

    PerspectiveCamera *activeCamera() const;
    void setActiveCamera(PerspectiveCamera *newActiveCamera);

    static CameraManager *instance();

private:
    PerspectiveCamera *mActiveCamera;
    QList<Camera *> mCameras;

    int mWidth;
    int mHeight;
};

#endif // CAMERAMANAGER_H
