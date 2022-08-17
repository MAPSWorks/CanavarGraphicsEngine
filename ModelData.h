#ifndef MODELDATA_H
#define MODELDATA_H

#include "Model.h"

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class ModelData : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit ModelData(Model::Type type, QObject *parent = nullptr);
    virtual ~ModelData();

    bool create();
    void render();

    void setVertices(const QVector<QVector3D> &newVertices);
    void setNormals(const QVector<QVector3D> &newNormals);
    void setTextureCoords(const QVector<QVector2D> &newTextureCoords);

    static const QMap<Model::Type, QString> MODEL_TO_PATH;

private:
    Model::Type mType;
    QOpenGLVertexArrayObject mVertexArray;
    QOpenGLBuffer mVertexBuffer;
    QOpenGLBuffer mNormalBuffer;
    QOpenGLBuffer mTextureCoordsBuffer;
    QVector<QVector3D> mVertices;
    QVector<QVector3D> mNormals;
    QVector<QVector2D> mTextureCoords;

    static const QString ROOT_PATH;
};

#endif // MODELDATA_H
