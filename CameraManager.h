#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "Camera.h"
#include "CustomMouseEvent.h"

#include <QObject>

class CameraManager : public QObject
{
    Q_OBJECT

private:
    explicit CameraManager(QObject *parent = nullptr);

public:
    void addCamera(Camera *camera);
    void removeCamera(Camera *camera);

    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onMousePressed(CustomMouseEvent event);
    void onMouseReleased(CustomMouseEvent event);
    void onMouseMoved(CustomMouseEvent event);
    void update(float ifps);

    QVector3D getViewDirection();
    QVector3D getDirectionFromScreen(int x, int y, int width, int height);

    Camera *activeCamera() const;
    void setActiveCamera(Camera *newActiveCamera);

    static CameraManager *instance();

private:
    Camera *mActiveCamera;
    QList<Camera *> mCameras;
};

#endif // CAMERAMANAGER_H
