#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector4D>

struct Material {
    Material();

    float ambient;
    float diffuse;
    float specular;
    float shininess;
    QVector4D color;
};

#endif // MATERIAL_H
