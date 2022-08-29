#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "Camera.h"
#include "NodeManager.h"

#include <QObject>

class CameraManager : public QObject
{
    Q_OBJECT

private:
    friend class NodeManager;

    explicit CameraManager(QObject *parent = nullptr);

    void addCamera(Camera *camera);
    void removeCamera(Camera *camera);

public:
    void onMouseDoubleClicked(QMouseEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void onWheelMoved(QWheelEvent *event);
    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void resize(int width, int height);
    void update(float ifps);

    Camera *activeCamera() const;
    void setActiveCamera(Camera *newActiveCamera);

    static CameraManager *instance();

private:
    Camera *mActiveCamera;
    QList<Camera *> mCameras;

    int mWidth;
    int mHeight;
};

#endif // CAMERAMANAGER_H
