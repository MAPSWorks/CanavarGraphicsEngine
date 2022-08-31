#include "Helper.h"
#include "Mesh.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuaternion>
#include <QTextStream>
#include <QtMath>

Helper::Helper() {}

QByteArray Helper::getBytes(QString path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        return file.readAll();
    } else
    {
        qWarning() << QString("Could not open '%1'").arg(path);
        return QByteArray();
    }
}

QQuaternion Helper::rotateX(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateY(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateZ(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), qRadiansToDegrees(angleRadians));
}

ModelData *Helper::loadModel(const QString &name, const QString &path)
{
    Assimp::Importer importer;
    const aiScene *aiScene = importer.ReadFile(path.toStdString(),              //
                                               aiProcess_Triangulate |          //
                                                   aiProcess_GenSmoothNormals | //
                                                   aiProcess_FlipUVs |          //
                                                   aiProcess_CalcTangentSpace | //
                                                   aiProcess_GenBoundingBoxes);

    if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
    {
        qCritical() << Q_FUNC_INFO << importer.GetErrorString();
        return nullptr;
    }

    ModelData *data = new ModelData(name);

    // Meshes
    for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
    {
        aiMesh *aiMesh = aiScene->mMeshes[i];
        Mesh *mesh = processMesh(aiMesh);
        mesh->setIndex(i);
        mesh->setData(data);
        data->addMesh(mesh);
    }

    // Node
    ModelDataNode *rootNode = processNode(data, aiScene->mRootNode);
    data->setRootNode(rootNode);

    QString directory = path.left(path.lastIndexOf("/"));

    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        TextureMaterial *material = processMaterial(aiScene->mMaterials[i], directory);
        data->addMaterial(material);
    }

    return data;
}

QVector<PointLight *> Helper::getClosePointLights(const QList<PointLight *> &pointLights, Node *node)
{
    QMap<float, PointLight *> distanceToLight;

    for (const auto &light : pointLights)
        distanceToLight.insert((light->position() - node->position()).length(), light);

    QList<PointLight *> lights = distanceToLight.values();

    QVector<PointLight *> lightsVector;

    for (int i = 0; i < qMin(8, lights.size()); ++i)
        lightsVector << lights[i];

    return lightsVector;
}

QVector<SpotLight *> Helper::getCloseSpotLights(const QList<SpotLight *> &spotLights, Node *node)
{
    QMap<float, SpotLight *> distanceToLight;

    for (const auto &light : spotLights)
        distanceToLight.insert((light->position() - node->position()).length(), light);

    QList<SpotLight *> lights = distanceToLight.values();

    QVector<SpotLight *> lightsVector;

    for (int i = 0; i < qMin(8, lights.size()); ++i)
        lightsVector << lights[i];

    return lightsVector;
}

QQuaternion Helper::invert(const QQuaternion &rotation)
{
    float yaw, pitch, roll;
    getEulerDegrees(rotation, yaw, pitch, roll);

    QQuaternion r = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -pitch);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), roll);
    return r;
}

float Helper::angleBetween(const QVector3D &v1, const QVector3D &v2, const QVector3D &left)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;

    } else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;

    } else
    {
        float angle = qRadiansToDegrees(acos(dot));
        QVector3D u1xu2 = QVector3D::crossProduct(u1, u2);

        if (QVector3D::dotProduct(u1xu2, left) < 0.0001f)
            return angle;
        else
            return -angle;
    }
}

float Helper::angleBetween(const QVector3D &v1, const QVector3D &v2)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;

    } else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;

    } else
    {
        return qRadiansToDegrees(acos(dot));
    }
}

void Helper::getEulerDegrees(const QQuaternion &rotation, float &yaw, float &pitch, float &roll)
{
    QVector3D zAxis = rotation * QVector3D(0, 0, -1);
    float x = zAxis.x();
    float y = zAxis.y();
    float z = zAxis.z();
    yaw = qRadiansToDegrees(atan2(-z, x)) - 90;
    pitch = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

    QVector3D xAxis = rotation * QVector3D(1, 0, 0);

    QVector3D xAxisProj = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                          QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                          QVector3D(1, 0, 0);

    QVector3D left = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                     QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                     QVector3D(0, 0, -1);

    roll = Helper::angleBetween(xAxis, xAxisProj, left);

    if (yaw < 0.0f)
        yaw += 360.0f;
}

QQuaternion Helper::constructFromEulerDegrees(float yaw, float pitch, float roll)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
           QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
           QQuaternion::fromAxisAndAngle(QVector3D(0, 0, -1), roll);
}

Mesh *Helper::processMesh(aiMesh *aiMesh)
{
    Mesh *mesh = new Mesh;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;

        vertex.position = QVector3D(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);

        if (aiMesh->HasNormals())
            vertex.normal = QVector3D(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);

        if (aiMesh->mTextureCoords[0])
            vertex.texture = QVector2D(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
        else
            vertex.texture = QVector2D(0, 0);

        if (aiMesh->HasTangentsAndBitangents())
        {
            vertex.tangent = QVector3D(aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z);
            vertex.bitangent = QVector3D(aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z);
        }

        mesh->addVertex(vertex);
    }

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace aiFace = aiMesh->mFaces[i];

        for (unsigned int j = 0; j < aiFace.mNumIndices; j++)
            mesh->addIndex(aiFace.mIndices[j]);
    }

    mesh->setName(aiMesh->mName.C_Str());
    mesh->setMaterialIndex(aiMesh->mMaterialIndex);

    Mesh::AABB aabb;
    auto min = aiMesh->mAABB.mMin;
    auto max = aiMesh->mAABB.mMax;
    aabb.min = QVector3D(min.x, min.y, min.z);
    aabb.max = QVector3D(max.x, max.y, max.z);
    mesh->setAABB(aabb);

    return mesh;
}

ModelDataNode *Helper::processNode(ModelData *data, aiNode *aiParentNode)
{
    ModelDataNode *parentNode = new ModelDataNode(data);

    for (unsigned int i = 0; i < aiParentNode->mNumMeshes; ++i)
        parentNode->addMeshIndex(aiParentNode->mMeshes[i]);

    parentNode->setName(aiParentNode->mName.C_Str());
    parentNode->setTransformation(toQMatrix(aiParentNode->mTransformation));

    for (unsigned int i = 0; i < aiParentNode->mNumChildren; ++i)
        parentNode->addChild(processNode(data, aiParentNode->mChildren[i]));

    return parentNode;
}

TextureMaterial *Helper::processMaterial(aiMaterial *aiMaterial, const QString &directory)
{
    TextureMaterial *material = new TextureMaterial;
    processTexture(material, aiMaterial, aiTextureType_AMBIENT, Texture::Type::Ambient, directory);
    processTexture(material, aiMaterial, aiTextureType_DIFFUSE, Texture::Type::Diffuse, directory);
    processTexture(material, aiMaterial, aiTextureType_SPECULAR, Texture::Type::Specular, directory);
    processTexture(material, aiMaterial, aiTextureType_HEIGHT, Texture::Type::Normal, directory);

    return material;
}

void Helper::processTexture(TextureMaterial *material, aiMaterial *aiMaterial, aiTextureType aiType, Texture::Type type, const QString &directory)
{
    for (unsigned int i = 0; i < aiMaterial->GetTextureCount(aiType); i++)
    {
        aiString str;
        aiMaterial->GetTexture(aiType, i, &str);
        QString filename = QString(str.C_Str());
        Texture *texture = new Texture(type, directory + "/" + filename);
        material->addTexture(texture);
    }
}

// TODO
QMatrix4x4 Helper::toQMatrix(const aiMatrix4x4 &matrix)
{
    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D position;
    matrix.Decompose(scaling, rotation, position);

    QVector3D qScaling = QVector3D(scaling.x, scaling.y, scaling.z);
    QQuaternion qRotation = QQuaternion(rotation.w, rotation.x, rotation.y, rotation.z);
    QVector4D qPosition = QVector4D(position.x, position.y, position.z, 1.0);

    QMatrix4x4 transformation;

    transformation.scale(qScaling);
    transformation.rotate(qRotation);
    transformation.setColumn(3, qPosition);

    return transformation;
}
