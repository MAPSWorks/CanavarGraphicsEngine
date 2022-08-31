#ifndef NODESELECTOR_H
#define NODESELECTOR_H

#include "Common.h"
#include "Framebuffer.h"
#include "Manager.h"

#include <QMouseEvent>
#include <QObject>

class RendererManager;
class NodeManager;

class NodeSelector : public Manager, protected QOpenGLExtraFunctions
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

    bool init() override;
    void onMousePressed(QMouseEvent *event);
    void resize(int width, int height);

    static NodeSelector *instance();

private:
    void render();

private:
    NodeManager *mNodeManager;
    RendererManager *mRendererManager;

    Framebuffer *mSelectionFramebuffer;
    FramebufferFormat mFramebufferFormat;
    bool mResizeFlag;

    SelectionInfo mSelectionInfo;
    RenderSettings mRenderSettings;
};

#endif // NODESELECTOR_H
