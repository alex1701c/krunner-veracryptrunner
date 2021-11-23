//  Licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#pragma once

#include <QObject>
#include <QtCore/QFileSystemWatcher>
#include <core/VolumeCommandBuilder.h>
#include <core/VeracryptVolume.h>
#include <core/VeracryptVolumeManager.h>
#include "dbus_utils.h"

class VeracryptRunner : public QObject {
Q_OBJECT

public:
    explicit VeracryptRunner();

    ~VeracryptRunner() override;

    void init();

public Q_SLOTS:

    RemoteActions Actions();

    RemoteMatches Match(const QString &searchTerm);

    void Teardown() {};
    QVariantMap Config() {return {};}

    void Run(const QString &id, const QString &actionId);

    void configChanged(const QString &fileName);

private:
    QRegExp queryRegex = QRegExp(QStringLiteral(R"(^(?:veracrypt?|vc)(?: (.*))?$)"));
    VeracryptVolumeManager *manager;
    bool initialized = false;
    const QString iconName = QStringLiteral("veracrypt");
    QMap<QString, VeracryptVolume *> volumes;
    QFileSystemWatcher *watcher;

    QStringList mountedVolumes;
    QTime lastFetched;
    bool forceFetch = true;
    RemoteActions actions;

    void loadVolumesFromConfig();
};

