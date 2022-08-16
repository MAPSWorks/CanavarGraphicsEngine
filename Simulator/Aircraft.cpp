#include "Aircraft.h"

#include <QDebug>
#include <QDir>
#include <QtMath>

Aircraft::Aircraft(QObject *parent)
    : QObject(parent)
{}

bool Aircraft::init()
{
    QString currentPath = QDir::currentPath();
    QDir::setCurrent("Resources/Data");

    mExecutor = new JSBSim::FGFDMExec;
    mExecutor->SetDebugLevel(0);
    mExecutor->DisableOutput();

    qInfo() << "Aircraft is being initializing...";

    if (!mExecutor->LoadModel("f16"))
    {
        qCritical() << "Could not load JSBSim model.";
        return false;
    }

    if (!mExecutor->GetIC()->Load(SGPath("reset00.xml")))
    {
        qCritical() << "Could not load JSBSim initial conditions.";
        return false;
    }

    if (!mExecutor->RunIC())
    {
        qCritical() << "Could not run JSBSim initial conditions.";
        return false;
    }

    mCommander = mExecutor->GetFCS();
    mPropagate = mExecutor->GetPropagate();
    mPropulsion = mExecutor->GetPropulsion();
    mAuxiliary = mExecutor->GetAuxiliary();
    mExecutor->Setdt(0.01);

    double latitude = mExecutor->GetIC()->GetLatitudeDegIC();
    double longitude = mExecutor->GetIC()->GetLongitudeDegIC();

    mConverter = new Converter(latitude, longitude, 0.0);

    moveToThread(&mThread);
    mThread.start();

    connect(&mTimer, &QTimer::timeout, this, &Aircraft::tick, Qt::QueuedConnection);
    mTimer.start(10);

    QDir::setCurrent(currentPath);

    qInfo() << "Aircraft is initialized.";

    return true;
}

void Aircraft::onCommand(Aircraft::Command command, QVariant variant)
{
    switch (command)
    {
    case Command::InitRunning:
        mPropulsion->InitRunning(variant.toInt());
        break;
    case Command::Hold:
        mExecutor->Hold();
        break;
    case Command::Resume:
        mExecutor->Resume();
        break;
    case Command::Aileron:
        mCommander->SetDaCmd(variant.toDouble());
        break;
    case Command::Elevator:
        mCommander->SetDeCmd(variant.toDouble());
        break;
    case Command::Rudder:
        mCommander->SetDrCmd(variant.toDouble());
        break;
    case Command::Steering:
        mCommander->SetDsCmd(variant.toDouble());
        break;
    case Command::Flaps:
        mCommander->SetDfCmd(variant.toDouble());
        break;
    case Command::Speedbrake:
        mCommander->SetDsbCmd(variant.toDouble());
        break;
    case Command::Spoiler:
        mCommander->SetDspCmd(variant.toDouble());
        break;
    case Command::PitchTrim:
        mCommander->SetPitchTrimCmd(variant.toDouble());
        break;
    case Command::RudderTrim:
        mCommander->SetYawTrimCmd(variant.toDouble());
        break;
    case Command::AileronTrim:
        mCommander->SetRollTrimCmd(variant.toDouble());
        break;
    case Command::Throttle:
        mCommander->SetThrottleCmd(-1, variant.toDouble());
        break;
    case Command::Mixture:
        mCommander->SetMixtureCmd(-1, variant.toDouble());
        break;
    case Command::Gear:
        mCommander->SetGearCmd(variant.toDouble());
        break;
    case Command::PropellerPitch:
        mCommander->SetPropAdvanceCmd(-1, variant.toDouble());
        break;
    case Command::PropellerFeather:
        mCommander->SetFeatherCmd(-1, variant.toDouble());
        break;
    }
}

void Aircraft::tick()
{
    mPropagate->SetTerrainElevation(0);

    mExecutor->Run();

    PrimaryFlightData pfd;

    pfd.angleOfAttack = mAuxiliary->Getalpha();
    pfd.sideSlip = mAuxiliary->Getbeta();
    pfd.climbRate = mPropagate->Gethdot();

    pfd.roll = mPropagate->GetEulerDeg(1);
    pfd.pitch = mPropagate->GetEulerDeg(2);
    pfd.heading = mPropagate->GetEulerDeg(3);

    pfd.airspeed = mAuxiliary->GetVcalibratedKTS();
    pfd.machNumber = mAuxiliary->GetMach();

    pfd.latitude = mPropagate->GetLatitudeDeg();
    pfd.longitude = mPropagate->GetLongitudeDeg();
    pfd.altitude = 0.3048 * mPropagate->GetGeodeticAltitude();

    JSBSim::FGQuaternion rotation = mPropagate->GetQuaternion();
    QQuaternion localToBody = QQuaternion(rotation(1), rotation(2), rotation(3), rotation(4));

    pfd.rotation = mConverter->toOpenGL(pfd.latitude, pfd.longitude, localToBody);
    pfd.position = mConverter->toOpenGL(pfd.latitude, pfd.longitude, pfd.altitude);

    emit pfdChanged(pfd);
}

void Aircraft::stop()
{
    qInfo() << thread() << "Stopping Aircraft...";

    mTimer.stop();

    qInfo() << thread() << "Aircraft is stopped.";
}

bool Aircraft::holding() const
{
    return mExecutor->Holding();
}
