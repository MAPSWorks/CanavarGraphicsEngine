#ifndef NODESELECTOR_H
#define NODESELECTOR_H

#include "Framebuffer.h"
#include "NodeManager.h"
#include "RendererManager.h"
#include "ShaderManager.h"

#include <QMouseEvent>
#include <QObject>

class NodeSelector : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
private:
    explicit NodeSelector(QObject *parent = nullptr);

public:
    struct SelectionInfo {
        unsigned int nodeIndex;
        unsigned int meshIndex;
        unsigned int vertexIndex;
    };

    void init();
    void onMousePressed(QMouseEvent *event);
    void resize(int width, int height);

    static NodeSelector *instance();

private:
    void render();

private:
    NodeManager *mNodeManager;
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    RendererManager *mRendererManager;

    Framebuffer *mSelectionFramebuffer;
    FramebufferFormat mFramebufferFormat;
    bool mResizeFlag;

    SelectionInfo mSelectionInfo;
};

#endif // NODESELECTOR_H
