#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "Converter.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVariant>

#include <QQuaternion>

#include <FGFDMExec.h>
#include <FGJSBBase.h>
#include <initialization/FGInitialCondition.h>
#include <models/FGAuxiliary.h>
#include <models/FGFCS.h>
#include <models/FGPropagate.h>
#include <models/FGPropulsion.h>
#include <models/propulsion/FGThruster.h>

class Aircraft : public QObject
{
    Q_OBJECT
public:
    enum class Command {
        Aileron,
        Elevator,
        Rudder,
        Steering,
        Flaps,
        Speedbrake,
        Spoiler,
        PitchTrim,
        RudderTrim,
        AileronTrim,
        Throttle,
        Mixture,
        Gear,
        PropellerPitch,
        PropellerFeather,
        InitRunning,
        Hold,
        Resume,
    };
    Q_ENUM(Command);

    struct PrimaryFlightData {
        double angleOfAttack;
        double sideSlip;
        double roll;
        double pitch;
        double heading;
        double airspeed;
        double machNumber;
        double climbRate;
        double latitude;
        double longitude;
        double altitude;
        QVector3D position;
        QQuaternion rotation;
        double rudderPos;
        double elevatorPos;
    };

    explicit Aircraft(QObject *parent = nullptr);
    bool init();
    bool holding() const;

public slots:
    void onCommand(Aircraft::Command command, QVariant variant = QVariant());
    void tick();
    void stop();

signals:
    void pfdChanged(Aircraft::PrimaryFlightData);

private:
    JSBSim::FGFDMExec *mExecutor;
    JSBSim::FGFCS *mCommander;
    JSBSim::FGPropagate *mPropagate;
    JSBSim::FGPropulsion *mPropulsion;
    JSBSim::FGAuxiliary *mAuxiliary;
    Converter *mConverter;

    QThread mThread;
    QTimer mTimer;
};

Q_DECLARE_METATYPE(Aircraft::Command);
Q_DECLARE_METATYPE(Aircraft::PrimaryFlightData);

#endif // AIRCRAFT_H
