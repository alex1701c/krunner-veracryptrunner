#pragma once

#include "ui_veracryptrunner_config_item.h"
#include <KConfigCore/KConfigGroup>
#include "../core/VeracryptVolumeManager.h"


class VeracryptConfigItem : public QWidget, public Ui::VeracryptItemUi {
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

public:
Q_SIGNALS:

    void changed();

    void moveItemUp();

    void moveItemDown();

    void confirmedDelete();
};

