#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "LightManager.h"
#include "ModelData.h"
#include "NodeManager.h"
#include "NozzleEffect.h"
#include "ScreenRenderer.h"
#include "ShaderManager.h"
#include "SkyBox.h"
#include "Terrain.h"

#include <QMap>
#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>

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

    struct Framebuffer {
        unsigned int framebuffer;
        unsigned int texture;
        unsigned int renderObject;
    };

    static RendererManager *instance();

    bool init();
    void render(float ifps);
    void resize(int w, int h);

    bool renderWireframe() const;
    void setRenderWireframe(bool newRenderWireframe);

    bool renderNormals() const;
    void setRenderNormals(bool newRenderNormals);

    bool renderObjects() const;
    void setRenderObjects(bool newRenderObjects);

    const Fog &fog() const;
    void setFog(const Fog &newFog);
    void resetFog();

    bool useBlinnShading() const;
    void setUseBlinnShading(bool newUseBlinnShading);

    ModelData *getModelData(const QString &modelName);

    ModelData *nozzleModel() const;
    void setNozzleModel(ModelData *newNozzleModel);

    NozzleEffect *nozzleEffect() const;
    void setNozzleEffect(NozzleEffect *newNozzleEffect);

private:
    void renderModels(float ifps);
    void renderSkyBox(float ifps);
    void renderTerrain(float ifps);
    void renderParticles(float ifps);
    void renderModel(Model *model);
    void fillFramebuffer(Framebuffer read, Framebuffer draw);
    void applyBlur(Framebuffer stencil, Framebuffer read, bool horizontal);

private:
    QMap<QString, ModelData *> mModelsData;

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

    bool mUseBlinnShading;

    ScreenRenderer *mScreenRenderer;

    Framebuffer mFramebuffers[3];

    int mWindowWidth;
    int mWindowHeight;

    NozzleEffect *mNozzleEffect;
};

#endif // RENDERERMANAGER_H
