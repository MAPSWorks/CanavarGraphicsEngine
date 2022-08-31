#ifndef COMMON_H
#define COMMON_H

enum class RenderFor : int { //
    Default = 2,
    Reflection = 1,
    Refraction = -1,
    NodeSelector = 3
};

struct RenderSettings {
    RenderFor renderFor;
    float ifps;
    bool useBlinnShading;
    bool renderModels;
    bool renderWireframe;
    bool renderNormals;
};

#endif // COMMON_H
