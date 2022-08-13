#include "LightManager.h"
#include "Light.h"

LightManager::LightManager(QObject *parent)
    : QObject(parent)
    , mActiveLight(nullptr)
{
    mNodeManager = NodeManager::instance();
}

void LightManager::addLight(Light *light)
{
    mLights << light;
    mNodeManager->addNode(light);
}

void LightManager::removeLight(Light *light)
{
    if (light)
    {
        if (light == mActiveLight)
        {
            mActiveLight = nullptr;
        }

        mNodeManager->removeNode(light);
        mLights.removeAll(light);
        light->deleteLater();
    }
}

Light *LightManager::activeLight() const
{
    return mActiveLight;
}

void LightManager::setActiveLight(Light *newActiveLight)
{
    mActiveLight = newActiveLight;
}

LightManager *LightManager::instance()
{
    static LightManager instance;

    return &instance;
}

const QList<Light *> &LightManager::lights() const
{
    return mLights;
}
