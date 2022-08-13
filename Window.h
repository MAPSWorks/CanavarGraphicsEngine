#ifndef WINDOW_H
#define WINDOW_H

#include "CustomMouseEvent.h"
#include "Light.h"
#include "LightManager.h"
#include "RendererManager.h"

#include <QOpenGLFunctionsPrivate>
#include <QOpenGLWindow>

class Window : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Window(QWindow *parent = nullptr);

    bool imguiWantCapture() const;

signals:
    void init();
    void resized(int w, int h);
    void render(float ifps);
    void keyPressed(QKeyEvent *);
    void keyReleased(QKeyEvent *);
    void mousePressed(CustomMouseEvent);
    void mouseReleased(CustomMouseEvent);
    void mouseMoved(CustomMouseEvent);
    void wheelMoved(QWheelEvent *);
    void mouseDoubleClicked(CustomMouseEvent);
    void mouseCaptured(bool captured);

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
    ModelManager *mModelManager;

    Light *mActiveLight;

    QList<Model *> mModels;

    int mSelectedIndex;
    Model *mSelectedModel;

    bool mMouseCaptured;

    QPoint mMouseGrabPosition;
};
#endif // WINDOW_H
