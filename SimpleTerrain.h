#ifndef SIMPLETERRAIN_H
#define SIMPLETERRAIN_H

#include <QObject>

#include <QMap>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

class SimpleTerrain : public QObject
{
    Q_OBJECT
public:
    explicit SimpleTerrain(QObject *parent = nullptr);
    //    ~SimpleTerrain();

    //    bool create();
    //    void render();

private:
    //    QMap<Tile, QOpenGLVertexArrayObject *> mVAOs;
    //    QMap<Tile, unsigned int> mVBOs;
    //    QMap<Tile, unsigned int> mEBOs;
    //    QMap<Tile, QVector<Vertex>> mVertices;
    //    QMap<Tile, QVector<unsigned int>> mIndices;
};

#endif // SIMPLETERRAIN_H
