// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "astyleformatter.h"

#include "astyletr.h"

// The bundled Artistic Style library. AStyleMain() is the documented C entry
// point: it tokenizes the option string the same way the astyle command line
// front end does, formats the buffer and returns the result allocated with the
// memory allocation callback handed in below.
#include <astyle_main.h>

#include <cstdlib>

namespace Astyle::Internal {

namespace {

// AStyleMain() reports errors through a plain C callback without user data, so
// the collector has to be thread local: project wide formatting runs on a
// thread pool.
thread_local QStringList *g_errorCollector = nullptr;

void STDCALL collectError(int errorNumber, const char *message)
{
    if (!g_errorCollector)
        return;

    QString text = QString::fromUtf8(message ? message : "").trimmed();
    if (text.isEmpty())
        text = Tr::tr("Artistic Style reported error %1.").arg(errorNumber);
    if (!g_errorCollector->contains(text))
        g_errorCollector->append(text);
}

char *STDCALL allocateOutput(unsigned long size)
{
    return static_cast<char *>(std::malloc(size));
}

QString decodeUtf16(const QByteArray &data, bool bigEndian)
{
    const int count = data.size() / 2;
    QString text;
    text.resize(count);

    const uchar *src = reinterpret_cast<const uchar *>(data.constData());
    QChar *dst = text.data();
    for (int i = 0; i < count; ++i) {
        const uchar first = src[2 * i];
        const uchar second = src[2 * i + 1];
        const ushort value = bigEndian ? ushort((first << 8) | second)
                                       : ushort((second << 8) | first);
        dst[i] = QChar(value);
    }
    return text;
}

QByteArray encodeUtf16(const QString &text, bool bigEndian)
{
    QByteArray data;
    data.resize(text.size() * 2);

    uchar *dst = reinterpret_cast<uchar *>(data.data());
    for (int i = 0; i < text.size(); ++i) {
        const ushort value = text.at(i).unicode();
        dst[2 * i] = bigEndian ? uchar(value >> 8) : uchar(value & 0xff);
        dst[2 * i + 1] = bigEndian ? uchar(value & 0xff) : uchar(value >> 8);
    }
    return data;
}

bool runFormatter(const QByteArray &input, const QString &options,
                  QStringList *errors, QByteArray *output)
{
    const QByteArray optionData = options.toUtf8();

    QStringList collected;
    QStringList *previousCollector = g_errorCollector;
    g_errorCollector = &collected;
    char *formatted = AStyleMain(input.constData(), optionData.constData(),
                                 &collectError, &allocateOutput);
    g_errorCollector = previousCollector;

    *errors = collected;
    if (!formatted)
        return false;

    *output = QByteArray(formatted);
    std::free(formatted);
    return true;
}

} // namespace

FormatResult formatBytes(const QByteArray &input, const QString &options)
{
    FormatResult result;

    // Split off a byte order mark: AStyle works on narrow strings only, and a
    // BOM would otherwise be treated as ordinary text on the first line.
    QByteArray bom;
    QByteArray body = input;
    bool isUtf16 = false;
    bool bigEndian = false;

    if (body.size() >= 2) {
        const uchar first = uchar(body.at(0));
        const uchar second = uchar(body.at(1));
        if (first == 0xff && second == 0xfe) {
            isUtf16 = true;
            bigEndian = false;
            bom = body.left(2);
            body = decodeUtf16(body.mid(2), bigEndian).toUtf8();
        } else if (first == 0xfe && second == 0xff) {
            isUtf16 = true;
            bigEndian = true;
            bom = body.left(2);
            body = decodeUtf16(body.mid(2), bigEndian).toUtf8();
        }
    }

    if (!isUtf16 && body.startsWith("\xef\xbb\xbf")) {
        bom = body.left(3);
        body.remove(0, 3);
    }

    // Reject binary content *after* the UTF-16 decoding, the NUL bytes of a
    // UTF-16 stream are perfectly fine.
    if (body.contains('\0')) {
        result.errors << Tr::tr("The file contains binary data and is not formatted.");
        return result;
    }

    QStringList errors;
    QByteArray formatted;
    if (!runFormatter(body, options, &errors, &formatted)) {
        result.errors = errors;
        return result;
    }

    result.errors = errors;
    result.ok = true;
    if (isUtf16)
        result.text = bom + encodeUtf16(QString::fromUtf8(formatted), bigEndian);
    else
        result.text = bom + formatted;
    return result;
}

QStringList validateOptions(const QString &options)
{
    static const QByteArray sample = "int main()\n{\nreturn 0;\n}\n";
    return formatBytes(sample, options).errors;
}

QString libraryVersion()
{
    return QString::fromLatin1(AStyleGetVersion());
}

} // namespace Astyle::Internal
