//  Licensed under the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1. See License in the project root for license information.
#pragma once

#include <QObject>
#include "dbus_utils.h"

class VeracryptRunner : public QObject {
Q_OBJECT

public:
    explicit VeracryptRunner();

public Q_SLOTS:

    RemoteActions Actions();

    RemoteMatches Match(const QString &searchTerm);

    void Run(const QString &id, const QString &actionId);
};

