#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

class DirectionalLight : public Light
{
private:
    friend class NodeManager;
    explicit DirectionalLight(QObject *parent = nullptr);

public:
    const QVector3D &direction() const;
    void setDirection(const QVector3D &newDirection);

private:
    QVector3D mDirection;
};

#endif // DIRECTIONALLIGHT_H
