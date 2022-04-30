#include "CLIMain.h"
#include <QTextStream>
#include <core/VeracryptVolumeManager.h>
#include <core/VolumeCommandBuilder.h>

CLIMain::CLIMain(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("veracryptrunner");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Command line interface for the krunner-veracryptrunner Plugin");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("query", "Volume you want to mount/unmount");

    QCommandLineOption listOption({"l", "list"}, "List available configs");
    parser.addOption(listOption);

    QCommandLineOption infoOption({"i", "info"}, "Show info for the volume");
    parser.addOption(infoOption);

    parser.process(app);
    VeracryptVolumeManager::fetchMountedVolumes(mountedVolumes);
    volumes = manager.getVeracryptVolumesMap();

    if (parser.isSet(listOption)) {
        showVolumeList();
    } else if (parser.isSet(infoOption)) {
        if (parser.positionalArguments().isEmpty()) {
            QTextStream err(stderr);
            err << "info option requires a query\n";
            err.flush();
        } else {
            showVolumeInfo(parser.positionalArguments().join(' '));
        }
    } else {
        executeMountCommand(parser.positionalArguments().join(' '));
    }
}

void CLIMain::showVolumeList() {
    QTextStream ts(stdout);

    ts << "List of volumes:\n";
    for (const auto &volume:volumes) {
        const auto mountNote = mountedVolumes.contains(volume->source) ? " [Mounted]" : " [Unmounted]";
        ts << volume->source << mountNote << '\n';
    }
}

void CLIMain::showVolumeInfo(const QString &query) {
    const QString volumeName = getVolumeByChoice(query);
    VeracryptVolume *volume = volumes.value(volumeName);
    QTextStream ts(stdout);
    ts << "Name: " << volume->name << '\n';
    ts << "Type: " << volume->type << '\n';
    ts << "Source: " << volume->source << '\n';
    ts << "Mount Path: " << volume->mountPath << '\n';
    ts << "Mounted: " << (mountedVolumes.contains(volume->source) ? "true" : "false") << '\n';
    if (!volume->passPath.isEmpty()) {
        ts << "Pass Path (optional): " << volume->passPath << '\n';
    }
    if (!volume->keyFiles.isEmpty()) {
        ts << "Key Files: " << '\n';
        for (const auto &keyFile:volume->keyFiles) {
            ts << "    " << keyFile << '\n';
        }

    }

}

void CLIMain::executeMountCommand(const QString &query) {
    const QString volumeName = getVolumeByChoice(query);
    QTextStream ts(stdout);
    QTextStream err(stderr);
    QTextStream in(stdin);


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

QString CLIMain::getVolumeByChoice(const QString &query) {
    QString volumeName;
    QTextStream ts(stderr);
    QTextStream err(stderr);
    QTextStream in(stdin);
    QStringList choices;

    for (const auto *volume:volumes) {
        if (volume->name.contains(query, Qt::CaseInsensitive)) {
            choices.append(volume->name);
        }
    }
    if (choices.isEmpty()) {
        err << "The term matches no configured volume, use the --list option to show the available names\n";
    } else if (choices.size() > 1) {
        ts << "The query matches multiple volume names, please select the volume by typing the number\n";
        const int choicesCount = choices.count();
        for (int i = 0; i < choicesCount; ++i) {
            const QString &name = choices.at(i);
            const auto mountNote = mountedVolumes.contains(volumes.value(name)->source) ? " [Mounted]" : " [Unmounted]";
            ts << name << mountNote << ' ' << i << '\n';
        }
        ts.flush();
        bool ok = true;
        const int volumeIndex = in.readLine().toInt(&ok);
        if (ok) {
            if (volumeIndex >= 0 && volumeIndex < choices.size()) {
                volumeName = choices.at(volumeIndex);
            } else {
                err << "Given index is not in list !\n";
            }
        } else {
            err << "Could not parse integer\n";
        }
    } else {
        volumeName = choices.at(0);
    }
    err.flush();
    ts.flush();
    return volumeName;
}
