#pragma once

#include <QtCore/QCommandLineParser>

void cli(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("veracryptrunner");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Command line interface for the krunner-veracryptrunner Plugin");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("volume", "Volume you want to mount/unmount");

    QCommandLineOption listOption({"l", "list"}, "List available configs");
    parser.addOption(listOption);

    parser.process(app);

    const auto config = KSharedConfig::openConfig(QStringLiteral("veracryptrunnerrc"))->group("Configs");
    QStringList mountedVolumes;
    VeracryptVolumeManager::fetchMountedVolumes(mountedVolumes);
    VeracryptVolumeManager manager;
    auto volumes = manager.getVeracryptVolumesMap();
    if (parser.isSet(listOption)) {
        QTextStream ts(stdout);
        ts << "List of volumes:\n";
        for (const auto &volume:volumes) {
            const auto mountNote = mountedVolumes.contains(volume->source) ? " [Mounted]" : " [Unmounted]";
            ts << volume->source << mountNote << "\n";
        }
    } else {
        const QString query = parser.positionalArguments().join(' ');
        QString volumeName;
        QStringList choices;
        QTextStream err(stderr);
        QTextStream ts(stderr);
        QTextStream in(stdin);
        for (const auto *volume:volumes) {
            if (volume->name.contains(query, Qt::CaseInsensitive)) {
                choices.append(volume->name);
            }
        }
        if (choices.isEmpty()) {
            err << "The term matches no configured volume, use the --list option to show the available names";
        } else if (choices.size() > 1) {
            ts << "The query matches multiple volume names, please select the volume by typing the number\n";
            const int choicesCount = choices.count();
            for (int i = 0; i < choicesCount; ++i) {
                const QString &name = choices.at(i);
                const auto mountNote = mountedVolumes.contains(volumes.value(name)->source) ? " [Mounted]" : " [Unmounted]";
                ts << name << mountNote << ' ' << i << "\n";
            }
            ts.flush();
            bool ok = true;
            const int volumeIndex = in.readLine().toInt(&ok);
            if (ok) {
                if (volumeIndex >= 0 && volumeIndex < choices.size()) {
                    volumeName = choices.at(volumeIndex);
                } else {
                    err << "Given index is not in list !";
                }
            } else {
                err << "Could not parse integer";
            }
        } else {
            volumeName = choices.at(0);
        }

        // Confirm/ mount the volume
        if (!volumeName.isEmpty()) {
            VeracryptVolume *volume = volumes.value(volumeName);
            const bool mounted = mountedVolumes.contains(volume->source);
            ts << "Do you want to " << (mounted ? "unmount " : "mount ") << volume->source << "  [y/n]";
            ts.flush();
            const QString confirm = in.readLine().remove('\n');
            if (confirm.isEmpty() || confirm == "y") {
                VeracryptVolumeManager volumeManager;
                if (mounted) {
                    VolumeCommandBuilder::buildUnmountCommand(volume, true);
                } else {
                    VolumeCommandBuilder::buildMountCommand(volume, true);
                }
            } else {
                err << "Aborting\n";
            }

        }
    }
}
