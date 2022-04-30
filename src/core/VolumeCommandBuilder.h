#pragma once

#ifndef VOLUME_COMMAND_BUILDER_H
#define VOLUME_COMMAND_BUILDER_H

#include <QtCore/QProcess>
#include "VeracryptVolume.h"
#include <QtCore/QThread>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <KNotifications/KNotification>
#include <QStringBuilder>
#include <QRegularExpression>
#include <QDebug>

class VolumeCommandBuilder {
public:
    static void buildUnmountCommand(const VeracryptVolume *volume, const bool text = false) {
        QStringList options;
        if (text) {
            options.append(QStringLiteral("--text"));
        }
        options.append(QStringLiteral("-d"));
        options.append(volume->source);

        if (text) {
            QProcess process;
            options.prepend(QStringLiteral("veracrypt"));
            Q_UNUSED(system(qPrintable(options.join(' '))));
        } else {
            QProcess::startDetached(QStringLiteral("veracrypt"), options);
        }
    }

    static void buildMountCommand(const VeracryptVolume *volume, const bool text = false) {
        QStringList options;

        if (text) {
            options.append(QStringLiteral("--text"));
        }

        // Validate Key Files
        for (const auto &keyFile:volume->keyFiles) {
            if (!QFile::exists(keyFile)) {
                showErrorMessage(QStringLiteral("The key file ( ") % keyFile % QStringLiteral(" ) does not exist!"), text);
                return;
            }
        }
        options.append(QStringLiteral("--keyfiles"));
        options.append(volume->keyFiles.join(','));

        // Validate source
        if (volume->source == QLatin1String("Select File") || volume->source == QLatin1String("Select Device")) {
            showErrorMessage(QStringLiteral("Please select a valid source for the volume!"), text);
            return;
        } else if (!QFile::exists(volume->source)) {
            showErrorMessage(QStringLiteral("The volume source ( ") % volume->source % QStringLiteral(" ) does not exist!"), text);
            return;
        }
        options.append(volume->source);
        options.append(volume->mountPath);

        if (text) {
            QProcess process;
            options.prepend(QStringLiteral("veracrypt"));
            Q_UNUSED(system(qPrintable(options.join(' '))));
        } else {
            QProcess::startDetached(QStringLiteral("veracrypt"), options);
        }


        // Optional pass integration
        if (!volume->passPath.isEmpty() && !text) {
            QProcess passProcess;
            passProcess.start("pass", QStringList() << "show" << volume->passPath);
            // Wait for windows to be shown
            QThread::msleep(750);
            const QMap<QString, QString> ids = getCurrentWindows(volume->source);
            if (!ids.contains(QStringLiteral("veracrypt"))) {
                passProcess.kill();
                showErrorMessage(QStringLiteral("Could not find Veracrypt Window, proceeding without autotype"), text);
            }
            if (ids.contains(QStringLiteral("pass"))) {
                // Focus window if it exists
                QProcess::startDetached("wmctrl", QStringList({"-i", "-a", ids.value("pass")}));
            }

            passProcess.waitForFinished(-1);
            const QString passResults = passProcess.readAllStandardOutput();
            const QString passError = passProcess.readAllStandardError();
            if (passError.contains(QLatin1String(" is not in the password store."))) {
                showErrorMessage(passError, text);
            }
            if (!passResults.isEmpty()) {
                const QString password = passResults.split('\n', Qt::SkipEmptyParts).at(0);
                if (!password.isEmpty()) {
                    QProcess::startDetached("xdotool",
                                            QStringList() << "type" << "--window" << ids.value("veracrypt") << password);
                }
            }
        }
    }

    static void showErrorMessage(const QString &msg, const bool text) {
        if (!text) {
            KNotification::event(KNotification::Error,
                                 QStringLiteral("Veracrypt Runner"),
                                 msg,
                                 QStringLiteral("veracrypt")
            );
        } else {
            QTextStream err(stderr);
            err << msg << '\n';
            err.flush();
        }
    }


    static QString getHostName() {
        QProcess hostNameProces;
        hostNameProces.start("hostname", QStringList());
        hostNameProces.waitForFinished(-1);
        return QString(hostNameProces.readAllStandardOutput()).remove('\n');
    }

    /**
     * Results are used to determine if there is a pass window that needs to be focused
     * Additionally it contains the id of the veracrypt window for the autotype feature
     *
     * @param volumeSource
     * @return
     */
    static QMap<QString, QString> getCurrentWindows(const QString &volumeSource) {
        QProcess windowListProcess;
        windowListProcess.start("wmctrl", QStringList({"-l"}));
        windowListProcess.waitForFinished(-1);
        const QString out = windowListProcess.readAllStandardOutput();
        const QStringList entries = out.split('\n', Qt::SkipEmptyParts);

        // Window Id, desktop number, hostname, window name
        static const QRegularExpression entryRegex((R"(^([\w]+)  \d+ (.*))"));
        QMap<QString, QString> windowsIDs;
        const auto veracryptWindowContains = QString(QStringLiteral("Enter password for \"") % volumeSource % "\"");
        const QString hostname = getHostName();

        // Write only the window isd for pass and veracrypt
        for (const auto &entry:entries) {
            QRegularExpressionMatch match = entryRegex.match(entry);
            if (!match.hasMatch()) {
                continue;
            }
            const QString id = match.capturedTexts().at(1);
            const QString title = match.capturedTexts().at(2);
            if (!id.isEmpty()) {
                if (title.contains(veracryptWindowContains)) {
                    windowsIDs.insert("veracrypt", id);
                } else if (title.endsWith(hostname)) {
                    windowsIDs.insert("pass", id);
                }
            }
        }
        return windowsIDs;
    }
};

#endif // VOLUME_COMMAND_BUILDER_H
