#include "NodeSelector.h"
#include "NodeManager.h"
#include "RendererManager.h"

NodeSelector::NodeSelector(QObject *parent)
    : Manager(parent)
    , mMeshSelectionFramebuffer(nullptr)
    , mResizeFlag(false)
{
    mNodeManager = NodeManager::instance();
    mRendererManager = RendererManager::instance();

    mFramebufferFormat.setSamples(0);
    mFramebufferFormat.setWidth(1600);
    mFramebufferFormat.setHeight(900);
    mFramebufferFormat.addColorAttachment(0, //
                                          FramebufferFormat::TextureTarget::TEXTURE_2D,
                                          FramebufferFormat::TextureInternalFormat::RGB32UI,
                                          FramebufferFormat::TexturePixelFormat::RGB_INTEGER,
                                          FramebufferFormat::TextureDataType::UNSIGNED_INT);
}

bool NodeSelector::init()
{
    initializeOpenGLFunctions();
    mMeshSelectionFramebuffer = new Framebuffer(mFramebufferFormat);
    mVertexSelectionFramebuffer = new Framebuffer(mFramebufferFormat);
    return true;
}

void NodeSelector::onMousePressed(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // If vertex selection is enabled then DO NOT select mesh
        if (!mNodeManager->vertexSelectionEnabled())
        {
            mRenderSettings.renderFor = RenderFor::NodeSelectorMeshes;
            mMeshSelectionFramebuffer->bind();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            render();
            glFinish();

            unsigned int info[3];
            glReadPixels(event->x(), mFramebufferFormat.height() - event->y(), 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, info);

            mNodeManager->setSelectedNode(info[0]);

            if (mNodeManager->meshSelectionEnabled())
                mNodeManager->setSelectedMesh(info[1]);
        }

        if (mNodeManager->vertexSelectionEnabled())
        {
            mRenderSettings.renderFor = RenderFor::NodeSelectorVertices;
            mVertexSelectionFramebuffer->bind();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            render();
            glFinish();

            unsigned int info[3];

            glReadPixels(event->x(), mFramebufferFormat.height() - event->y(), 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, info);
            if (info[2] == 1)
                mNodeManager->setSelectedVertexIndex(info[0]);
            else
                mNodeManager->setSelectedVertexIndex(-1);
        }
    }
}

void NodeSelector::resize(int width, int height)
{
    mFramebufferFormat.setWidth(width);
    mFramebufferFormat.setHeight(height);

    if (!mResizeFlag)
    {
        mResizeFlag = true;

        QTimer::singleShot(250, [=]() {
            if (mMeshSelectionFramebuffer)
                mMeshSelectionFramebuffer->deleteLater();

            if (mVertexSelectionFramebuffer)
                mVertexSelectionFramebuffer->deleteLater();

            mMeshSelectionFramebuffer = new Framebuffer(mFramebufferFormat);
            mVertexSelectionFramebuffer = new Framebuffer(mFramebufferFormat);
            mResizeFlag = false;
        });
    }
}

NodeSelector *NodeSelector::instance()
{
    static NodeSelector instance;
    return &instance;
}

void NodeSelector::render()
{
    auto nodes = mNodeManager->nodes();

    for (const auto node : qAsConst(nodes))
    {
        Model *model = dynamic_cast<Model *>(node);
        if (model)
        {
            auto data = mRendererManager->getModelData(model->modelName());

            if (data)
                data->render(model, mRenderSettings);
        }
    }
}
