#include "Cube.h"

Cube::Cube(QObject *parent)
    : QObject{parent}
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
}

void Cube::render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

Cube *Cube::instance()
{
    static Cube instance;

    return &instance;
}

const float Cube::VERTICES[108] = {-0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  0.0175f, 0.0175f,  0.0175f,  -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f,
                                   0.0175f,  -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  -0.0175f, -0.0175f, -0.0175f, 0.0175f, -0.0175f, -0.0175f, 0.0175f,  0.0175f,  -0.0175f, 0.0175f,  -0.0175f,
                                   -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, -0.0175f, 0.0175f, 0.0175f,  -0.0175f, 0.0175f,  -0.0175f, 0.0175f,  -0.0175f, 0.0175f,
                                   -0.0175f, -0.0175f, 0.0175f,  -0.0175f, -0.0175f, -0.0175f, -0.0175f, 0.0175f,  0.0175f, -0.0175f, -0.0175f, 0.0175f,  0.0175f,  -0.0175f, 0.0175f,  0.0175f,
                                   0.0175f,  0.0175f,  0.0175f,  -0.0175f, -0.0175f, 0.0175f,  0.0175f,  -0.0175f, 0.0175f, -0.0175f, -0.0175f, 0.0175f,  0.0175f,  0.0175f,  0.0175f,  -0.0175f,
                                   0.0175f,  0.0175f,  0.0175f,  0.0175f,  0.0175f,  0.0175f,  -0.0175f, -0.0175f, 0.0175f, -0.0175f, 0.0175f,  0.0175f,  0.0175f,  -0.0175f, 0.0175f,  -0.0175f,
                                   -0.0175f, 0.0175f,  0.0175f,  0.0175f,  0.0175f,  0.0175f,  -0.0175f, 0.0175f,  0.0175f, 0.0175f,  -0.0175f, 0.0175f};
