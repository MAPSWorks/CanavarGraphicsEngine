#include "SimpleTerrainTile.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QtMath>

SimpleTerrainTile::SimpleTerrainTile(QObject *parent)
    : QObject(parent)
    , mOffset(4)
    , mAmplitude(10.0f)
    , mOctaves(3)
    , mRoughness(0.3f)
{
    mSeed = QRandomGenerator::securelySeeded().generate();
}

bool SimpleTerrainTile::create()
{
    mVertices.reserve(100000);
    mIndices.reserve(100000);

    for (int i = -32; i < 32; i++)
    {
        for (int j = -32; j < 32; j++)
        {
            QVector3D topLeft = QVector3D(i * mOffset, generateHeight(i * mOffset, j * mOffset), j * mOffset);
            QVector3D bottomLeft = QVector3D(i * mOffset, generateHeight(i * mOffset, (j + 1) * mOffset), (j + 1) * mOffset);
            QVector3D topRight = QVector3D((i + 1) * mOffset, generateHeight((i + 1) * mOffset, j * mOffset), j * mOffset);
            QVector3D bottomRight = QVector3D((i + 1) * mOffset, generateHeight((i + 1) * mOffset, (j + 1) * mOffset), (j + 1) * mOffset);

            QVector3D normal0 = QVector3D::crossProduct((bottomLeft - topLeft), (topRight - topLeft)).normalized();
            QVector3D normal1 = QVector3D::crossProduct((bottomLeft - topRight), (bottomRight - topRight)).normalized();

            // 0-> Top left
            {
                Vertex vertex;
                vertex.position = topLeft;
                vertex.normal = normal0;
                vertex.texture = QVector2D(i / 32.0f, j / 32.0f);
                mVertices << vertex;
            }

            // 1-> Bottom Left
            {
                Vertex vertex;
                vertex.position = bottomLeft;
                vertex.normal = ((normal0 + normal1) / 2.0f).normalized();
                vertex.texture = QVector2D(i / 32.0f, (j + 1) / 32.0f);
                mVertices << vertex;
            }

            // 2-> Top Right
            {
                Vertex vertex;
                vertex.position = topRight;
                vertex.normal = ((normal0 + normal1) / 2.0f).normalized();
                vertex.texture = QVector2D((i + 1) / 32.0f, j / 32.0f);
                mVertices << vertex;
            }

            // 4-> Bottom Right
            {
                Vertex vertex;
                vertex.position = bottomRight;
                vertex.normal = normal1;
                vertex.texture = QVector2D((i + 1) / 32.0f, (j + 1) / 32.0f);
                mVertices << vertex;
            }
        }
    }

    qInfo() << "Tile vertex count:" << mVertices.size();

    for (unsigned int i = 0; i < 64; ++i)
    {
        for (unsigned int j = 0; j < 64; ++j)
        {
            mIndices << 256 * i + 4 * j;
            mIndices << 256 * i + 4 * j + 1;
            mIndices << 256 * i + 4 * j + 2;
            mIndices << 256 * i + 4 * j + 2;
            mIndices << 256 * i + 4 * j + 1;
            mIndices << 256 * i + 4 * j + 3;
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

float SimpleTerrainTile::generateHeight(float x, float z)
{
    float total = 0;
    float d = pow(2, mOctaves - 1);
    for (int i = 0; i < mOctaves; i++)
    {
        float freq = pow(2, i) / d;
        float amp = pow(mRoughness, i) * mAmplitude;
        total += getInterpolatedNoise((x + mOffset) * freq, (z + mOffset) * freq) * amp;
    }
    return total;
}

float SimpleTerrainTile::getSmoothNoise(float x, float z)
{
    float corners = (getNoise(x - 1, z - 1) + getNoise(x + 1, z - 1) + getNoise(x - 1, z + 1) + getNoise(x + 1, z + 1)) / 16.0f;
    float sides = (getNoise(x - 1, z) + getNoise(x + 1, z) + getNoise(x, z - 1) + getNoise(x, z + 1)) / 8.0f;
    float center = getNoise(x, z) / 4.0f;

    return corners + sides + center;
}

float SimpleTerrainTile::getNoise(float x, float z)
{
    mRandomGenerator.seed(101 * x + 100001 * z + mSeed);
    return 2.0f * mRandomGenerator.generateDouble() - 1;
}

float SimpleTerrainTile::interpolate(float a, float b, float blend)
{
    float theta = M_PI * blend;
    float f = 0.5f * (1.0f - cos(theta));
    return a * (1.0f - f) + b * f;
}

float SimpleTerrainTile::getInterpolatedNoise(float x, float z)
{
    int intX = (int) x;
    int intZ = (int) z;
    float fracX = x - intX;
    float fracZ = z - intZ;

    float v1 = getSmoothNoise(intX, intZ);
    float v2 = getSmoothNoise(intX + 1, intZ);
    float v3 = getSmoothNoise(intX, intZ + 1);
    float v4 = getSmoothNoise(intX + 1, intZ + 1);
    float i1 = interpolate(v1, v2, fracX);
    float i2 = interpolate(v3, v4, fracX);
    return interpolate(i1, i2, fracZ);
}
