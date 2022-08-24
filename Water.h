#ifndef WATER_H
#define WATER_H

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

class Water : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
private:
    explicit Water(QObject *parent = nullptr);

    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
    };

public:
    static Water *instance();
    void create(int numberOfVerticesOnEdge = 4, float width = 1024.0f);

private:
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    QOpenGLVertexArrayObject *mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
};

#endif // WATER_H
