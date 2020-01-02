#include <QtWidgets/QApplication>
#include <runner/VeracryptRunner.h>
#include "cli/CLIMain.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        QApplication app(argc, argv);
        QApplication::setQuitOnLastWindowClosed(false);
        VeracryptRunner r;
        QApplication::exec();
    } else {
        CLIMain(argc, argv);
    }
}