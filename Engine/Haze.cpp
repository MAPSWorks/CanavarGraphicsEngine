#include "Haze.h"

Canavar::Engine::Haze::Haze()
    : Node()
    , mColor(0.33f, 0.38f, 0.47f)
    , mEnabled(true)
    , mDensity(1.0f)
    , mGradient(1.5f)
{
    mType = Node::NodeType::Haze;
    mName = "Haze";
    mSelectable = false;
}

Canavar::Engine::Haze *Canavar::Engine::Haze::instance()
{
    static Haze instance;
    return &instance;
}

void Canavar::Engine::Haze::toJson(QJsonObject &object)
{
    Node::toJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    object.insert("color", color);

    object.insert("enabled", mEnabled);
    object.insert("density", mDensity);
    object.insert("gradient", mGradient);
}

void Canavar::Engine::Haze::fromJson(const QJsonObject &object)
{
    Node::fromJson(object);

    // Color
    {
        float x = object["color"]["r"].toDouble();
        float y = object["color"]["g"].toDouble();
        float z = object["color"]["b"].toDouble();

        mColor = QVector3D(x, y, z);
    }

    mEnabled = object["enabled"].toBool();
    mDensity = object["density"].toDouble();
    mGradient = object["gradient"].toDouble();
}
