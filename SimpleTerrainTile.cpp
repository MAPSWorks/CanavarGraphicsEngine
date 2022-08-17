#include "SimpleTerrainTile.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QtMath>

SimpleTerrainTile::SimpleTerrainTile(QObject *parent)
    : QObject(parent)
    , mDummyNormal(0, 1, 0)
{
    mProperties.cellCount = 512;
    mProperties.offset = 1.0f;
    mProperties.amplitude = 20.0f;
    mProperties.octaves = 3;
    mProperties.roughness = 0.3f;
    mProperties.ambient = 0.25f;
    mProperties.diffuse = 0.5f;
    mProperties.shininess = 4.0f;
    mProperties.specular = 0.1f;
}

bool SimpleTerrainTile::create()
{
    mVertices.reserve(100000);
    mIndices.reserve(100000);

    float offset = mProperties.offset;
    int halfCellCount = mProperties.cellCount / 2;

    for (int i = -halfCellCount; i < halfCellCount; i++)
    {
        for (int j = -halfCellCount; j < halfCellCount; j++)
        {
            // 0-> Top left
            {
                Vertex vertex;
                vertex.position = QVector3D(i * offset, 0, j * offset);
                vertex.texture = QVector2D(i / float(halfCellCount), j / float(halfCellCount));
                mVertices << vertex;
            }

            // 1-> Bottom Left
            {
                Vertex vertex;
                vertex.position = QVector3D(i * offset, 0, (j + 1) * offset);
                vertex.texture = QVector2D(i / float(halfCellCount), (j + 1) / float(halfCellCount));
                mVertices << vertex;
            }

            // 2-> Top Right
            {
                Vertex vertex;
                vertex.position = QVector3D((i + 1) * offset, 0, j * offset);
                vertex.texture = QVector2D((i + 1) / float(halfCellCount), j / float(halfCellCount));
                mVertices << vertex;
            }

            // 4-> Bottom Right
            {
                Vertex vertex;
                vertex.position = QVector3D((i + 1) * offset, 0, (j + 1) * offset);
                vertex.texture = QVector2D((i + 1) / float(halfCellCount), (j + 1) / float(halfCellCount));
                mVertices << vertex;
            }
        }
    }

    qInfo() << "Tile vertex count:" << mVertices.size();

    for (unsigned int i = 0; i < 2 * halfCellCount; ++i)
    {
        for (unsigned int j = 0; j < 2 * halfCellCount; ++j)
        {
            mIndices << 4 * 2 * halfCellCount * i + 4 * j;
            mIndices << 4 * 2 * halfCellCount * i + 4 * j + 1;
            mIndices << 4 * 2 * halfCellCount * i + 4 * j + 2;
            mIndices << 4 * 2 * halfCellCount * i + 4 * j + 2;
            mIndices << 4 * 2 * halfCellCount * i + 4 * j + 1;
            mIndices << 4 * 2 * halfCellCount * i + 4 * j + 3;
        }
    }

    qInfo() << "Tile index count:" << mIndices.size();

    initializeOpenGLFunctions();

    mVAO.create();
    mVAO.bind();

    glGenBuffers(1, &mEBO);
    glGenBuffers(1, &mVBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,              // Size
                          GL_FLOAT,       // Type
                          GL_FALSE,       // Normalized
                          sizeof(Vertex), // Stride
                          (void *) 0      // Offset
    );

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    //Texture Cooords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    mVAO.release();

    return true;
}

void SimpleTerrainTile::render()
{
    mVAO.bind();
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVAO.release();
}

QMatrix4x4 SimpleTerrainTile::transformation()
{
    return QMatrix4x4();
}

const SimpleTerrainTile::TileProperties &SimpleTerrainTile::properties() const
{
    return mProperties;
}

void SimpleTerrainTile::setProperties(const TileProperties &newProperties)
{
    mProperties = newProperties;
}
