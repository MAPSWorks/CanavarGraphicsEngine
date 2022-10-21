#include "LightManager.h"

Canavar::Engine::LightManager::LightManager(QObject *parent)
    : Manager(parent)
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
