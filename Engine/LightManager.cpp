#include "LightManager.h"

Canavar::Engine::LightManager::LightManager(QObject *parent)
    : Manager(parent)
    , mSun(nullptr)
{}

Canavar::Engine::Sun *Canavar::Engine::LightManager::sun() const
{
    return mSun;
}

void Canavar::Engine::LightManager::setSun(Sun *sun)
{
    mSun = sun;
}

Canavar::Engine::LightManager *Canavar::Engine::LightManager::instance()
{
    static LightManager instance;
    return &instance;
}

bool Canavar::Engine::LightManager::init()
{
    return true;
}
