#ifndef QUAD_H
#define QUAD_H

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class Quad : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
private:
    explicit Quad(QObject *parent = nullptr);

public:
    void render();

    static Quad *instance();

private:
    unsigned int mVAO;
    unsigned int mVBO;
    static const float VERTICES[24];
};

#endif // QUAD_H
