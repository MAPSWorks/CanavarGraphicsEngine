#ifndef SCREENRENDERER_H
#define SCREENRENDERER_H

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class ScreenRenderer : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit ScreenRenderer(QObject *parent = nullptr);

    void create();
    void render();

private:
    unsigned int mVAO;
    unsigned int mVBO;
    static const float QUAD_VERTICES[24];
};

#endif // SCREENRENDERER_H
