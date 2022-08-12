#ifndef HELPER_H
#define HELPER_H

#include <QQuaternion>
#include <QString>

class Helper
{
private:
    Helper();

public:
    static QByteArray getBytes(QString path);
    static QQuaternion rotateX(float angleRadians);
    static QQuaternion rotateY(float angleRadians);
    static QQuaternion rotateZ(float angleRadians);
    static QVector3D projectOntoPlane(const QVector3D &normal, const QVector3D &point, const QVector3D &subject);
};

#endif // HELPER_H
