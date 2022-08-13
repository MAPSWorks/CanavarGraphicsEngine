#ifndef HELPER_H
#define HELPER_H

#include "TexturedModelData.h"
#include "TexturedModelDataNode.h"

#include <QMatrix4x4>
#include <QQuaternion>
#include <QString>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Helper
{
private:
    Helper();

public:
    static QByteArray getBytes(QString path);
    static QQuaternion rotateX(float angleRadians);
    static QQuaternion rotateY(float angleRadians);
    static QQuaternion rotateZ(float angleRadians);
    static QVector3D projectOntoPlane(const QVector3D &normal, const QVector3D &point, const QVector3D &subject);
    static TexturedModelData *loadTexturedModel(const QString &path);

private:
    static Mesh *processMesh(aiMesh *aiMesh);
    static TexturedModelDataNode *processNode(aiNode *node);
    static QMatrix4x4 toQMatrix(const aiMatrix4x4 &matrix);
};

#endif // HELPER_H
