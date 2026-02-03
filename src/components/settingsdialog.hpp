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

#include <QDialog>
#include <QComboBox>

class ApplicationContext;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(ApplicationContext *context, QWidget *parent = nullptr);
    ~SettingsDialog() override;

    void showCentered();

private:
    void m_setupUI();
    void m_loadSettings();
    void m_saveSettings();

    ApplicationContext *m_context{nullptr};
    QComboBox *m_themeComboBox{nullptr};
};