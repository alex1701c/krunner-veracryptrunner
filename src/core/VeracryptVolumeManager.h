#pragma once

#include <QDebug>
#include <KSharedConfig>
#include <KConfigCore/KConfigGroup>
#include "VeracryptVolume.h"

class VeracryptVolumeManager {
public:
    KConfigGroup config;

    VeracryptVolumeManager() {
        this->config = KSharedConfig::openConfig("veracryptrunnerrc")->group("Configs");
    }

    QList<VeracryptVolume> getVeracryptVolumes() {
        QList<VeracryptVolume> volumes;
        for (const auto &volumeName:config.groupList().filter(QRegExp(R"(^(?!General$).*$)"))) {
            VeracryptVolume volume;
            KConfigGroup volumeConfig = config.group(volumeName);
            volume.name = volumeName;
            volume.id = volumeConfig.readEntry("id").toInt();
            volume.priority = volumeConfig.readEntry("priority").toInt();
            volume.type = volumeConfig.readEntry("type");
            volume.source = volumeConfig.readEntry("source");
            volume.mountPath = volumeConfig.readEntry("mountPath");
            volume.keyFiles = volumeConfig.readEntry("keyFiles")
                    .split(";", QString::SplitBehavior::SkipEmptyParts);
            volume.passPath = volumeConfig.readEntry("passPath");
            volumes.append(volume);
        }
        return volumes;
    }
};

