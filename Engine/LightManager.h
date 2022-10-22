#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "Manager.h"
#include "NodeManager.h"
#include "PointLight.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class LightManager : public Manager
{
    Q_OBJECT
private:
    friend class Canavar::Engine::NodeManager;

    explicit LightManager(QObject *parent = nullptr);

public:
    static LightManager *instance();
    bool init() override;

    void addLight(Light *light);
    void removeLight(Light *light);
    const QList<PointLight *> &getPointLights() const;

private:
    QList<PointLight *> mPointLights;
};

} // namespace Engine
} // namespace Canavar

#endif // LIGHTMANAGER_H
