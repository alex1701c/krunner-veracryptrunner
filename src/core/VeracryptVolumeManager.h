//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
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

    QMap<QString, VeracryptVolume *> getVeracryptVolumesMap() {
        QMap<QString, VeracryptVolume *> volumes;
        for (auto *volume:readVolumes()) {
            volumes.insert(volume->name, volume);
        }
        return volumes;
    }

    QList<VeracryptVolume *> getVeracryptVolumes() {
        QList<VeracryptVolume *> volumes = readVolumes();
        std::sort(volumes.begin(), volumes.end(), [](VeracryptVolume *volume1, VeracryptVolume *volume2) -> bool {
            return volume1->priority < volume2->priority;
        });

        return volumes;
    }

private:
    QList<VeracryptVolume *> readVolumes() {
        QList<VeracryptVolume *> volumes;
        config.config()->reparseConfiguration();
        for (const auto &volumeName:config.groupList().filter(QRegExp(R"(^(?!General$).*$)"))) {
            auto *volume = new VeracryptVolume();
            KConfigGroup volumeConfig = config.group(volumeName);
            volume->name = volumeName;
            volume->priority = volumeConfig.readEntry("priority").toInt();
            volume->type = volumeConfig.readEntry("type");
            volume->source = volumeConfig.readEntry("source");
            volume->mountPath = volumeConfig.readEntry("mountPath");
            volume->keyFiles = volumeConfig.readEntry("keyFiles")
                    .split(";", QString::SplitBehavior::SkipEmptyParts);
            volume->passPath = volumeConfig.readEntry("passPath");
            volumes.append(volume);
        }
        return volumes;
    }
};

