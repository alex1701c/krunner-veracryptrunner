//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#include "editdialog.h"

#include <QClipboard>
#include <QDialogButtonBox>
#include <QDebug>
#include <QPushButton>
#include <QStringBuilder>
#include <config/VeracryptConfigItem.h>

EditDialog::EditDialog(VeracryptVolume *volume, QStringList volumeNames) : QDialog(nullptr) {
    setWindowTitle(QStringLiteral("Veracrypt Runner Edit Config"));
    auto *layout = new QGridLayout(this);
    item = new VeracryptConfigItem(this, volume);
    item->moveUp->setHidden(true);
    item->moveDown->setHidden(true);
    item->deleteConfigButton->setHidden(true);
    auto *box = new QDialogButtonBox(this);
    box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(item, 0, 0);
    layout->addWidget(box, 1, 0);
    connect(item, &VeracryptConfigItem::changed, this, &EditDialog::changed);
    connect(box->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &EditDialog::save);
    connect(box, &QDialogButtonBox::clicked, this, &EditDialog::close);

    initialName = volume->name;
    this->volume = volume;
    config = KSharedConfig::openConfig("veracryptrunnerrc")->group("Configs");
    volumeNames.removeOne(initialName);
    this->volumeNames = volumeNames;
}

void EditDialog::save() {
    if (!edited) {
        this->close();
        return;
    }
    QString name = item->nameLineEdit->text();
    if (name != initialName) {
        config.deleteGroup(initialName);
    }
    if (volumeNames.contains(name)) {
        // Add number to name if it is already taken and the warning is ignored
        for (int i = 1; i < 11; ++i) {
            QString _name = name + QString::number(i);
            if (!volumeNames.contains(_name)) {
                name = _name;
                i = 11;
            }
        }
    }
    auto group = config.group(name);
    group.writeEntry("priority", volume->priority);
    group.writeEntry("type", item->fileRadioButton->isChecked() ? "FILE" : "DEVICE");
    group.writeEntry("source", item->fileRadioButton->isChecked() ? item->filePushButton->text().remove('&') :
                               item->devicePushButton->text().remove('&'));
    group.writeEntry("mountPath", item->mountPath->text().remove('&'));
    QString keyFilesString;
    int count = item->keyFileListWidget->model()->rowCount();
    for (int i = 0; i < count; ++i) keyFilesString.append(item->keyFileListWidget->item(i)->text() + ";");
    group.writeEntry("keyFiles", keyFilesString);
    group.writeEntry("passPath", item->passIntegration->text());
    config.sync();
    this->close();
}
