// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <QByteArray>
#include <QString>
#include <QStringList>

namespace Astyle::Internal {

// Result of formatting one byte buffer.
struct FormatResult
{
    QByteArray text;
    QStringList errors;
    bool ok = false;
};

// Formats @p input with the Artistic Style library.
//
// @p options is the raw AStyle option list in the same syntax as the .astylerc
// files, i.e. switches separated by white space or newlines, '#' starting a
// comment.
//
// The byte stream is passed to AStyle unchanged, so legacy encodings such as
// GBK survive; only the byte order marks (UTF-8, UTF-16 LE/BE) are handled
// explicitly so that AStyle never has to see them.
FormatResult formatBytes(const QByteArray &input, const QString &options);

// Returns the problems AStyle has with @p options, empty when all of them are
// understood. Used by the settings page and before touching real files.
QStringList validateOptions(const QString &options);

// Version of the bundled Artistic Style library, e.g. "3.6.20".
QString libraryVersion();

} // namespace Astyle::Internal
