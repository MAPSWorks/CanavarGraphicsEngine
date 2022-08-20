#include "SkyBox.h"
#include "qdebug.h"

#include <QImage>

SkyBox::SkyBox(QObject *parent)
    : QObject{parent}
{}

SkyBox::~SkyBox()
{
    // TODO
}

void SkyBox::setPath(unsigned int key, const QString &path)
{
    mPaths.insert(key, path);
}

void SkyBox::create()
{
    initializeOpenGLFunctions();

    mVertexArray.create();
    mVertexArray.bind();

    // Vertices (Position)
    mVertexBuffer.create();
    mVertexBuffer.bind();
    mVertexBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    mVertexBuffer.allocate(VERTICES, sizeof(VERTICES));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          3 * sizeof(float), // Stride
                          nullptr            // Offset
    );
    mVertexBuffer.release();
    mVertexArray.release();

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    auto keys = mPaths.keys();

    for (auto key : qAsConst(keys))
    {
        QImage image(mPaths[key]);

        if (image.isNull())
        {
            qWarning() << "Could not load image for SkyBox:" << mPaths[key];
            continue;
        }

        image = image.convertToFormat(QImage::Format_RGBA8888);

        glTexImage2D(key, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        qInfo() << "Texture" << mPaths[key] << "for SkyBox is loaded."
                << "ID is" << mTextureId;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SkyBox::render()
{
    glDepthFunc(GL_LEQUAL);
    mVertexArray.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}

const float SkyBox::VERTICES[108] = {
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  1.0f,  -1.0f, //
    -1.0f, 1.0f,  -1.0f, //

    -1.0f, -1.0f, 1.0f,  //
    -1.0f, -1.0f, -1.0f, //
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, 1.0f,  -1.0f, //
    -1.0f, 1.0f,  1.0f,  //
    -1.0f, -1.0f, 1.0f,  //

    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, 1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  -1.0f, //
    1.0f,  -1.0f, -1.0f, //

    -1.0f, -1.0f, 1.0f, //
    -1.0f, 1.0f,  1.0f, //
    1.0f,  1.0f,  1.0f, //
    1.0f,  1.0f,  1.0f, //
    1.0f,  -1.0f, 1.0f, //
    -1.0f, -1.0f, 1.0f, //

    -1.0f, 1.0f,  -1.0f, //
    1.0f,  1.0f,  -1.0f, //
    1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  1.0f,  //
    -1.0f, 1.0f,  1.0f,  //
    -1.0f, 1.0f,  -1.0f, //

    -1.0f, -1.0f, -1.0f, //
    -1.0f, -1.0f, 1.0f,  //
    1.0f,  -1.0f, -1.0f, //
    1.0f,  -1.0f, -1.0f, //
    -1.0f, -1.0f, 1.0f,  //
    1.0f,  -1.0f, 1.0f   //
};
