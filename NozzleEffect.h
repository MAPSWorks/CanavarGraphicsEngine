#ifndef NOZZLEEFFECT_H
#define NOZZLEEFFECT_H

#include "ModelData.h"
#include "Node.h"

class NozzleEffect : public Node
{
public:
    explicit NozzleEffect(QObject *parent = nullptr);
    virtual ~NozzleEffect();

    void render();

    ModelData *modelData() const;
    void setModelData(ModelData *newModelData);

private:
    ModelData *mModelData;
};

#endif // NOZZLEEFFECT_H
