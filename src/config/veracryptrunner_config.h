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

    virtual ~VeracryptRunnerConfig();

    VeracryptVolumeManager manager;

public Q_SLOTS:

    void save() override;

    void load() override;

    void defaults() override;

    /**
     * Adds the volume to the UI
     * @param volume VeracryptVolume which contains the information
     * @param validate if the move buttons should be validated
     */
    void addVeracryptItem(VeracryptVolume *volume, bool validate);

    /**
     * Overload
     */
    void addVeracryptItem();

    void confirmedDeleteOfItem();

    void moveItemUp();

    void moveItemDown();

    void validateMoveButtons();

private:
    QList<VeracryptVolume *> volumes;
    VeracryptRunnerConfigForm *m_ui;
};

