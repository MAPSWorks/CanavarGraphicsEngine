#ifndef COMMON_H
#define COMMON_H

enum class RenderFor : int { //
    Refraction = -1,
    Reflection = 1,
    Default = 2,
    NodeSelectorMeshes = 3,
    NodeSelectorVertices = 4
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
