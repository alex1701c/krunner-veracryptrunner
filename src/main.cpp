#include <QCoreApplication>
#include <runner/VeracryptRunner.h>
#include <QtWidgets/QApplication>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    VeracryptRunner r;
    QApplication::exec();
}