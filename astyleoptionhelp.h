// Copyright (C) 2026 The Astyle Plugin Authors
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <QString>

namespace Astyle::Internal {

// Rich text help for one AStyle option, meant for tool tips: the option name,
// the wording of the Artistic Style manual and, when the manual has one, a
// "before / after" code sample side by side.
//
// @p option uses the AStyle spelling, e.g. "--indent-switches" or
// "--align-pointer=type". An empty string is returned for options that are not
// covered, so callers can fall back to a plain text tool tip.
QString optionHelp(const QString &option);

} // namespace Astyle::Internal
