#ifndef MATERIAL_H
#define MATERIAL_H

#include <QMap>
#include <QObject>
#include <QOpenGLTexture>

namespace Canavar {
namespace Engine {

class Material : public QObject
{
    Q_OBJECT
public:
    explicit Material(QObject *parent = nullptr);
    virtual ~Material();

    enum class TextureType //
    {
        Ambient,
        Diffuse,
        Specular,
        Normal
    };

    void insert(TextureType type, QOpenGLTexture *texture);
    QOpenGLTexture *get(TextureType type);

private:
    QMap<TextureType, QOpenGLTexture *> mTextures;
};

} // namespace Engine
} // namespace Canavar
#endif // MATERIAL_H
