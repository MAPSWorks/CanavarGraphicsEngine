#include "LightManager.h"

LightManager::LightManager(QObject *parent)
    : QObject(parent)
    , mDirectionalLight(nullptr)
{
    mNodeManager = NodeManager::instance();
}

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

    mNodeManager->addNode(light);
}

void LightManager::removeLight(Light *light)
{
    PointLight *pointLight = dynamic_cast<PointLight *>(light);
    SpotLight *spotLight = dynamic_cast<SpotLight *>(light);
    DirectionalLight *directionalLight = dynamic_cast<DirectionalLight *>(light);

    if (pointLight)
    {
        mNodeManager->removeNode(pointLight);
        mPointLights.removeAll(pointLight);
        pointLight->deleteLater();

    } else if (spotLight)
    {
        mNodeManager->removeNode(spotLight);
        mPointLights.removeAll(spotLight);
        spotLight->deleteLater();

    } else if (directionalLight)
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
