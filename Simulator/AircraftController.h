#ifndef AIRCRAFTCONTROLLER_H
#define AIRCRAFTCONTROLLER_H

#include "Aircraft.h"

#include <QKeyEvent>
#include <QObject>
#include <QTimer>

class AircraftController : public QObject
{
    Q_OBJECT
public:
    explicit AircraftController(Aircraft *aircraft, QObject *parent = nullptr);

    void onKeyPressed(QKeyEvent *);
    void onKeyReleased(QKeyEvent *);

    double aileron() const;
    double elevator() const;
    double rudder() const;
    double throttle() const;
    bool holding() const;

signals:
    void command(Aircraft::Command command, QVariant variant = QVariant());

public slots:
    void init();
    void tick();

private:
    Aircraft *mAircraft;
    QMap<Qt::Key, bool> mPressedKeys;
    QTimer mTimer;

    double mAileron;  // [-1, 1]
    double mElevator; // [-1, 1]
    double mRudder;   // [-1, 1]
    double mThrottle; // [0, 1]

    Aircraft::PrimaryFlightData mPfd;
};

#endif // AIRCRAFTCONTROLLER_H
