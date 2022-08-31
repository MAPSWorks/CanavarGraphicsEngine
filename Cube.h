#ifndef CUBE_H
#define CUBE_H

#include <QObject>
#include <QOpenGLExtraFunctions>

class Cube : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
private:
    explicit Cube(QObject *parent = nullptr);

public:
    void render();

    static Cube *instance();

private:
    unsigned int mVAO;
    unsigned int mVBO;
    static const float VERTICES[108];
};

#endif // CUBE_H
