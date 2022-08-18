#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "LightManager.h"
#include "ModelData.h"
#include "NodeManager.h"
#include "ShaderManager.h"
#include "SkyBox.h"
#include "Terrain.h"
#include "TexturedModelData.h"

#include <QMap>
#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLExtraFunctions>

class RendererManager : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    struct Fog {
        bool enabled;
        QVector3D color;
        float density;
        float gradient;
    };

    static RendererManager *instance();

    bool init();
    void render(float ifps);

    bool renderWireframe() const;
    void setRenderWireframe(bool newRenderWireframe);

    bool renderNormals() const;
    void setRenderNormals(bool newRenderNormals);

    bool renderObjects() const;
    void setRenderObjects(bool newRenderObjects);

    const Fog &fog() const;
    void setFog(const Fog &newFog);
    void resetFog();

private slots:
    void renderNode(Node *node);
    void renderSkyBox();
    void renderTerrain();

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
    Terrain *mTerrain;
    Fog mFog;
};

#endif // RENDERERMANAGER_H
