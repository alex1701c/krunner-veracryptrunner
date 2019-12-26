#pragma once

#include <QtCore/QProcess>
#include "VeracryptVolume.h"
#include <QtCore/QThread>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <KNotifications/KNotification>
#include <QStringBuilder>
#include <QRegularExpression>

class VolumeCommandBuilder {
public:
    static QString hostname;

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
        options.append(QStringLiteral("--keyfiles"));
        options.append(volume->keyFiles.join(','));

        // Validate source
        if (volume->source == QLatin1String("Select File") || volume->source == QLatin1String("Select Device")) {
            showErrorMessage(QStringLiteral("Please select a valid source for the volume!"));
            return;
        } else if (!QFile::exists(volume->source)) {
            showErrorMessage(QStringLiteral("The volume source ( ") % volume->source % QStringLiteral(" ) does not exist!"));
            return;
        }
        options.append(volume->source);
        options.append(volume->mountPath);

        QProcess::startDetached(QStringLiteral("veracrypt"), options);

        // Optional pass integration
        if (!volume->passPath.isEmpty()) {
            QProcess passProcess;
            passProcess.start("pass", QStringList() << "show" << volume->passPath);
            // Wait for windows to be shown
            QThread::msleep(750);
            const QMap<QString, QString> ids = getCurrentWindows(volume->source);
            if (!ids.contains(QStringLiteral("veracrypt"))) {
                passProcess.kill();
                showErrorMessage(QStringLiteral("Could not find Veracrypt Window, proceeding without autotype"));
            }
            if (ids.contains(QStringLiteral("pass"))) {
                // Focus window if it exists
                QProcess::startDetached("wmctrl", QStringList({"-i", "-a", ids.value("pass")}));
            }

            passProcess.waitForFinished(-1);
            const QString passResults = passProcess.readAllStandardOutput();
            const QString passError = passProcess.readAllStandardError();
            if (passError.contains(QLatin1String(" is not in the password store."))) {
                showErrorMessage(passError);
            }
            if (!passResults.isEmpty()) {
                const QString password = passResults.split('\n', QString::SkipEmptyParts).at(0);
                if (!password.isEmpty()) {
                    QProcess::startDetached("xdotool",
                                            QStringList() << "type" << "--window" << ids.value("veracrypt") << password);
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
        const QStringList entries = out.split('\n', QString::SkipEmptyParts);

        // Window Id, desktop number, hostname, window name
        const auto regexStr = QString(R"(^([\w]+)  \d+ (.*))");
        QRegExp entryRegex(regexStr);
        QMap<QString, QString> windowsIDs;
        const auto veracryptWindowContains = QString(QStringLiteral("Enter password for \"") % volumeSource % "\"");

        // Write only the window isd for pass and veracrypt
        for (const auto &entry:entries) {
            entryRegex.indexIn(entry);
            const QString id = entryRegex.cap(1);
            const QString title = entryRegex.cap(2);
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

// Initialize static variable
QString VolumeCommandBuilder::hostname = VolumeCommandBuilder::getHostName();
