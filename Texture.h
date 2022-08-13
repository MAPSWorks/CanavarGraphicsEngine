#ifndef TEXTURE_H
#define TEXTURE_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

class Texture : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    enum class Type { //
        Diffuse,
        Specular,
        Ambient,
        Height
    };

    explicit Texture(Type type, const QString &path, QObject *parent = nullptr);
    virtual ~Texture();

    void create();

    Type type() const;
    const QString &path() const;
    unsigned int id() const;

private:
    Type mType;
    QString mPath;
    unsigned int mId;
};

#endif // TEXTURE_H
