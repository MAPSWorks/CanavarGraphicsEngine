#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "DirectionalLight.h"
#include "NodeManager.h"
#include "PointLight.h"
#include "SpotLight.h"

#include <QObject>

class LightManager : public QObject
{
    Q_OBJECT

private:
    friend class NodeManager;
    explicit LightManager(QObject *parent = nullptr);
    void addLight(Light *light);
    void removeLight(Light *light);

public:
    DirectionalLight *directionalLight() const;
    void setDirectionalLight(DirectionalLight *newDirectionalLight);

    const QList<PointLight *> &pointLights() const;
    const QList<SpotLight *> &spotLights() const;

    static LightManager *instance();

private:
    QList<PointLight *> mPointLights;
    QList<SpotLight *> mSpotLights;
    DirectionalLight *mDirectionalLight;
};

#endif // LIGHTMANAGER_H
