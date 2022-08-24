#ifndef TEXTURE_H
#define TEXTURE_H

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>

class Texture : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    enum class Type { //
        Custom2D,
        Custom3D,
        Ambient,
        Diffuse,
        Specular,
        Normal
    };

    explicit Texture(Type type, const QString &path, QObject *parent = nullptr);
    explicit Texture(int width, int height, QObject *parent = nullptr);
    explicit Texture(int width, int height, int depth, QObject *parent = nullptr);
    virtual ~Texture();

    Type type() const;
    unsigned int id() const;
    const QString &path() const;
    int width() const;
    int height() const;
    int depth() const;

private:
    unsigned int mId;
    Type mType;
    QString mPath;
    int mWidth;
    int mHeight;
    int mDepth;
};

#endif // TEXTURE_H
