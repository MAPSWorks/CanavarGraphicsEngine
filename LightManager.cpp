#include "LightManager.h"

LightManager::LightManager(QObject *parent)
    : Manager(parent)
    , mDirectionalLight(nullptr)
{}

void LightManager::addLight(Light *light)
{
    PointLight *pointLight = dynamic_cast<PointLight *>(light);
    SpotLight *spotLight = dynamic_cast<SpotLight *>(light);
    DirectionalLight *directionalLight = dynamic_cast<DirectionalLight *>(light);

    if (pointLight)
    {
        mPointLights << pointLight;

    } else if (spotLight)
    {
        mSpotLights << spotLight;

    } else if (directionalLight)
    {
        qWarning() << "Could not add a directional light. Directional light must be set via set method because there must be only one directional light in the scene.";
        return;

    } else
    {
        qWarning() << "Light is nullptr.";
        return;
    }
}

void LightManager::removeLight(Light *light)
{
    PointLight *pointLight = dynamic_cast<PointLight *>(light);
    SpotLight *spotLight = dynamic_cast<SpotLight *>(light);
    DirectionalLight *directionalLight = dynamic_cast<DirectionalLight *>(light);

    if (pointLight)
        mPointLights.removeAll(pointLight);
    else if (spotLight)
        mPointLights.removeAll(spotLight);
    else if (directionalLight)
    {
        qWarning() << "Directional light cannot be removed.";
        return;
    } else
    {
        qWarning() << "Light is nullptr.";
        return;
    }
}

LightManager *LightManager::instance()
{
    static LightManager instance;

    return &instance;
}

const QList<PointLight *> &LightManager::pointLights() const
{
    return mPointLights;
}

const QList<SpotLight *> &LightManager::spotLights() const
{
    return mSpotLights;
}

DirectionalLight *LightManager::directionalLight() const
{
    return mDirectionalLight;
}

void LightManager::setDirectionalLight(DirectionalLight *newDirectionalLight)
{
    mDirectionalLight = newDirectionalLight;
}

bool LightManager::init()
{
    return true;
}
