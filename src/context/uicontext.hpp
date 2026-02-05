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

#include <QWidget>
class ToolBar;
class PropertyBar;
class ActionBar;
class SettingsDialog;
class Event;
class PropertyManager;
class Tool;
class ApplicationContext;
class KeybindManager;
class ActionManager;
class IconManager;
class NotificationLabel;
class ChangesTracker;

class UIContext : public QObject {
    // Action bar button IDs for clarity and maintainability
    static constexpr int BUTTON_ID_ZOOM_OUT{1};
    static constexpr int BUTTON_ID_ZOOM_IN{2};
    static constexpr int BUTTON_ID_LIGHT_MODE{3};
    static constexpr int BUTTON_ID_UNDO{4};
    static constexpr int BUTTON_ID_REDO{5};
    static constexpr int BUTTON_ID_SAVE_AS{6};
    static constexpr int BUTTON_ID_OPEN_FILE{7};
    static constexpr int BUTTON_ID_SETTINGS{8};
    static constexpr int BUTTON_ID_SAVE{9};

public:
    UIContext(ApplicationContext *context);
    ~UIContext() override;

    void setUIContext();

    ToolBar &toolBar() const;
    PropertyBar &propertyBar() const;
    ActionBar &actionBar() const;
    Event &event() const;
    KeybindManager &keybindManager() const;
    ActionManager &actionManager() const;
    PropertyManager &propertyManager() const;
    IconManager &iconManager() const;

    void showNotification(const QString &message, int durationMs = 2000);
    
    ChangesTracker &changesTracker() const;
    
    bool promptSaveBeforeClose();

    void reset();

public slots:
    void toolChanged(Tool &);

private:
    ToolBar *m_toolBar{nullptr};
    PropertyBar *m_propertyBar{};
    ActionBar *m_actionBar{};
    SettingsDialog *m_settingsDialog{nullptr};
    PropertyManager *m_propertyManager{};
    KeybindManager *m_keybindManager{};
    ActionManager *m_actionManager{};
    Event *m_event{nullptr};
    IconManager *m_iconManager{};
    NotificationLabel *m_notificationLabel{nullptr};
    ChangesTracker *m_changesTracker{nullptr};

    Tool *m_lastTool{nullptr};  // Used to call the cleanup function of the last tool

    ApplicationContext *m_applicationContext;
};
