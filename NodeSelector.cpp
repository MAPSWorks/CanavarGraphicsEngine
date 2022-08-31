#include "NodeSelector.h"
#include "CameraManager.h"

NodeSelector::NodeSelector(QObject *parent)
    : QObject{parent}
    , mSelectionFramebuffer(nullptr)
    , mResizeFlag(false)
{
    mNodeManager = NodeManager::instance();
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mRendererManager = RendererManager::instance();

    mFramebufferFormat.setSamples(0);
    mFramebufferFormat.setWidth(1600);
    mFramebufferFormat.setHeight(900);
    mFramebufferFormat.addColorAttachment(0, //
                                          FramebufferFormat::TextureTarget::TEXTURE_2D,
                                          FramebufferFormat::TextureInternalFormat::RGBA32UI,
                                          FramebufferFormat::TexturePixelFormat::RGBA_INTEGER,
                                          FramebufferFormat::TextureDataType::UNSIGNED_INT);
}

void NodeSelector::init()
{
    initializeOpenGLFunctions();
    mSelectionFramebuffer = new Framebuffer(mFramebufferFormat);
}

void NodeSelector::onMousePressed(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mSelectionFramebuffer->bind();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        render();
        glFinish();

        glReadPixels(event->x(), mFramebufferFormat.height() - event->y(), 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &mSelectionInfo);

        mNodeManager->setSelectedNode(mSelectionInfo.nodeIndex);
        mNodeManager->setSelectedMesh(mSelectionInfo.meshIndex);
        mNodeManager->setSelecteVertex(mSelectionInfo.vertexIndex);
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
            if (mSelectionFramebuffer)
                mSelectionFramebuffer->deleteLater();

            mSelectionFramebuffer = new Framebuffer(mFramebufferFormat);
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
            {
                mShaderManager->bind(ShaderManager::ShaderType::NodeSelectionShader);
                mShaderManager->setUniformValue("VP", mCameraManager->activeCamera()->getVP());
                mShaderManager->setUniformValue("nodeIndex", model->index());
                data->renderForNodeSelector(model);
                mShaderManager->release();
            }
        }
    }
}
