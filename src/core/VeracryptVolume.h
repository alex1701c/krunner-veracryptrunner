#pragma once

#include <QtCore/QString>
#include <QtCore/QStringList>

enum VolumeType {
    File,
    Device
};

class VeracryptVolume {

public:
    int id = -1;
    int priority = 0;
    QString name;
    QString type;
    QString source;
    QString mountPath;
    QStringList keyFiles;
    QString passPath;
};

