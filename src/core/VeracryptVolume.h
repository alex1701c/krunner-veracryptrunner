//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#ifndef VERACRYPT_VOLUME_H
#define VERACRYPT_VOLUME_H

#include <QtCore/QString>
#include <QtCore/QStringList>

enum VolumeType {
    File,
    Device
};

class VeracryptVolume {

public:
    int priority = 0;
    QString name;
    QString type;
    QString source;
    QString mountPath;
    QStringList keyFiles;
    QString passPath;
};
#endif // VERACRYPT_VOLUME_H
