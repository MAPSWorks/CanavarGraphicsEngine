#ifndef HELPER_H
#define HELPER_H

#include "ModelData.h"
#include "ModelDataNode.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "TextureMaterial.h"

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
    static ModelData *loadModel(const QString &name, const QString &path);
    static QVector<PointLight *> getClosePointLights(const QList<PointLight *> &pointLights, Node *node);
    static QVector<SpotLight *> getCloseSpotLights(const QList<SpotLight *> &spotLights, Node *node);

private:
    static Mesh *processMesh(aiMesh *aiMesh);
    static ModelDataNode *processNode(ModelData *data, aiNode *node);
    static TextureMaterial *processMaterial(aiMaterial *aiMaterial, const QString &directory);
    static void processTexture(TextureMaterial *material, aiMaterial *aiMaterial, aiTextureType aiType, Texture::Type type, const QString &directory);
    static QMatrix4x4 toQMatrix(const aiMatrix4x4 &matrix);
};

#endif // HELPER_H
