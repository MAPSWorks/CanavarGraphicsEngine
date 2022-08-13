#include "Controller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Controller controller(&app);
    controller.run();

    return app.exec();
}
