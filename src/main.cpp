#include <QCoreApplication>
#include <runner/VeracryptRunner.h>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    VeracryptRunner r;
    QCoreApplication::exec();
}