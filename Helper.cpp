#include "Helper.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuaternion>
#include <QTextStream>
#include <QtMath>

#include <Dense>

Helper::Helper() {}

QByteArray Helper::getBytes(QString path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        return file.readAll();
    } else
    {
        qWarning() << QString("Could not open '%1'").arg(path);
        return QByteArray();
    }
}

QQuaternion Helper::rotateX(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateY(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateZ(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), qRadiansToDegrees(angleRadians));
}

QVector3D Helper::projectOntoPlane(const QVector3D &normal, const QVector3D &point, const QVector3D &subject)
{
    Eigen::Vector3f normalEigen = Eigen::Vector3f(normal.x(), normal.y(), normal.z());
    Eigen::Vector3f pointEigen = Eigen::Vector3f(point.x(), point.y(), point.z());
    Eigen::Vector3f subjectEigen = Eigen::Vector3f(subject.x(), subject.y(), subject.z());
    Eigen::Hyperplane<float, 3> plane = Eigen::Hyperplane<float, 3>(normalEigen, -normalEigen.dot(pointEigen));
    Eigen::Vector3f projection = plane.projection(subjectEigen);

    return QVector3D(projection.x(), projection.y(), projection.z());
}
