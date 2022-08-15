#ifndef WINDOW_H
#define WINDOW_H

#include "Light.h"
#include "LightManager.h"
#include "RendererManager.h"
#include "Simulator/AircraftController.h"

#include <QOpenGLFunctionsPrivate>
#include <QOpenGLWindow>

class Window : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Window(QWindow *parent = nullptr);

    bool imguiWantCapture() const;

    void setAircraftController(AircraftController *newAircraftController);

public slots:
    void onPfdChanged(Aircraft::PrimaryFlightData pfd);

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
    void command(Aircraft::Command command, QVariant variant = QVariant());

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

    void populateComboBox(Node *node);

private:
    long long mPreviousTime;
    long long mCurrentTime;

    bool mImguiWantCapture;

    RendererManager *mRendererManager;
    LightManager *mLightManager;
    NodeManager *mNodeManager;

    Light *mActiveLight;

    QList<Node *> mNodes;

    Node *mSelectedNode;

    AircraftController *mAircraftController;
    float mAileron;  // [-1, 1]
    float mElevator; // [-1, 1]
    float mRudder;   // [-1, 1]
    float mThrottle; // [0, 1]
    Aircraft::PrimaryFlightData mPfd;
};
#endif // WINDOW_H
