#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include "Texture.h"

#include <QObject>

class TextureMaterial : public QObject
{
    Q_OBJECT
public:
    explicit TextureMaterial(QObject *parent = nullptr);

    void addTexture(Texture *texture);
    const QVector<Texture *> &textures() const;

private:
    QVector<Texture *> mTextures;
};

#endif // TEXTUREMATERIAL_H
