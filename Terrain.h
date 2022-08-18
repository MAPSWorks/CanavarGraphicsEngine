#ifndef TERRAIN_H
#define TERRAIN_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctionsPrivate>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

class Terrain : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    struct Properties {
        int resolution;
        int grids;
        float width;
        float amplitude;
        float freq;
        int octaves;
        float power;
        QVector3D seed;
        QVector4D clipPlane;
        float tessellationMultiplier;
    };

    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
    };

    explicit Terrain(QObject *parent = nullptr);

    void create();
    void render();

    QMatrix4x4 transformation() const;

    const Properties &properties() const;
    void setProperties(const Properties &newProperties);

private:
    Properties mProperties;
    QVector<QVector2D> mPositions;
    QOpenGLVertexArrayObject mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
    unsigned int mPBO;
};

#endif // TERRAIN_H
