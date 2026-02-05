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

#include "changestracker.hpp"

#include "../command/commandhistory.hpp"
#include "../context/applicationcontext.hpp"
#include "../context/spatialcontext.hpp"

ChangesTracker::ChangesTracker(ApplicationContext *context)
    : QObject(context),
      m_context(context),
      m_hasUnsavedChanges(false) {
    // Connect to command history signals to track changes
    CommandHistory &commandHistory = m_context->spatialContext().commandHistory();
    
    connect(&commandHistory, &CommandHistory::commandExecuted, this, &ChangesTracker::m_onCommandExecuted);
    connect(&commandHistory, &CommandHistory::commandUndone, this, &ChangesTracker::m_onCommandUndone);
}

ChangesTracker::~ChangesTracker() {
    qDebug() << "Object deleted: ChangesTracker";
}

bool ChangesTracker::hasUnsavedChanges() const {
    return m_hasUnsavedChanges;
}

void ChangesTracker::markSaved() {
    if (m_hasUnsavedChanges) {
        m_hasUnsavedChanges = false;
        emit changesStatusChanged(false);
    }
}

void ChangesTracker::markChanged() {
    if (!m_hasUnsavedChanges) {
        m_hasUnsavedChanges = true;
        emit changesStatusChanged(true);
    }
}

void ChangesTracker::m_onCommandExecuted() {
    markChanged();
}

void ChangesTracker::m_onCommandUndone() {
    // Note: This is a simplified implementation. A full implementation would
    // track the command history depth to determine if we're at a saved point.
    markChanged();
}