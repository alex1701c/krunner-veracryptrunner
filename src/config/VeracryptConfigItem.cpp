#include <QtWidgets/QFileDialog>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QtCore/QStorageInfo>
#include <QtCore/QProcess>
#include <QtWidgets/QInputDialog>
#include "../core/VeracryptVolumeManager.h"
#include "VeracryptConfigItem.h"

VeracryptConfigItem::VeracryptConfigItem(QWidget *parent, VeracryptVolume *volume) : QWidget(parent) {
    setupUi(this);
    this->volume = volume;
    initializeValues();
    // General Signals
    connect(this->nameLineEdit, SIGNAL(textChanged(QString)), parent, SLOT(changed()));
    // Delete Signals
    connect(this->deleteConfigButton, SIGNAL(clicked(bool)), this, SLOT(deleteConfig()));
    connect(this->deleteConfigButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    connect(this, SIGNAL(confirmedDelete()), parent, SLOT(confirmedDeleteOfItem()));
    connect(this, SIGNAL(confirmedDelete()), parent, SLOT(validateMoveButtons()));
    // Connect move up/down signals to parent
    connect(this->moveUp, SIGNAL(clicked(bool)), parent, SLOT(moveItemUp()));
    connect(this->moveUp, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    connect(this->moveDown, SIGNAL(clicked(bool)), parent, SLOT(moveItemDown()));
    connect(this->moveDown, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    // Connect type signals
    connect(this->fileRadioButton, SIGNAL(clicked(bool)), this, SLOT(toggleVolumeSource()));
    connect(this->deviceRadioButton, SIGNAL(clicked(bool)), this, SLOT(toggleVolumeSource()));
    connect(this->fileRadioButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    connect(this->deviceRadioButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    // Connect file/device/path pickers
    connect(this->filePushButton, SIGNAL(clicked(bool)), this, SLOT(openVolumeFilePicker()));
    connect(this->filePushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    connect(this->devicePushButton, SIGNAL(clicked(bool)), this, SLOT(openVolumeDevicePicker()));
    connect(this->devicePushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    connect(this->mountPath, SIGNAL(clicked(bool)), this, SLOT(openMountPathPicker()));
    connect(this->mountPath, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    // Key file controls
    connect(this->keyFileListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(validateKeyFileControls()));
    connect(this->addKeyFileButton, SIGNAL(clicked(bool)), this, SLOT(openKeyFilePicker()));
    connect(this->addKeyFileButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    connect(this->removeKeyFileButton, SIGNAL(clicked(bool)), this, SLOT(removeKeyFile()));
    connect(this->removeKeyFileButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
    // Signals for pass integration
    connect(this->passIntegration, SIGNAL(textChanged(QString)), parent, SLOT(changed()));
    connect(this->passIntegrationSelectButton, SIGNAL(clicked(bool)), this, SLOT(passFilePicker()));
    connect(this->passIntegrationSelectButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
}

void VeracryptConfigItem::toggleVolumeSource() {
    bool fileSource = this->fileRadioButton->isChecked();
    this->filePushButton->setHidden(!fileSource);
    this->devicePushButton->setHidden(fileSource);
}

void VeracryptConfigItem::initializeValues() {
    this->nameLineEdit->setText(volume->name.isEmpty() ? "New Volume " + QString::number(volume->id) : volume->name);
    this->idLabel->setText(QString::number(volume->id));
    this->idLabel->hide();
    // Initialize type and source
    bool fileType = this->volume->type == "FILE";
    if (fileType) {
        this->filePushButton->setText(volume->source.isEmpty() ? "Select File" : volume->source);
    } else {
        this->devicePushButton->setText(volume->source.isEmpty() ? "Select Device" : volume->source);
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
    this->mountPath->setText(volume->mountPath.isEmpty() ? "/media/veracrypt" + QString::number(volume->id) : volume->mountPath);
    this->passIntegration->setText(volume->passPath);
}

void VeracryptConfigItem::openVolumeFilePicker() {
    QString volumePath = QFileDialog::getOpenFileName(this, tr("Select Volume"), QDir::homePath(), tr("Veracrypt Volumes (*.hc)"));
    if (!volumePath.isEmpty()) this->filePushButton->setText(volumePath);
}

void VeracryptConfigItem::openVolumeDevicePicker() {
    QProcess process;
    process.start("lsblk", QStringList() << "-o" << "NAME,LABEL,SIZE,MOUNTPOINT");
    process.waitForFinished(-1);
    QStringList devices = QString(process.readAllStandardOutput()).split("\n", QString::SkipEmptyParts)
            .filter(QRegExp(R"(.*sd.*)"));
    auto deviceInfo = QInputDialog::getItem(this, "Select Device", "Name Label Size Mount Point", devices);
    if (!deviceInfo.isEmpty()) {
        QRegExp deviceRegex(R"((sd[a-z]\d*))");
        deviceRegex.indexIn(deviceInfo);
        this->devicePushButton->setText("/dev/" + deviceRegex.capturedTexts().at(1));
    }
}

void VeracryptConfigItem::openMountPathPicker() {
    QString mountPath = QFileDialog::getExistingDirectory(this, tr("Select Mount Directory"), this->mountPath->text(),
                                                          QFileDialog::ShowDirsOnly);
    if (!mountPath.isEmpty()) this->mountPath->setText(mountPath);
}

void VeracryptConfigItem::openKeyFilePicker() {
    QStringList keyFilePaths = QFileDialog::getOpenFileNames(this, tr("Select Key File"), QDir::homePath(), tr("Key Files (*)"));
    if (!keyFilePaths.isEmpty()) this->keyFileListWidget->addItems(keyFilePaths);
    validateKeyFileControls();
}

void VeracryptConfigItem::removeKeyFile() {
    const int currentIndex = this->keyFileListWidget->currentIndex().row();
    if (currentIndex == -1) return;
    this->keyFileListWidget->takeItem(currentIndex);
    validateKeyFileControls();
}

void VeracryptConfigItem::validateKeyFileControls() {
    const bool empty = this->keyFileListWidget->model()->rowCount() == 0;
    this->keyFileListWidget->setHidden(empty);
    this->removeKeyFileButton->setDisabled(empty || this->keyFileListWidget->currentIndex().row() == -1);
}

void VeracryptConfigItem::deleteConfig() {
    auto res = QMessageBox::question(this, "Delete Entry ?", "Do You Want To Delete This Entry ?", QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes) {
        // Deletes Widget and remove it from list in parent
        emit confirmedDelete();
    }
}

void VeracryptConfigItem::passFilePicker() {
    QString passPath = QFileDialog::getOpenFileName(this, tr("Select Pass File"), QDir::homePath() + "/.password-store",
                                                    tr("Pass File (*.gpg)"));
    if (!passPath.isEmpty()) {
        QRegExp passRegex(R"(^.*/\.password-store/(.*)\.gpg$)");
        passRegex.indexIn(passPath);
        const QString res = passRegex.capturedTexts().at(1);
        if (!res.isEmpty()) this->passIntegration->setText(res);
    }
}
