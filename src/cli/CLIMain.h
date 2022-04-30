#pragma once

#include "../core/VeracryptVolumeManager.h"
#include <QtCore/QCommandLineParser>

class CLIMain {
public:
    explicit CLIMain(int argc, char **argv);

private:
    VeracryptVolumeManager manager;
    QStringList mountedVolumes;
    QMap<QString, VeracryptVolume *> volumes;

    /**
     * Shows an overview of the configured volumes
     */
    void showVolumeList();

    /**
     *Show information of the volume to the user
     * @param query
     */
    void showVolumeInfo(const QString &query);

    /**
     * Mounts/Unmounts the volume the user has selected
     * @param query
     */
    void executeMountCommand(const QString &query);

    /**
     * Gets the volume name based on the query, aks user for input if query matches more than one volume
     * @param query
     * @return name of volume
     */
    QString getVolumeByChoice(const QString &query);
};

