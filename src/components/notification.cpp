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

#include "notification.hpp"

NotificationLabel::NotificationLabel(QWidget *parent)
    : QLabel(parent),
      m_hideTimer(new QTimer(this)) {
    
    setStyleSheet(
        "QLabel {"
        "  background-color: rgba(40, 40, 40, 200);"
        "  color: white;"
        "  padding: 8px 16px;"
        "  border-radius: 4px;"
        "  font-size: 12px;"
        "}"
    );
    
    setAlignment(Qt::AlignCenter);
    hide();
    
    connect(m_hideTimer, &QTimer::timeout, this, &NotificationLabel::m_hide);
}

NotificationLabel::~NotificationLabel() {
    m_hideTimer->stop();
}

void NotificationLabel::showMessage(const QString &message, int durationMs) {
    setText(message);
    adjustSize();
    
    // Position at top-right corner of parent widget
    if (parentWidget()) {
        int x = parentWidget()->width() - width() - 20;
        int y = 20;
        move(x, y);
    }
    
    show();
    raise();
    
    m_hideTimer->stop();
    m_hideTimer->start(durationMs);
}

void NotificationLabel::m_hide() {
    m_hideTimer->stop();
    hide();
}