#include "veracryptrunner_config.h"
#include "VeracryptConfigItem.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <krunner/abstractrunner.h>
#include <QtGui>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>


K_PLUGIN_FACTORY(VeracryptRunnerConfigFactory, registerPlugin<VeracryptRunnerConfig>("kcm_krunner_veracryptrunner2");)
// TODO Get rid of id and use name as identifier


VeracryptRunnerConfigForm::VeracryptRunnerConfigForm(QWidget *parent) : QWidget(parent) {
    setupUi(this);
}


VeracryptRunnerConfig::VeracryptRunnerConfig(QWidget *parent, const QVariantList &args) : KCModule(parent, args) {
    m_ui = new VeracryptRunnerConfigForm(this);
    auto *layout = new QGridLayout(this);
    layout->addWidget(m_ui, 0, 0);

    connect(m_ui->pushButton, &QPushButton::clicked, this,
            static_cast<void (VeracryptRunnerConfig::*)()>(&VeracryptRunnerConfig::changed));
    connect(m_ui->pushButton, &QPushButton::clicked, this,
            static_cast<void (VeracryptRunnerConfig::*)()>(&VeracryptRunnerConfig::addVeracryptItem));
}

VeracryptRunnerConfig::~VeracryptRunnerConfig() {
    qDeleteAll(volumes);
}

void VeracryptRunnerConfig::load() {
    if (!volumes.isEmpty()) {
        qDeleteAll(volumes);
    }
    volumes = manager.getVeracryptVolumes();
    for (auto *volume:volumes) {
        addVeracryptItem(volume, false);
    }
    validateMoveButtons();
}


void VeracryptRunnerConfig::save() {
    QList<VeracryptConfigItem *> configItemsUi;
    const int itemCount = m_ui->veracryptVolumes->count();
    for (int i = 0; i < itemCount; ++i) {
        configItemsUi.append(reinterpret_cast<VeracryptConfigItem *>( m_ui->veracryptVolumes->itemAt(i)->widget()));
    }
    auto config = KSharedConfig::openConfig("veracryptrunnerrc")->group("Configs");
    for (const auto &volumeGroupName:config.groupList().filter(QRegExp(R"(^(?!General$).*$)"))) {
        config.group(volumeGroupName).deleteGroup();
    }
    for (auto *item:configItemsUi) {
        auto group = config.group(item->nameLineEdit->text());
        group.writeEntry("id", item->idLabel->text());
        group.writeEntry("priority", 100 - configItemsUi.indexOf(item));
        group.writeEntry("type", item->fileRadioButton->isChecked() ? "FILE" : "DEVICE");
        group.writeEntry("source", item->fileRadioButton->isChecked() ? item->filePushButton->text().remove('&') :
                                   item->devicePushButton->text().remove('&'));
        group.writeEntry("mountPath", item->mountPath->text().remove('&'));
        QString keyFilesString;
        int count = item->keyFileListWidget->model()->rowCount();
        for (int i = 0; i < count; ++i) keyFilesString.append(item->keyFileListWidget->item(i)->text() + ";");
        group.writeEntry("keyFiles", keyFilesString);
        group.writeEntry("passPath", item->passIntegration->text());
    }
}

void VeracryptRunnerConfig::defaults() {
    emit changed(true);
}

void VeracryptRunnerConfig::addVeracryptItem(VeracryptVolume *volume, bool validate) {
    if (volume->id == -1) {
        volume->id = manager.config.group("General").readEntry("id", "1").toInt();
        manager.config.group("General").writeEntry("id", volume->id + 1);
    }
    auto *element = new VeracryptConfigItem(this, volume);
    m_ui->veracryptVolumes->insertWidget(0, element);
    connect(element, &VeracryptConfigItem::changed, this,
            static_cast<void (VeracryptRunnerConfig::*)()>(&VeracryptRunnerConfig::changed));
    connect(element, &VeracryptConfigItem::confirmedDelete, this, &VeracryptRunnerConfig::confirmedDeleteOfItem);
    connect(element, &VeracryptConfigItem::moveItemUp, this, &VeracryptRunnerConfig::moveItemUp);
    connect(element, &VeracryptConfigItem::moveItemDown, this, &VeracryptRunnerConfig::moveItemDown);
    if (validate) validateMoveButtons();
}

void VeracryptRunnerConfig::addVeracryptItem() {
    addVeracryptItem(new VeracryptVolume(), true);
}

void VeracryptRunnerConfig::confirmedDeleteOfItem() {
    auto *item = dynamic_cast<VeracryptConfigItem *>(this->sender());
    m_ui->veracryptVolumes->removeWidget(item);
    item->deleteLater();
    validateMoveButtons();
}

void VeracryptRunnerConfig::moveItemUp() {
    auto *item = reinterpret_cast<VeracryptConfigItem *>(sender());
    const int idx = m_ui->veracryptVolumes->indexOf(item);
    m_ui->veracryptVolumes->removeWidget(item);
    m_ui->veracryptVolumes->insertWidget(idx - 1, item);
    m_ui->scrollArea->ensureWidgetVisible(m_ui->veracryptVolumes->itemAt(idx)->widget());
    validateMoveButtons();
}

void VeracryptRunnerConfig::moveItemDown() {
    auto *item = reinterpret_cast<VeracryptConfigItem *>(sender());
    const int idx = m_ui->veracryptVolumes->indexOf(item);
    m_ui->veracryptVolumes->removeWidget(item);
    m_ui->veracryptVolumes->insertWidget(idx + 1, item);
    validateMoveButtons();
    m_ui->scrollArea->ensureWidgetVisible(m_ui->veracryptVolumes->itemAt(idx)->widget());
}

void VeracryptRunnerConfig::validateMoveButtons() {
    const int itemCount = m_ui->veracryptVolumes->count();
    for (int i = 0; i < itemCount; ++i) {
        const auto *item = reinterpret_cast<VeracryptConfigItem *>( m_ui->veracryptVolumes->itemAt(i)->widget());
        item->moveUp->setDisabled(i == 0);
        item->moveDown->setDisabled(i == itemCount - 1);
    }
}


#include "veracryptrunner_config.moc"

