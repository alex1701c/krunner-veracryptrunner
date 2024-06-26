//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#pragma once

#include "../core/VeracryptVolumeManager.h"
#include "ui_veracryptrunner_config.h"
#include <KCModule>
#include <KConfigGroup>
#include <KSharedConfig>

class VeracryptRunnerConfigForm : public QWidget, public Ui::VeracryptRunnerConfigUi
{
    Q_OBJECT

public:
    explicit VeracryptRunnerConfigForm(QWidget *parent);
};

class VeracryptRunnerConfig : public KCModule
{
    Q_OBJECT

public:
    explicit VeracryptRunnerConfig(QObject *parent, const QVariantList &args);

    ~VeracryptRunnerConfig() override;

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

    void addNewVeracryptItem()
    {
        addVeracryptItem(new VeracryptVolume(), true);
    }

    void confirmedDeleteOfItem();

    void moveItemUp();

    void moveItemDown();

    void validateMoveButtons();

    void validateDuplicateNames();

private:
    QList<VeracryptVolume *> volumes;
    VeracryptRunnerConfigForm *m_ui;
};
