#ifndef QUAD_H
#define QUAD_H

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class Quad : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit Quad(QObject *parent = nullptr);

    void create();
    void render();

private:
    unsigned int mVAO;
    unsigned int mVBO;
    static const float VERTICES[24];
};

#endif // QUAD_H
