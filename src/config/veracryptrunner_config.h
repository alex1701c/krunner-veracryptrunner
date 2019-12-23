#pragma once

#include "VeracryptConfigItem.h"
#include "ui_veracryptrunner_config.h"
#include <KCModule>
#include <KConfigCore/KConfigGroup>
#include "../core/VeracryptVolume.h"
#include "../core/VeracryptVolumeManager.h"

class VeracryptRunnerConfigForm : public QWidget, public Ui::VeracryptRunnerConfigUi {
Q_OBJECT

public:
    explicit VeracryptRunnerConfigForm(QWidget *parent);
};

class VeracryptRunnerConfig : public KCModule {
Q_OBJECT

public:
    explicit VeracryptRunnerConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());

    VeracryptVolumeManager manager;

public Q_SLOTS:

    void save() override;

    void load() override;

    void defaults() override;

    void addVeracryptItem(VeracryptVolume *volume = new VeracryptVolume(), bool validate = true);

    void confirmedDeleteOfItem();

    void moveItemUp();

    void moveItemDown();

    void validateMoveButtons();

private:
    VeracryptRunnerConfigForm *m_ui;
};

