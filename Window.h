#ifndef WINDOW_H
#define WINDOW_H

#include "LightManager.h"
#include "RendererManager.h"
#include "Simulator/AircraftController.h"

#include <QOpenGLFunctionsPrivate>
#include <QOpenGLWindow>
#include <QRandomGenerator>

class Window : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Window(QWindow *parent = nullptr);
    bool imguiWantCapture() const;

    AircraftController *aircraftController() const;
    void setAircraftController(AircraftController *newAircraftController);

signals:
    void init();
    void resized(int w, int h);
    void render(float ifps);
    void keyPressed(QKeyEvent *);
    void keyReleased(QKeyEvent *);
    void mousePressed(QMouseEvent *);
    void mouseReleased(QMouseEvent *);
    void mouseMoved(QMouseEvent *);
    void wheelMoved(QWheelEvent *);
    void mouseDoubleClicked(QMouseEvent *);

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;

private:
    long long mPreviousTime;
    long long mCurrentTime;

    bool mImguiWantCapture;

    RendererManager *mRendererManager;
    LightManager *mLightManager;
    NodeManager *mNodeManager;

    AircraftController *mAircraftController;
};
#endif // WINDOW_H
