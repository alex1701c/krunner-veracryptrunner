//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#include "veracryptrunner_config.h"
#include "../ui/VeracryptConfigItem.h"
#include <KPluginFactory>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

K_PLUGIN_FACTORY(VeracryptRunnerConfigFactory, registerPlugin<VeracryptRunnerConfig>();)

VeracryptRunnerConfigForm::VeracryptRunnerConfigForm(QWidget *parent) : QWidget(parent) {
    setupUi(this);
}

VeracryptRunnerConfig::VeracryptRunnerConfig(QWidget *parent, const QVariantList &args) : KCModule(parent, args) {
    m_ui = new VeracryptRunnerConfigForm(this);
    auto *layout = new QGridLayout(this);
    layout->addWidget(m_ui, 0, 0);

    connect(m_ui->pushButton, &QPushButton::clicked, this, &VeracryptRunnerConfig::markAsChanged);
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
    auto config = KSharedConfig::openConfig(QStringLiteral("veracryptrunnerrc"))->group("Configs");
    for (const auto &volumeGroupName:config.groupList().filter(QRegExp(R"(^(?!General$).*$)"))) {
        config.group(volumeGroupName).deleteGroup();
    }
    QStringList savedNames;
    for (auto *item:configItemsUi) {
        QString name = item->nameLineEdit->text();
        if (savedNames.contains(name)) {
            // Add number to name if it is already taken and the warning is ignored
            for (int i = 1; i < 11; ++i) {
                QString _name = name + QString::number(i);
                if (!savedNames.contains(_name)) {
                    name = _name;
                    i = 11;
                }
            }
        }
        savedNames.append(name);
        auto group = config.group(name);
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
    auto *element = new VeracryptConfigItem(this, volume);
    m_ui->veracryptVolumes->insertWidget(0, element);
    connect(element, &VeracryptConfigItem::changed, this, &VeracryptRunnerConfig::markAsChanged);
    connect(element, &VeracryptConfigItem::confirmedDelete, this, &VeracryptRunnerConfig::confirmedDeleteOfItem);
    connect(element, &VeracryptConfigItem::moveItemUp, this, &VeracryptRunnerConfig::moveItemUp);
    connect(element, &VeracryptConfigItem::moveItemDown, this, &VeracryptRunnerConfig::moveItemDown);
    connect(element, &VeracryptConfigItem::nameChanged, this, &VeracryptRunnerConfig::validateDuplicateNames);
    if (validate) {
        emit element->nameChanged();
        validateMoveButtons();
    }
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

void VeracryptRunnerConfig::validateDuplicateNames() {
    const auto *senderItem = reinterpret_cast<VeracryptConfigItem *>(sender());
    const int itemCount = m_ui->veracryptVolumes->count();
    QStringList names;

    for (int i = 0; i < itemCount; ++i) {
        const auto *item = reinterpret_cast<VeracryptConfigItem *>( m_ui->veracryptVolumes->itemAt(i)->widget());
        if (item != senderItem) {
            names.append(item->nameLineEdit->text());
        }
    }
    senderItem->nameExistsInfoLabel->setHidden(!names.contains(senderItem->nameLineEdit->text()));
}

#include "veracryptrunner_config.moc"
