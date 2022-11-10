#include "Sun.h"

Canavar::Engine::Sun::Sun()
    : Light()
    , mEnabled(true)
    , mDirection(0, -1, 0)
{
    mAmbient = 1.0f;
    mDiffuse = 1.0f;
    mSpecular = 1.0f;

    float inf = std::numeric_limits<float>::infinity();
    mPosition = QVector3D(inf, inf, inf);

    mType = Node::NodeType::Sun;
    mName = "Sun";
    mSelectable = false;
}

Canavar::Engine::Sun *Canavar::Engine::Sun::instance()
{
    static Sun instance;

    return &instance;
}

void Canavar::Engine::Sun::toJson(QJsonObject &object)
{
    Light::toJson(object);

    QJsonObject direction;
    direction.insert("x", mDirection.x());
    direction.insert("y", mDirection.y());
    direction.insert("z", mDirection.z());
    object.insert("direction", direction);
    object.insert("enabled", mEnabled);
}

void Canavar::Engine::Sun::fromJson(const QJsonObject &object)
{
    Light::fromJson(object);

    // Direction
    float x = object["direction"]["x"].toDouble();
    float y = object["direction"]["y"].toDouble();
    float z = object["direction"]["z"].toDouble();
    setDirection(QVector3D(x, y, z));

    mEnabled = object["enabled"].toBool(mEnabled);
}
