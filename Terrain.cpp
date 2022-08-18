#include "Terrain.h"

#include <QMatrix4x4>

Terrain::Terrain(QObject *parent)
    : QObject{parent}
{
    mProperties.resolution = 4;
    mProperties.grids = 101;
    mProperties.width = 1024;
    mProperties.octaves = 13;
    mProperties.freq = 0.01f;
    mProperties.tessellationMultiplier = 1.0f;
    mProperties.amplitude = 40.0f;
    mProperties.power = 3.0f;
}

void Terrain::create()
{
    const int resolution = mProperties.resolution;
    const int width = mProperties.width;

    const int nPoints = resolution * resolution;
    const int size = nPoints * 3 + nPoints * 3 + nPoints * 2;
    float *vertices = new float[size];

    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            //add position
            float x = j * (float) width / (resolution - 1) - width / 2.0;
            float y = 0.0;
            float z = -i * (float) width / (resolution - 1) + width / 2.0;

            vertices[(i + j * resolution) * 8] = x; //8 = 3 + 3 + 2, float per point
            vertices[(i + j * resolution) * 8 + 1] = y;
            vertices[(i + j * resolution) * 8 + 2] = z;

            //add normal
            float x_n = 0.0;
            float y_n = 1.0;
            float z_n = 0.0;

            vertices[(i + j * resolution) * 8 + 3] = x_n;
            vertices[(i + j * resolution) * 8 + 4] = y_n;
            vertices[(i + j * resolution) * 8 + 5] = z_n;

            //add texcoords
            vertices[(i + j * resolution) * 8 + 6] = (float) j / (resolution - 1);
            vertices[(i + j * resolution) * 8 + 7] = (float) (resolution - i - 1) / (resolution - 1);
        }
    }

    const int nTris = (resolution - 1) * (resolution - 1) * 2;
    int *trisIndices = new int[nTris * 3];

    for (int i = 0; i < nTris; i++)
    {
        int trisPerRow = 2 * (resolution - 1);
        for (int j = 0; j < trisPerRow; j++)
        {
            if (!(i % 2))
            { //upper triangle
                int k = i * 3;
                int triIndex = i % trisPerRow;

                int row = i / trisPerRow;
                int col = triIndex / 2;
                trisIndices[k] = row * resolution + col;
                trisIndices[k + 1] = ++row * resolution + col;
                trisIndices[k + 2] = --row * resolution + ++col;
            } else
            {
                int k = i * 3;
                int triIndex = i % trisPerRow;

                int row = i / trisPerRow;
                int col = triIndex / 2;
                trisIndices[k] = row * resolution + ++col;
                trisIndices[k + 1] = ++row * resolution + --col;
                trisIndices[k + 2] = row * resolution + ++col;
            }
        }
    }

    QVector2D I = QVector2D(1, 0) * mProperties.width;
    QVector2D J = QVector2D(0, 1) * mProperties.width;

    mPositions.resize(mProperties.grids * mProperties.grids);

    for (int i = 0; i < mProperties.grids; i++)
    {
        for (int j = 0; j < mProperties.grids; j++)
        {
            QVector2D pos = (float) (j - mProperties.grids / 2) * I + (float) (i - mProperties.grids / 2) * J;
            mPositions[i + j * mProperties.grids] = pos;
        }
    }

    initializeOpenGLFunctions();
    mVAO.create();
    mVAO.bind();
    glGenBuffers(1, &mEBO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mPBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTris * 3 * sizeof(unsigned int), trisIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // PBO
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mPositions.size() * sizeof(QVector2D), mPositions.constData(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), (void *) 0);
    glVertexAttribDivisor(3, 1);

    mVAO.release();

    delete[] vertices;
    delete[] trisIndices;
}

void Terrain::render()
{
    mVAO.bind();
    glDrawElementsInstanced(GL_PATCHES, (mProperties.resolution - 1) * (mProperties.resolution - 1) * 2 * 3, GL_UNSIGNED_INT, 0, mPositions.size());
    mVAO.release();
}

QMatrix4x4 Terrain::transformation() const
{
    return QMatrix4x4();
}

const Terrain::Properties &Terrain::properties() const
{
    return mProperties;
}

void Terrain::setProperties(const Properties &newProperties)
{
    mProperties = newProperties;
}
