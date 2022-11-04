#ifndef HELPER_H
#define HELPER_H

#include "ModelData.h"

#include <PointLight.h>
#include <QByteArray>
#include <QQuaternion>
#include <QRandomGenerator>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Canavar {
namespace Engine {

class Helper
{
private:
    Helper();

public:
    static QByteArray getBytes(QString path);
    static float calculateHorizontalFovForGivenVerticalFov(float verticalFov, float width, float height);
    static float calculateVerticalFovForGivenHorizontalFov(float horizontalFov, float width, float height);
    static QQuaternion rotateX(float angleRadians);
    static QQuaternion rotateY(float angleRadians);
    static QQuaternion rotateZ(float angleRadians);
    static float angleBetween(const QVector3D &v1, const QVector3D &v2, const QVector3D &left);
    static float angleBetween(const QVector3D &v1, const QVector3D &v2);
    static void getEulerDegrees(const QQuaternion &rotation, float &yaw, float &pitch, float &roll);
    static QQuaternion constructFromEulerDegrees(float yaw, float pitch, float roll);
    static QQuaternion invert(const QQuaternion &rotation);
    static ModelData *loadModel(const QString &name, const QString &path);
    static float generateFloat(float bound);
    static float generateBetween(float lower, float upper);
    static QVector3D generateVec3(float x, float y, float z);
    static QOpenGLTexture *createTexture(const QString &path);

    static QVector<PointLight *> getClosePointLights(const QList<PointLight *> &nodes, const QVector3D &position, int maxCount);
    static QJsonDocument loadJson(const QString &path);

private:
    static Mesh *processMesh(aiMesh *aiMesh);
    static ModelDataNode *processNode(ModelData *data, aiNode *aiParentNode);
    static Material *processMaterial(aiMaterial *aiMaterial, const QString &directory);
    static void processTexture(Material *material, aiMaterial *aiMaterial, aiTextureType aiType, Material::TextureType type, const QString &directory);
    static QMatrix4x4 toQMatrix(const aiMatrix4x4 &matrix);
    static void calculateAABB(ModelData *data);

    static QRandomGenerator mGenerator;
};

} // namespace Engine
} // namespace Canavar

#endif // HELPER_H
