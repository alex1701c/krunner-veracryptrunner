//  Licensed under the GNU GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <utility>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <KNotifications/KNotification>
#include <core/VeracryptVolume.h>
#include <ui/VeracryptConfigItem.h>

class EditDialog : public QDialog {
Q_OBJECT


public:

    /**
     * @param volume Volume which should be edited
     * @param volumeNames Names of currently loaded volumes
     */
    explicit EditDialog(VeracryptVolume *volume, QStringList volumeNames);

    bool edited = false;
    QString initialName;
    KConfigGroup config;
    VeracryptConfigItem *item;
    VeracryptVolume *volume;
    QStringList volumeNames;

private slots:

    void save();

    void changed() { edited = true; };

};

#endif // ADDDIALOG_H
