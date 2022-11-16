#ifndef LINESTRIP_H
#define LINESTRIP_H

#include "Common.h"

#include <QObject>
#include <QVector4D>

namespace Canavar {
namespace Engine {

class LineStrip : public QObject
{
    Q_OBJECT
public:
    explicit LineStrip(QObject *parent = nullptr);

    void appendPoint(const QVector3D &point);
    void removePoint(int index);
    void removeAllPoints();

    const QVector<QVector3D> &points() const;
    QVector<QVector3D> &points_nonConst();

private:
    QVector<QVector3D> mPoints;

    DEFINE_MEMBER(QVector4D, Color);
};

} // namespace Engine
} // namespace Canavar

#endif // LINESTRIP_H
