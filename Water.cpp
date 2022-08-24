#include "Water.h"

Water::Water(QObject *parent)
    : QObject(parent)
{}

Water *Water::instance()
{
    static Water water;
    return &water;
}

void Water::create(int numberOfVerticesOnEdge, float width)
{
    const int n = numberOfVerticesOnEdge;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            float x = j * width / (n - 1) - width / 2.0f;
            float y = 0;
            float z = i * width / (n - 1) - width / 2.0f;
            float u = float(j) / (n - 1);
            float v = float(n - i - 1) / (n - 1);

            Vertex vertex;
            vertex.position = QVector3D(x, y, z);
            vertex.normal = QVector3D(0, 1, 0);
            vertex.texture = QVector2D(u, v);

            mVertices << vertex;
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - 1; j++)
        {
            mIndices << n * i + j;
            mIndices << n * (i + 1) + j;
            mIndices << n * i + j + 1;

            mIndices << n * (i + 1) + j;
            mIndices << n * (i + 1) + j + 1;
            mIndices << n * i + j + 1;
        }
    }

    initializeOpenGLFunctions();
    mVAO = new QOpenGLVertexArrayObject;
    mVAO->create();
    mVAO->bind();

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);
}
