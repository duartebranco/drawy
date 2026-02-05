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

#include "window.hpp"

#include <QButtonGroup>
#include <QFile>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QShortcut>
#include <QStandardPaths>
#include <QTextStream>

#include "../canvas/canvas.hpp"
#include "../components/actionbar.hpp"
#include "../components/changestracker.hpp"
#include "../components/propertybar.hpp"
#include "../components/settingsdialog.hpp"
#include "../components/toolbar.hpp"
#include "../context/applicationcontext.hpp"
#include "../context/renderingcontext.hpp"
#include "../context/uicontext.hpp"
#include "../controller/controller.hpp"
#include "../serializer/loader.hpp"
#include "boardlayout.hpp"

#include <QCloseEvent>

// Constants for layout configuration
static constexpr int LAYOUT_MARGIN{10};

MainWindow::MainWindow(QWidget *parent) : QWidget{parent} {
    this->m_applyCustomStyles();

    BoardLayout *layout{new BoardLayout(this)};
    Controller *controller{new Controller(this)};
    ApplicationContext *context{ApplicationContext::instance()};

    RenderingContext &renderingContext{context->renderingContext()};
    UIContext &uiContext{context->uiContext()};

    renderingContext.canvas().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->setLayout(layout);

    layout->setMargins(LAYOUT_MARGIN);
    layout->setLeftWidget(&uiContext.propertyBar());
    layout->setTopWidget(&uiContext.toolBar());
    layout->setBottomWidget(&uiContext.actionBar());
    layout->setCentralWidget(&renderingContext.canvas());

    // Attempt to auto-load the last opened file if the user has enabled that setting
    m_tryLoadLastOpenedFile(context);

    // Connect canvas events to the controller for proper input handling
    QObject::connect(&renderingContext.canvas(),
                     &Canvas::mousePressed,
                     controller,
                     &Controller::mousePressed);
    QObject::connect(&renderingContext.canvas(),
                     &Canvas::mouseMoved,
                     controller,
                     &Controller::mouseMoved);
    QObject::connect(&renderingContext.canvas(),
                     &Canvas::mouseReleased,
                     controller,
                     &Controller::mouseReleased);
    QObject::connect(&renderingContext.canvas(),
                     &Canvas::keyPressed,
                     controller,
                     &Controller::keyPressed);
    QObject::connect(&renderingContext.canvas(),
                     &Canvas::keyReleased,
                     controller,
                     &Controller::keyReleased);

    QObject::connect(&renderingContext.canvas(), &Canvas::tablet, controller, &Controller::tablet);
    QObject::connect(&renderingContext.canvas(), &Canvas::wheel, controller, &Controller::wheel);
    QObject::connect(&renderingContext.canvas(), &Canvas::leave, controller, &Controller::leave);

    m_applyCustomStyles();
}

MainWindow::~MainWindow() {
}

void MainWindow::m_tryLoadLastOpenedFile(ApplicationContext *context) {
    // Build the path to the settings file in the ~/.config/drawy directory
    QString settingsPath{QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
                         "/drawy/settings.json"};
    QFile settingsFile{settingsPath};

    // Read the settings file to check if auto-restore is enabled
    if (settingsFile.exists() && settingsFile.open(QIODevice::ReadOnly)) {
        QByteArray fileContent{settingsFile.readAll()};
        settingsFile.close();

        QJsonDocument settingsDocument{QJsonDocument::fromJson(fileContent)};
        if (settingsDocument.isObject()) {
            QJsonObject settingsObject{settingsDocument.object()};

            // Check if the user has enabled "restore last file" setting
            bool restoreLastFileEnabled{settingsObject.value("restoreLastFile").toBool(false)};
            QString lastOpenedFilePath{settingsObject.value("lastOpenedFile").toString("")};

            // If enabled and a file path is stored, attempt to load it
            if (restoreLastFileEnabled && !lastOpenedFilePath.isEmpty()) {
                Loader fileLoader{};
                fileLoader.loadFromFilePath(context, lastOpenedFilePath);
                context->uiContext().changesTracker().markSaved();
            }
        }
    }
}

void MainWindow::m_applyCustomStyles() {
    // Load and apply the custom stylesheet
    QFile styleFile{":/styles/style.qss"};
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream styleStream{&styleFile};
        QString styleSheet{styleStream.readAll()};
        this->setStyleSheet(styleSheet);
    } else {
        qWarning() << "Failed to load stylesheet.";
    }

    // Load custom fonts used in the application
    int fuzzyBubblesId{QFontDatabase::addApplicationFont(":/fonts/FuzzyBubbles.ttf")};
    if (fuzzyBubblesId == -1) {
        qWarning() << "Failed to load font: FuzzyBubbles";
    }

    int interFontId{QFontDatabase::addApplicationFont(":/fonts/Inter.ttf")};
    if (interFontId == -1) {
        qWarning() << "Failed to load font: Inter";
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    ApplicationContext *context{ApplicationContext::instance()};
    UIContext &uiContext{context->uiContext()};

    if (uiContext.promptSaveBeforeClose()) {
        event->accept();
    } else {
        event->ignore();
    }
}