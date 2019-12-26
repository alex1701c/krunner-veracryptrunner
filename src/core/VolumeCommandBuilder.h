#pragma once

#include <QtCore/QProcess>
#include "VeracryptVolume.h"
#include <QtCore/QThread>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <KNotifications/KNotification>
#include <QStringBuilder>

class VolumeCommandBuilder {
public:
    static void buildUnmountCommand(const VeracryptVolume *volume) {
        QProcess::startDetached("veracrypt", QStringList() << "-d" << volume->source);
    }

    static void buildMountCommand(const VeracryptVolume *volume) {
        QStringList options;

        // Validate Key Files
        for (const auto &keyFile:volume->keyFiles) {
            if (!QFile::exists(keyFile)) {
                showErrorMessage(QStringLiteral("The key file ( ") % keyFile % QStringLiteral(" ) does not exist!"));
                return;
            }
        }
        options.append("--keyfiles");
        options.append(volume->keyFiles.join(","));

        // Validate source
        if (volume->source == "Select File" || volume->source == "Select Device") {
            showErrorMessage(QStringLiteral("Please select a valid source for the volume!"));
            return;
        } else if (!QFile::exists(volume->source)) {
            showErrorMessage(QStringLiteral("The volume source ( ") % volume->source % QStringLiteral(" ) does not exist!"));
            return;
        }
        options.append(volume->source);

        options.append(volume->mountPath);

        QProcess::startDetached("veracrypt", options);
        // TODO Fetch all window isd when the veracrypt and optionally pass windows are open
        // TODO Make focus on pass wondow only if it exists

        // Optional pass integration
        if (!volume->passPath.isEmpty()) {
            // Gives automatically focus so you can type
            QProcess::startDetached("sh", QStringList({"-c", "sleep 0.5; wmctrl -a" % getHostName()}));
            QProcess passProcess;
            passProcess.start("pass", QStringList() << "show" << volume->passPath);
            passProcess.waitForFinished(-1);
            const QString passResults = passProcess.readAllStandardOutput();
            const QString passError = passProcess.readAllStandardError();
            if (passError.contains(QLatin1String(" is not in the password store."))) {
                showErrorMessage(passError);
            }
            if (!passResults.isEmpty()) {
                QString password = passResults.split('\n', QString::SkipEmptyParts).at(0);
                if (!password.isEmpty()) {
                    QProcess windowIdProcess;
                    windowIdProcess.start("xwininfo",
                                          QStringList({"-name", "Enter password for \"" % volume->source % "\""}));
                    windowIdProcess.waitForFinished(-1);
                    QString windowIdRes = windowIdProcess.readAll();
                    if (windowIdRes.contains(QLatin1String("Window id: "))) {
                        const auto id = windowIdRes.split(QStringLiteral("Window id: ")).at(1).split(' ').at(0);
                        QProcess::startDetached("xdotool", QStringList() << "type" << "--window" << id << password);
                    }
                }
            }
        }
    }

    static void showErrorMessage(const QString &msg) {
        KNotification::event(KNotification::Error,
                             QStringLiteral("Veracrypt Runner"),
                             msg,
                             QStringLiteral("veracrypt")
        );

    }

    static QString getHostName() {
        QProcess hostNameProces;
        hostNameProces.start("hostname");
        hostNameProces.waitForFinished(-1);
        return QString(hostNameProces.readAllStandardOutput());
    }
};

