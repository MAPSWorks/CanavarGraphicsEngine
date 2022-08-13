#ifndef TEXTUREDMATERIAL_H
#define TEXTUREDMATERIAL_H

#include "Texture.h"

#include <QObject>

class TexturedMaterial : public QObject
{
    Q_OBJECT
public:
    explicit TexturedMaterial(QObject *parent = nullptr);

    void addTexture(Texture *texture);

    const QVector<Texture *> &textures() const;

private:
    QVector<Texture *> mTextures;
};

#endif // TEXTUREDMATERIAL_H
