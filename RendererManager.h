#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "Light.h"
#include "LightManager.h"
#include "ModelData.h"
#include "NodeManager.h"
#include "ShaderManager.h"
#include "SimpleTerrainTile.h"
#include "SkyBox.h"
#include "TexturedModelData.h"

#include <QMap>
#include <QMatrix4x4>
#include <QObject>

class RendererManager : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    static RendererManager *instance();

    bool init();
    void render(float ifps);

    bool renderWireframe() const;
    void setRenderWireframe(bool newRenderWireframe);

    bool renderNormals() const;
    void setRenderNormals(bool newRenderNormals);

    bool renderObjects() const;
    void setRenderObjects(bool newRenderObjects);

private slots:
    void renderNode(Node *node);
    void renderSkyBox();

private:
    QMap<Model::Type, ModelData *> mTypeToModelData;
    QMap<QString, TexturedModelData *> mNameToTexturedModelData;

    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    DirectionalLight *mSun;

    bool mRenderObjects;
    bool mRenderWireframe;
    bool mRenderNormals;

    SkyBox *mSkyBox;
    SimpleTerrainTile *mTile;
};

#endif // RENDERERMANAGER_H
