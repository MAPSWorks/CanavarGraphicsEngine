#ifndef SIMPLETERRAINTILE_H
#define SIMPLETERRAINTILE_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QRandomGenerator>
#include <QVector2D>
#include <QVector3D>

class SimpleTerrainTile : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    enum class Tile : int {
        Center = 0,
        Top = -10,
        Bottom = 10,
        Left = -1,
        Right = 1,
        TopLeft = -11,
        TopRight = -9,
        BottomLeft = 9,
        BottomRight = 11

    };

    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
    };

    explicit SimpleTerrainTile(QObject *parent = nullptr);

    bool create();
    void render();
    QMatrix4x4 transformation();

private:
    float generateHeight(float x, float z);
    float getSmoothNoise(float x, float z);
    float getNoise(float x, float z);
    float interpolate(float a, float b, float blend);
    float getInterpolatedNoise(float x, float z);

private:
    QOpenGLVertexArrayObject mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;
    QRandomGenerator mRandomGenerator;
    unsigned int mSeed;
    float mOffset;
    float mAmplitude;
    int mOctaves;
    float mRoughness;
};

#endif // SIMPLETERRAINTILE_H
