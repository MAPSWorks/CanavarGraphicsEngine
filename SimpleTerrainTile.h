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

    struct TileProperties {
        int cellCount;
        float offset;
        float amplitude;
        int octaves;
        float roughness;
        float ambient;
        float diffuse;
        float shininess;
        float specular;
    };

    explicit SimpleTerrainTile(QObject *parent = nullptr);

    bool create();
    void render();
    QMatrix4x4 transformation();

    const TileProperties &properties() const;
    void setProperties(const TileProperties &newProperties);

private:
    QOpenGLVertexArrayObject mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    QVector3D mDummyNormal;
    TileProperties mProperties;
};

#endif // SIMPLETERRAINTILE_H
