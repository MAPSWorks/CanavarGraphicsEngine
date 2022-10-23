#ifndef TILEGENERATOR_H
#define TILEGENERATOR_H

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

namespace Canavar {
namespace Engine {

class TileGenerator : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D texture;
    };

    explicit TileGenerator(int resolution, int tiles, float width, QObject *parent = nullptr);
    virtual ~TileGenerator();

    void render(GLenum primitive);

    QVector2D whichTile(const QVector3D &subject) const;
    void translateTiles(const QVector2D &translation);

private:
    QVector<QVector2D> mTilePositions;
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    int mResolution;
    int mTiles;
    float mWidth;

    // OpenGL Stuff
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
    unsigned int mPBO;
};

} // namespace Engine
} // namespace Canavar

#endif // TILEGENERATOR_H
