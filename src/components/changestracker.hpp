/*
 * Drawy - A simple brainstorming tool with an infinite canvas
 * Copyright (C) 2025 - Prayag Jain <prayagjain2@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QObject>

class ApplicationContext;

class ChangesTracker : public QObject {
    Q_OBJECT

public:
    explicit ChangesTracker(ApplicationContext *context);
    ~ChangesTracker() override;

    bool hasUnsavedChanges() const;
    void markSaved();
    void markChanged();

signals:
    void changesStatusChanged(bool hasChanges);

private slots:
    void m_onCommandExecuted();
    void m_onCommandUndone();

private:
    ApplicationContext *m_context;
    bool m_hasUnsavedChanges;
};