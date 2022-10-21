#ifndef AIRCRAFTCONTROLLER_H
#define AIRCRAFTCONTROLLER_H

#include "../Engine/Model.h"
#include "Aircraft.h"

#include <QKeyEvent>
#include <QObject>
#include <QTimer>

#include <imgui.h>
#include <QtImGui.h>

class AircraftController : public QObject
{
    Q_OBJECT
public:
    explicit AircraftController(Aircraft *aircraft, QObject *parent = nullptr);

    void keyPressed(QKeyEvent *);
    void keyReleased(QKeyEvent *);

    void drawGUI();
    void setJet(Canavar::Engine::Model *newJet);
    void setRootJetNode(Canavar::Engine::Node *newRootJetNode);

signals:
    void command(Aircraft::Command command, QVariant variant = QVariant());

public slots:
    bool init();
    void tick();
    void update(float ifps);

private:
    Aircraft *mAircraft;
    QMap<Qt::Key, bool> mPressedKeys;
    QTimer mTimer;

    float mAileron;  // [-1, 1]
    float mElevator; // [-1, 1]
    float mRudder;   // [-1, 1]
    float mThrottle; // [0, 1]

    Aircraft::PrimaryFlightData mPfd;

    Canavar::Engine::Model *mJet;
    Canavar::Engine::Node *mRootJetNode;
};

#endif // AIRCRAFTCONTROLLER_H
