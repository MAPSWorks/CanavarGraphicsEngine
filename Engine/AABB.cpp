#include "AABB.h"

Canavar::Engine::AABB::AABB() {}

QMatrix4x4 Canavar::Engine::AABB::getTransformation() const
{
    QMatrix4x4 result;
    result.translate((mMin + mMax) / 2.0f);
    result.scale(mMax.x() - mMin.x(), mMax.y() - mMin.y(), mMax.z() - mMin.z());
    return result;
}

void Canavar::Engine::AABB::toJson(QJsonObject &object)
{
    QJsonObject aabb;

    QJsonObject min;
    min.insert("x", mMin.x());
    min.insert("y", mMin.y());
    min.insert("z", mMin.z());
    aabb.insert("min", min);

    QJsonObject max;
    max.insert("x", mMax.x());
    max.insert("y", mMax.y());
    max.insert("z", mMax.z());
    aabb.insert("max", max);

    object.insert("aabb", aabb);
}

void Canavar::Engine::AABB::fromJson(const QJsonObject &object)
{
    // Min
    {
        float x = object["min"]["x"].toDouble();
        float y = object["min"]["y"].toDouble();
        float z = object["min"]["z"].toDouble();

        mMin = QVector3D(x, y, z);
    }

    // Max
    {
        float x = object["max"]["x"].toDouble();
        float y = object["max"]["y"].toDouble();
        float z = object["max"]["z"].toDouble();

        mMax = QVector3D(x, y, z);
    }
}
