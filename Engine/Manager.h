#ifndef MANAGER_H
#define MANAGER_H

#include <QMouseEvent>
#include <QObject>

namespace Canavar {
namespace Engine {

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);

    virtual bool init() = 0;
    virtual void mouseDoubleClicked(QMouseEvent *event);
    virtual void mousePressed(QMouseEvent *event);
    virtual void mouseReleased(QMouseEvent *event);
    virtual void mouseMoved(QMouseEvent *event);
    virtual void wheelMoved(QWheelEvent *event);
    virtual void keyPressed(QKeyEvent *event);
    virtual void keyReleased(QKeyEvent *event);
    virtual void resize(int width, int height);
    virtual void update(float ifps);
    virtual void render(float ifps);
    virtual void reset();
};

} // namespace Engine
} // namespace Canavar

#endif // MANAGER_H
