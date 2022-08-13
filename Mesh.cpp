#include "Mesh.h"
#include "qdebug.h"

Mesh::Mesh(QObject *parent)
    : QObject{parent}
{}

void Mesh::addVertex(const Vertex &vertex)
{
    mVertices << vertex;
}

void Mesh::addIndex(unsigned int index)
{
    mIndices << index;
}

int Mesh::materialIndex() const
{
    return mMaterialIndex;
}

void Mesh::setMaterialIndex(int newMaterialIndex)
{
    mMaterialIndex = newMaterialIndex;
}

bool Mesh::create()
{
    qInfo() << Q_FUNC_INFO << "Creating VAO for mesh [" << mName << "] of model [" << parent()->objectName() << "]...";

    initializeOpenGLFunctions();

    mVertexArray.create();
    mVertexArray.bind();

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
    glVertexAttribPointer(1,
                          3,                                // Size
                          GL_FLOAT,                         // Type
                          GL_FALSE,                         // Normalized
                          sizeof(Vertex),                   // Stride
                          (void *) offsetof(Vertex, normal) // Offset
    );

    //Texture Cooords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, textureCoord));
    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
    // Bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));
    // IDs
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, boneIDs));
    // Weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, weights));

    mVertexArray.release();

    return true;
}

void Mesh::bind()
{
    mVertexArray.bind();
}

void Mesh::release()
{
    mVertexArray.release();
}

int Mesh::count()
{
    return mIndices.size();
}

const QString &Mesh::name() const
{
    return mName;
}

void Mesh::setName(const QString &newName)
{
    setObjectName(newName);
    mName = newName;
}

const QVector<unsigned int> &Mesh::indices() const
{
    return mIndices;
}
