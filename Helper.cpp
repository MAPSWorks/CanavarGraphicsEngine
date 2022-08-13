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

#include <Dense>

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

QVector3D Helper::projectOntoPlane(const QVector3D &normal, const QVector3D &point, const QVector3D &subject)
{
    Eigen::Vector3f normalEigen = Eigen::Vector3f(normal.x(), normal.y(), normal.z());
    Eigen::Vector3f pointEigen = Eigen::Vector3f(point.x(), point.y(), point.z());
    Eigen::Vector3f subjectEigen = Eigen::Vector3f(subject.x(), subject.y(), subject.z());
    Eigen::Hyperplane<float, 3> plane = Eigen::Hyperplane<float, 3>(normalEigen, -normalEigen.dot(pointEigen));
    Eigen::Vector3f projection = plane.projection(subjectEigen);

    return QVector3D(projection.x(), projection.y(), projection.z());
}

ModelData *Helper::loadModel(Model::Type type, const QString &path)
{
    qInfo() << Q_FUNC_INFO << "Loading model" << (int) type;

    QVector<QVector3D> tempVertices;
    QVector<QVector3D> tempNormals;
    QVector<QVector2D> tempTextureCoords;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector2D> textureCoords;

    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream fileText(&file);
        while (!fileText.atEnd())
        {
            QString fileLine = fileText.readLine();
            if (fileLine.startsWith("vn "))
            {
                QStringList lineList = fileLine.split(" ");
                tempNormals << QVector3D(lineList[1].toFloat(), lineList[2].toFloat(), lineList[3].toFloat());
            } else if (fileLine.startsWith("vt "))
            {
                QStringList lineList = fileLine.split(" ");
                tempTextureCoords << QVector2D(lineList[1].toFloat(), lineList[2].toFloat());
            } else if (fileLine.startsWith("v "))
            {
                QStringList lineList = fileLine.split(" ");
                tempVertices << QVector3D(lineList[1].toFloat(), lineList[2].toFloat(), lineList[3].toFloat());
            } else if (fileLine.startsWith("f "))
            {
                QStringList lineList = fileLine.split(" ");
                for (int i = 1; i <= 3; i++)
                {
                    QStringList arg = lineList[i].split("/");
                    if (arg.size() == 2)
                    {
                        if (arg[0].toInt() - 1 < tempVertices.size())
                            vertices << tempVertices[arg[0].toInt() - 1];

                        if (arg[1].toInt() - 1 < tempNormals.size())
                            normals << tempNormals[arg[1].toInt() - 1];
                    } else if (arg.size() == 3)
                    {
                        if (arg[0].toInt() - 1 < tempVertices.size())
                            vertices << tempVertices[arg[0].toInt() - 1];

                        if (arg[1].toInt() - 1 < tempTextureCoords.size())
                            textureCoords << tempTextureCoords[arg[1].toInt() - 1];

                        if (arg[2].toInt() - 1 < tempNormals.size())
                            normals << tempNormals[arg[2].toInt() - 1];
                    }
                }
            }
        }
        file.close();
        qInfo() << Q_FUNC_INFO << "Model" << (int) type << "is loaded.";

        ModelData *data = new ModelData(type);
        data->setVertices(vertices);
        data->setNormals(normals);
        data->setTextureCoords(textureCoords);

        return data;
    } else
    {
        qWarning() << Q_FUNC_INFO << QString("Could not open file '%1'.").arg(file.fileName());
        qWarning() << Q_FUNC_INFO << "Could not load model" << (int) type;
        return nullptr;
    }
}

TexturedModelData *Helper::loadTexturedModel(const QString &name, const QString &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.toStdString(),              //
                                             aiProcess_Triangulate |          //
                                                 aiProcess_GenSmoothNormals | //
                                                 aiProcess_FlipUVs |          //
                                                 aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        qCritical() << Q_FUNC_INFO << importer.GetErrorString();
        return nullptr;
    }

    TexturedModelData *data = new TexturedModelData(name);

    // Meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *aiMesh = scene->mMeshes[i];
        Mesh *mesh = processMesh(aiMesh);
        data->addMesh(mesh);
    }

    // Node
    TexturedModelDataNode *rootNode = processNode(data, scene->mRootNode);
    data->setRootNode(rootNode);

    // TODO: Material

    return data;
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

    return mesh;
}

TexturedModelDataNode *Helper::processNode(TexturedModelData *data, aiNode *aiParentNode)
{
    TexturedModelDataNode *parentNode = new TexturedModelDataNode(data);

    for (unsigned int i = 0; i < aiParentNode->mNumMeshes; ++i)
        parentNode->addMeshIndex(aiParentNode->mMeshes[i]);

    parentNode->setName(aiParentNode->mName.C_Str());
    parentNode->setInitialTransformation(toQMatrix(aiParentNode->mTransformation));

    for (unsigned int i = 0; i < aiParentNode->mNumChildren; ++i)
        parentNode->addChild(processNode(data, aiParentNode->mChildren[i]));

    return parentNode;
}

// TODO
QMatrix4x4 Helper::toQMatrix(const aiMatrix4x4 &matrix)
{
    return QMatrix4x4();
}
