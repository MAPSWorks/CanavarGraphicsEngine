#include "LightManager.h"

Canavar::Engine::LightManager::LightManager()
    : Manager()
{}

Canavar::Engine::LightManager *Canavar::Engine::LightManager::instance()
{
    static LightManager instance;
    return &instance;
}

bool Canavar::Engine::LightManager::init()
{
    return true;
}

void Canavar::Engine::LightManager::removeLight(Light *light)
{
    if (auto pointLight = dynamic_cast<PointLight *>(light))
        mPointLights.removeAll(pointLight);
}

void Canavar::Engine::LightManager::addLight(Light *light)
{
    if (auto pointLight = dynamic_cast<PointLight *>(light))
        mPointLights << pointLight;
}

const QList<Canavar::Engine::PointLight *> &Canavar::Engine::LightManager::getPointLights() const
{
    return mPointLights;
}
