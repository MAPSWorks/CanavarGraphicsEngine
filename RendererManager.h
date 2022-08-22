#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "Haze.h"
#include "LightManager.h"
#include "ModelData.h"
#include "NodeManager.h"
#include "NozzleEffect.h"
#include "Quad.h"
#include "ShaderManager.h"
#include "Sky.h"
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
    struct Framebuffer {
        unsigned int framebuffer;
        unsigned int texture;
        unsigned int renderObject;
        unsigned int depthTexture;
    };

    enum class BlurDirection { Horizontal, Vertical };

    static RendererManager *instance();

    bool init();
    void render(float ifps);
    void resize(int w, int h);

    ModelData *getModelData(const QString &modelName);
    void setNozzleModel(ModelData *newNozzleModel);
    void setNozzleEffect(NozzleEffect *newNozzleEffect);
    void drawGui();

private:
    void renderModels(float ifps);
    void renderSkyBox(float ifps);
    void renderTerrain(float ifps);
    void renderSky(float ifps);
    void renderParticles(float ifps);
    void renderModel(Model *model);
    void fillFramebuffer(Framebuffer read, Framebuffer draw);
    bool createFramebuffers();
    void deleteFramebuffers();
    void loadModels();
    void fillStencilBuffer(Framebuffer framebuffer, float ifps);
    void applyNozzleBlur(Framebuffer stencilSource, Framebuffer textureSource);
    void applyMotionBlur(Framebuffer read, Framebuffer draw);

private:
    QMap<QString, ModelData *> mModelsData;

    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    DirectionalLight *mSun;
    SkyBox *mSkyBox;
    Terrain *mTerrain;
    Haze *mHaze;
    Sky *mSky;
    Quad *mQuad;

    bool mRenderObjects;
    bool mRenderWireframe;
    bool mRenderNormals;
    bool mUseBlinnShading;

    Framebuffer mFramebuffers[2];

    int mWidth;
    int mHeight;

    NozzleEffect *mNozzleEffect;

    bool mFlag;

    bool mApplyMotionBlur;
    int mMotionBlurSamples;
};

#endif // RENDERERMANAGER_H
