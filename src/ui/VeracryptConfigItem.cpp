//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>
#include <QStorageInfo>
#include <QStringBuilder>

#include "../core/VeracryptVolumeManager.h"
#include "VeracryptConfigItem.h"

VeracryptConfigItem::VeracryptConfigItem(QWidget *parent, VeracryptVolume *volume) : QWidget(parent) {
    setupUi(this);
    this->volume = volume;
    this->nameExistsInfoLabel->setHidden(true);
    initializeValues();

    connect(this->nameLineEdit, &QLineEdit::textChanged, this, &VeracryptConfigItem::changed);
    // To validate duplicate names
    connect(this->nameLineEdit, &QLineEdit::textChanged, this, &VeracryptConfigItem::nameChanged);
    // Delete Signals
    connect(this->deleteConfigButton, &QPushButton::clicked, this, &VeracryptConfigItem::deleteConfig);
    connect(this->deleteConfigButton, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    // Connect move up/down signals to parent
    connect(this->moveUp, &QPushButton::clicked, this, &VeracryptConfigItem::moveItemUp);
    connect(this->moveUp, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    connect(this->moveDown, &QPushButton::clicked, this, &VeracryptConfigItem::moveItemDown);
    connect(this->moveDown, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    // Connect type signals
    connect(this->fileRadioButton, &QRadioButton::clicked, this, &VeracryptConfigItem::toggleVolumeSource);
    connect(this->deviceRadioButton, &QRadioButton::clicked, this, &VeracryptConfigItem::toggleVolumeSource);
    connect(this->fileRadioButton, &QRadioButton::clicked, this, &VeracryptConfigItem::changed);
    connect(this->deviceRadioButton, &QRadioButton::clicked, this, &VeracryptConfigItem::changed);
    // Connect file/device/path pickers
    connect(this->filePushButton, &QPushButton::clicked, this, &VeracryptConfigItem::openVolumeFilePicker);
    connect(this->filePushButton, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    connect(this->devicePushButton, &QPushButton::clicked, this, &VeracryptConfigItem::openVolumeDevicePicker);
    connect(this->devicePushButton, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    connect(this->mountPath, &QPushButton::clicked, this, &VeracryptConfigItem::openMountPathPicker);
    connect(this->mountPath, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    // Key file controls
    connect(this->keyFileListWidget, &QListWidget::currentRowChanged, this, &VeracryptConfigItem::validateKeyFileControls);
    connect(this->addKeyFileButton, &QPushButton::clicked, this, &VeracryptConfigItem::openKeyFilePicker);
    connect(this->addKeyFileButton, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    connect(this->removeKeyFileButton, &QPushButton::clicked, this, &VeracryptConfigItem::removeKeyFile);
    connect(this->removeKeyFileButton, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
    // Signals for pass integration
    connect(this->passIntegration, &QLineEdit::textChanged, this, &VeracryptConfigItem::changed);
    connect(this->passIntegrationSelectButton, &QPushButton::clicked, this, &VeracryptConfigItem::passFilePicker);
    connect(this->passIntegrationSelectButton, &QPushButton::clicked, this, &VeracryptConfigItem::changed);
}

void VeracryptConfigItem::toggleVolumeSource() {
    bool fileSource = this->fileRadioButton->isChecked();
    this->filePushButton->setHidden(!fileSource);
    this->devicePushButton->setHidden(fileSource);
}

void VeracryptConfigItem::initializeValues() {
    this->nameLineEdit->setText(volume->name.isEmpty() ? QStringLiteral("New Volume") : volume->name);
    // Initialize type and source
    bool fileType = this->volume->type == QLatin1String("FILE");
    if (fileType) {
        this->filePushButton->setText(volume->source.isEmpty() ? QStringLiteral("Select File") : volume->source);
    } else {
        this->devicePushButton->setText(volume->source.isEmpty() ? QStringLiteral("Select Device") : volume->source);
    }
    this->fileRadioButton->setChecked(fileType);
    this->deviceRadioButton->setChecked(!fileType);
    toggleVolumeSource();

    // Initialize Key Files
    for (const auto &keyFile:volume->keyFiles) {
        this->keyFileListWidget->addItem(keyFile);
    }
    validateKeyFileControls();

    // Location and Pass paths
    this->mountPath->setText(
            volume->mountPath.isEmpty() ? QStringLiteral("/media/veracrypt") : volume->mountPath);
    this->passIntegration->setText(volume->passPath);
}

void VeracryptConfigItem::openVolumeFilePicker() {
    const QString volumePath = QFileDialog::getOpenFileName(this, tr("Select Volume"), QDir::homePath(),
                                                            tr("Veracrypt Volumes (*.hc)"));
    if (!volumePath.isEmpty()) {
        this->filePushButton->setText(volumePath);
    }
}

void VeracryptConfigItem::openVolumeDevicePicker() {
    QProcess process;
    process.start(QStringLiteral("lsblk"), QStringList{QStringLiteral("-o"), QStringLiteral("NAME,LABEL,SIZE,MOUNTPOINT")});
    process.waitForFinished(-1);
    const QStringList devices = QString(process.readAllStandardOutput())
            .split(QStringLiteral("\n"), Qt::SkipEmptyParts)
            .filter(QRegExp(QStringLiteral(R"(.*sd.*)")));
    const QString deviceInfo = QInputDialog::getItem(this, QStringLiteral("Select Device"),
                                                     QStringLiteral("Name Label Size Mount Point"), devices);
    if (!deviceInfo.isEmpty()) {
        QRegExp deviceRegex(QStringLiteral(R"((sd[a-z]\d*))"));
        deviceRegex.indexIn(deviceInfo);
        this->devicePushButton->setText(QStringLiteral("/dev/") + deviceRegex.cap(1));
    }
}

void VeracryptConfigItem::openMountPathPicker() {
    const QString mountPath = QFileDialog::getExistingDirectory(this, tr("Select Mount Directory"),
                                                                this->mountPath->text(),
                                                                QFileDialog::ShowDirsOnly);
    if (!mountPath.isEmpty()) {
        this->mountPath->setText(mountPath);
    }
}

void VeracryptConfigItem::openKeyFilePicker() {
    const QStringList keyFilePaths = QFileDialog::getOpenFileNames(
            this, tr("Select Key File"), QDir::homePath(), tr("Key Files (*)"));
    if (!keyFilePaths.isEmpty()) {
        this->keyFileListWidget->addItems(keyFilePaths);
    }
    validateKeyFileControls();
}

void VeracryptConfigItem::removeKeyFile() {
    const int currentIndex = this->keyFileListWidget->currentIndex().row();
    if (currentIndex == -1) return;
    delete this->keyFileListWidget->takeItem(currentIndex);
    validateKeyFileControls();
}

void VeracryptConfigItem::validateKeyFileControls() {
    const bool empty = this->keyFileListWidget->model()->rowCount() == 0;
    this->keyFileListWidget->setHidden(empty);
    this->removeKeyFileButton->setDisabled(empty || this->keyFileListWidget->currentIndex().row() == -1);
}

void VeracryptConfigItem::deleteConfig() {
    const auto res = QMessageBox::question(this, QStringLiteral("Delete Entry ?"),
                                           QStringLiteral("Do You Want To Delete This Entry ?"),
                                           QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes) {
        // Deletes Widget and remove it from list in parent
        emit confirmedDelete();
    }
}

void VeracryptConfigItem::passFilePicker() {
    const QString passPath = QFileDialog::getOpenFileName(this, tr("Select Pass File"),
                                                          QDir::homePath() % QStringLiteral("/.password-store"),
                                                          tr("Pass File (*.gpg)"));
    if (!passPath.isEmpty()) {
        QRegExp passRegex(QStringLiteral(R"(^.*/\.password-store/(.*)\.gpg$)"));
        passRegex.indexIn(passPath);
        const QString res = passRegex.cap(1);
        if (!res.isEmpty()) {
            this->passIntegration->setText(res);
        }
    }
}
