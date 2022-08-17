#include "ModelData.h"

#include <QFile>
#include <QVector2D>

ModelData::ModelData(Model::Type type, QObject *parent)
    : QObject(parent)
    , mType(type)
{}

ModelData::~ModelData()
{
    mVertexArray.destroy();
    mVertexBuffer.destroy();
    mNormalBuffer.destroy();
    mTextureCoordsBuffer.destroy();
}

bool ModelData::create()
{
    //qInfo() << "Creating VAO for model" << (int) mType;

    initializeOpenGLFunctions();

    mVertexArray.create();
    mVertexArray.bind();

    // Vertices (Position)
    mVertexBuffer.create();
    mVertexBuffer.bind();
    mVertexBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    mVertexBuffer.allocate(mVertices.constData(), sizeof(QVector3D) * mVertices.size());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector3D), // Stride
                          nullptr            // Offset
    );
    mVertexBuffer.release();

    // Normals
    mNormalBuffer.create();
    mNormalBuffer.bind();
    mNormalBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    mNormalBuffer.allocate(mNormals.constData(), sizeof(QVector3D) * mNormals.size());
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector3D), // Stride
                          nullptr            // Offset
    );
    mNormalBuffer.release();

    // TextureCoords
    mTextureCoordsBuffer.create();
    mTextureCoordsBuffer.bind();
    mTextureCoordsBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    mTextureCoordsBuffer.allocate(mTextureCoords.constData(), sizeof(QVector2D) * mTextureCoords.size());
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          2,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector2D), // Stride
                          nullptr            // Offset
    );
    mTextureCoordsBuffer.release();

    mVertexArray.release();

    //qInfo() << "VAO for model" << (int) mType << "is created.";

    return true;
}

void ModelData::render()
{
    mVertexArray.bind();
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    mVertexArray.release();
}

void ModelData::setVertices(const QVector<QVector3D> &newVertices)
{
    mVertices = newVertices;
}

void ModelData::setNormals(const QVector<QVector3D> &newNormals)
{
    mNormals = newNormals;
}

void ModelData::setTextureCoords(const QVector<QVector2D> &newTextureCoords)
{
    mTextureCoords = newTextureCoords;
}

const QString ModelData::ROOT_PATH = ":/Resources/Models/";

const QMap<Model::Type, QString> ModelData::MODEL_TO_PATH = { //
    {Model::Capsule, ROOT_PATH + "Capsule.obj"},
    {Model::Cone, ROOT_PATH + "Cone.obj"},
    {Model::Cube, ROOT_PATH + "Cube.obj"},
    {Model::Cylinder, ROOT_PATH + "Cylinder.obj"},
    {Model::Dome, ROOT_PATH + "Dome.obj"},
    {Model::Plane, ROOT_PATH + "Plane.obj"},
    {Model::Pyramid, ROOT_PATH + "Pyramid.obj"},
    {Model::Sphere, ROOT_PATH + "Sphere.obj"},
    {Model::Suzanne, ROOT_PATH + "Suzanne.obj"},
    {Model::Tetrahedron, ROOT_PATH + "Tetrahedron.obj"},
    {Model::Torus, ROOT_PATH + "Torus.obj"},
    {Model::TorusKnot, ROOT_PATH + "TorusKnot.obj"}};
