//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#pragma once

#include "../core/VeracryptVolume.h"
#include "ui_veracryptrunner_config_item.h"
#include <KConfigGroup>

class VeracryptConfigItem : public QWidget, public Ui::VeracryptItemUi
{
    Q_OBJECT

public:
    VeracryptVolume *volume;

    explicit VeracryptConfigItem(QWidget *parent, VeracryptVolume *volume = new VeracryptVolume());

    void initializeValues();

public Q_SLOTS:
    void toggleVolumeSource();
    void openVolumeFilePicker();
    void openVolumeDevicePicker();
    void openMountPathPicker();
    void validateKeyFileControls();
    void openKeyFilePicker();
    void removeKeyFile();
    void deleteConfig();
    void passFilePicker();

Q_SIGNALS:
    void changed();
    void moveItemUp();
    void moveItemDown();
    void confirmedDelete();
    void nameChanged();
};
