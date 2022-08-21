#ifndef AIRCRAFTCONTROLLER_H
#define AIRCRAFTCONTROLLER_H

#include "Aircraft.h"
#include "Model.h"

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

    void onKeyPressed(QKeyEvent *);
    void onKeyReleased(QKeyEvent *);

    void drawGui();

    void setJet(Model *newJet);

    void setRootJetNode(Node *newRootJetNode);

signals:
    void command(Aircraft::Command command, QVariant variant = QVariant());

public slots:
    void init();
    void tick();
    void render(float ifps);

private:
    Aircraft *mAircraft;
    QMap<Qt::Key, bool> mPressedKeys;
    QTimer mTimer;

    float mAileron;  // [-1, 1]
    float mElevator; // [-1, 1]
    float mRudder;   // [-1, 1]
    float mThrottle; // [0, 1]

    Aircraft::PrimaryFlightData mPfd;

    Model *mJet;
    Node *mRootJetNode;
};

#endif // AIRCRAFTCONTROLLER_H
