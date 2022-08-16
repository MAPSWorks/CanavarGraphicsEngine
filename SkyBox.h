#ifndef SKYBOX_H
#define SKYBOX_H

#include <QMap>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class SkyBox : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit SkyBox(QObject *parent = nullptr);
    ~SkyBox();
    void setPath(unsigned int key, const QString &path);
    void create();
    void render();

private:
    QMap<unsigned int, QString> mPaths;
    QOpenGLVertexArrayObject mVertexArray;
    QOpenGLBuffer mVertexBuffer;
    unsigned int mTextureId;

    static const float VERTICES[108];
};

#endif // SKYBOX_H
