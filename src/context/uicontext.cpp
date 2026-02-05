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

#include "uicontext.hpp"

#include "../canvas/canvas.hpp"
#include "../components/notification.hpp"
#include "../components/changestracker.hpp"
#include "../command/commandhistory.hpp"
#include "../common/renderitems.hpp"
#include "../components/actionbar.hpp"
#include "../components/propertybar.hpp"
#include "../components/settingsdialog.hpp"
#include "../components/toolbar.hpp"
#include "../event/event.hpp"
#include "../keybindings/actionmanager.hpp"
#include "../keybindings/keybindmanager.hpp"
#include "../properties/widgets/propertymanager.hpp"
#include "../serializer/loader.hpp"
#include "../serializer/serializer.hpp"
#include "../tools/arrowtool.hpp"
#include "../tools/ellipsetool.hpp"
#include "../tools/erasertool.hpp"
#include "../tools/freeformtool.hpp"
#include "../tools/linetool.hpp"
#include "../tools/movetool.hpp"
#include "../tools/rectangletool.hpp"
#include "../tools/selectiontool/selectiontool.hpp"
#include "../tools/texttool.hpp"
#include "applicationcontext.hpp"
#include "renderingcontext.hpp"
#include "selectioncontext.hpp"
#include "spatialcontext.hpp"

#include <QMessageBox>
#include <QPushButton>

UIContext::UIContext(ApplicationContext *context)
    : QObject{context},
      m_applicationContext{context},
      m_notificationLabel{nullptr},
      m_changesTracker{nullptr} {
}

UIContext::~UIContext() {
    delete m_event;
    delete m_changesTracker;
    qDebug() << "Object deleted: UIContext";
}

void UIContext::setUIContext() {
    m_toolBar = new ToolBar(m_applicationContext->parentWidget());
    m_actionBar = new ActionBar(m_applicationContext->parentWidget());
    m_propertyBar = new PropertyBar(m_applicationContext->parentWidget());
    m_keybindManager = new KeybindManager(&m_applicationContext->renderingContext().canvas());
    m_actionManager = new ActionManager(m_applicationContext);
    m_iconManager = new IconManager(m_applicationContext);
    m_settingsDialog =
        new SettingsDialog(m_applicationContext, m_applicationContext->parentWidget());
    m_changesTracker = new ChangesTracker(m_applicationContext);
    m_notificationLabel = new NotificationLabel(&m_applicationContext->renderingContext().canvas());

    m_propertyManager = new PropertyManager(m_propertyBar);
    m_propertyBar->setPropertyManager(m_propertyManager);

    QObject::connect(m_propertyManager,
                     &PropertyManager::propertyUpdated,
                     &m_applicationContext->selectionContext(),
                     &SelectionContext::updatePropertyOfSelectedItems);

    m_event = new Event();

    m_toolBar->addTool(std::make_shared<SelectionTool>(), Tool::Selection);
    m_toolBar->addTool(std::make_shared<FreeformTool>(), Tool::Freeform);
    m_toolBar->addTool(std::make_shared<RectangleTool>(), Tool::Rectangle);
    m_toolBar->addTool(std::make_shared<EllipseTool>(), Tool::Ellipse);
    m_toolBar->addTool(std::make_shared<ArrowTool>(), Tool::Arrow);
    m_toolBar->addTool(std::make_shared<LineTool>(), Tool::Line);
    m_toolBar->addTool(std::make_shared<EraserTool>(), Tool::Eraser);
    m_toolBar->addTool(std::make_shared<TextTool>(), Tool::Text);
    m_toolBar->addTool(std::make_shared<MoveTool>(), Tool::Move);

    // Initialize action bar buttons with their corresponding icons and event handlers
    m_actionBar->addButton("Save As", IconManager::ACTION_SAVE_AS, BUTTON_ID_SAVE_AS);
    m_actionBar->addButton("Save", IconManager::ACTION_SAVE, BUTTON_ID_SAVE);
    m_actionBar->addButton("Open File", IconManager::ACTION_OPEN_FILE, BUTTON_ID_OPEN_FILE);
    m_actionBar->addButton("Zoom Out", IconManager::ACTION_ZOOM_OUT, BUTTON_ID_ZOOM_OUT);
    m_actionBar->addButton("Zoom In", IconManager::ACTION_ZOOM_IN, BUTTON_ID_ZOOM_IN);
    m_actionBar->addButton("Light Mode", IconManager::ACTION_LIGHT_MODE, BUTTON_ID_LIGHT_MODE);
    m_actionBar->addButton("Undo", IconManager::ACTION_UNDO, BUTTON_ID_UNDO);
    m_actionBar->addButton("Redo", IconManager::ACTION_REDO, BUTTON_ID_REDO);
    m_actionBar->addButton("Settings", IconManager::ACTION_SETTINGS, BUTTON_ID_SETTINGS);

    QObject::connect(m_toolBar, &ToolBar::toolChanged, this, &UIContext::toolChanged);
    QObject::connect(m_toolBar,
                     &ToolBar::toolChanged,
                     m_propertyBar,
                     &PropertyBar::updateProperties);

    // Zoom Out: Decrease zoom level
    QObject::connect(&m_actionBar->button(BUTTON_ID_ZOOM_OUT),
                     &QPushButton::clicked,
                     this,
                     [this]() { m_applicationContext->renderingContext().updateZoomFactor(-1); });

    // Zoom In: Increase zoom level
    QObject::connect(&m_actionBar->button(BUTTON_ID_ZOOM_IN),
                     &QPushButton::clicked,
                     this,
                     [this]() { m_applicationContext->renderingContext().updateZoomFactor(1); });

    // Undo: Revert the last action
    QObject::connect(&m_actionBar->button(BUTTON_ID_UNDO), &QPushButton::clicked, this, [this]() {
        m_applicationContext->spatialContext().commandHistory().undo();
        m_applicationContext->renderingContext().markForRender();
        m_applicationContext->renderingContext().markForUpdate();
    });

    // Redo: Reapply the last undone action
    QObject::connect(&m_actionBar->button(BUTTON_ID_REDO), &QPushButton::clicked, this, [this]() {
        m_applicationContext->spatialContext().commandHistory().redo();
        m_applicationContext->renderingContext().markForRender();
        m_applicationContext->renderingContext().markForUpdate();
    });

    // Save As: Show file dialog to save drawing to a specified location
    QObject::connect(&m_actionBar->button(BUTTON_ID_SAVE_AS),
                     &QPushButton::clicked,
                     this,
                     [this]() {
                         Serializer serializer{};
                         serializer.serialize(m_applicationContext);
                         if (serializer.saveToFile()) {
                             m_notificationLabel->showMessage("File saved successfully!");
                             m_changesTracker->markSaved();
                         }
                     });

    // Save: Save drawing directly to the currently open file
    QObject::connect(&m_actionBar->button(BUTTON_ID_SAVE), &QPushButton::clicked, this, [this]() {
        Serializer serializer{};
        serializer.serialize(m_applicationContext);
        if (serializer.saveCurrentFile()) {
            m_notificationLabel->showMessage("File saved successfully!");
            m_changesTracker->markSaved();
        } else {
            // If no current file exists, fall back to Save As dialog
            if (serializer.saveToFile()) {
                m_notificationLabel->showMessage("File saved successfully!");
                m_changesTracker->markSaved();
            }
        }
    });

    // Open File: Load a drawing from disk
    QObject::connect(&m_actionBar->button(BUTTON_ID_OPEN_FILE),
                     &QPushButton::clicked,
                     this,
                     [this]() {
                         Loader loader{};
                         loader.loadFromFile(m_applicationContext);
                         m_changesTracker->markSaved();
                     });

    // Light/Dark Mode Toggle: Switch between light and dark theme
    QObject::connect(&m_actionBar->button(BUTTON_ID_LIGHT_MODE),
                     &QPushButton::clicked,
                     this,
                     [this]() {
                         Canvas &canvas{m_applicationContext->renderingContext().canvas()};
                         QPushButton &button{actionBar().button(BUTTON_ID_LIGHT_MODE)};

                         if (canvas.bg() == Common::lightBackgroundColor) {
                             canvas.setBg(Common::darkBackgroundColor);
                             button.setToolTip("Light Mode");
                             button.setIcon(iconManager().icon(IconManager::ACTION_LIGHT_MODE));
                         } else {
                             canvas.setBg(Common::lightBackgroundColor);
                             button.setToolTip("Dark Mode");
                             button.setIcon(iconManager().icon(IconManager::ACTION_DARK_MODE));
                         }

                         m_applicationContext->renderingContext().markForRender();
                         m_applicationContext->renderingContext().markForUpdate();
                     });

    // Settings: Open the settings dialog
    QObject::connect(&m_actionBar->button(BUTTON_ID_SETTINGS),
                     &QPushButton::clicked,
                     this,
                     [this]() { m_settingsDialog->showCentered(); });

    m_propertyBar->updateProperties(m_toolBar->curTool());
}

ToolBar &UIContext::toolBar() const {
    return *m_toolBar;
}

PropertyBar &UIContext::propertyBar() const {
    return *m_propertyBar;
}

ActionBar &UIContext::actionBar() const {
    return *m_actionBar;
}

KeybindManager &UIContext::keybindManager() const {
    return *m_keybindManager;
}

ActionManager &UIContext::actionManager() const {
    return *m_actionManager;
}

PropertyManager &UIContext::propertyManager() const {
    return *m_propertyManager;
}

Event &UIContext::event() const {
    return *m_event;
}

IconManager &UIContext::iconManager() const {
    return *m_iconManager;
}

void UIContext::toolChanged(Tool &tool) {
    if (tool.type() != Tool::Selection) {
        m_applicationContext->selectionContext().selectedItems().clear();
    }

    Common::renderCanvas(m_applicationContext);

    Canvas &canvas{m_applicationContext->renderingContext().canvas()};

    if (m_lastTool != nullptr)
        m_lastTool->cleanup();

    m_lastTool = &tool;
    canvas.setCursor(tool.cursor());

    m_applicationContext->renderingContext().markForUpdate();
}

void UIContext::showNotification(const QString &message, int durationMs) {
    m_notificationLabel->showMessage(message, durationMs);
}

ChangesTracker &UIContext::changesTracker() const {
    return *m_changesTracker;
}

bool UIContext::promptSaveBeforeClose() {
    if (!m_changesTracker->hasUnsavedChanges()) {
        return true;  // No unsaved changes, safe to close
    }

    QMessageBox msgBox(m_applicationContext->parentWidget());
    msgBox.setWindowTitle("Unsaved Changes");
    msgBox.setText("You have unsaved changes.");
    msgBox.setInformativeText("Do you want to save before closing?");
    msgBox.setIcon(QMessageBox::Warning);
    
    QPushButton *saveButton = msgBox.addButton("Save", QMessageBox::AcceptRole);
    QPushButton *dontSaveButton = msgBox.addButton("Don't Save", QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    
    msgBox.setDefaultButton(saveButton);
    msgBox.exec();
    
    QAbstractButton *clickedButton = msgBox.clickedButton();
    
    if (clickedButton == saveButton) {
        Serializer serializer{};
        serializer.serialize(m_applicationContext);
        if (!serializer.saveCurrentFile()) {
            // If no current file, show save dialog
            if (!serializer.saveToFile()) {
                return false;  // User cancelled save dialog
            }
        }
        m_changesTracker->markSaved();
        return true;
    } else if (clickedButton == dontSaveButton) {
        return true;  // User chose to discard changes
    } else {
        return false;  // User clicked Cancel - remain in app
    }
}

void UIContext::reset() {
    m_lastTool = nullptr;
    toolBar().changeTool(Tool::Selection);
}
