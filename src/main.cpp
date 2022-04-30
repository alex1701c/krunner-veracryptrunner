#include "cli/CLIMain.h"
#include <QApplication>
#include <runner/VeracryptRunner.h>

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
