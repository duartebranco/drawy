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

#include "settingsdialog.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QScreen>
#include <QStandardPaths>
#include <QVBoxLayout>

#include "../canvas/canvas.hpp"
#include "../common/constants.hpp"
#include "../common/renderitems.hpp"
#include "../context/applicationcontext.hpp"
#include "../context/renderingcontext.hpp"

// Constants for UI styling and colors
static constexpr Qt::WindowFlags DIALOG_WINDOW_FLAGS{Qt::Dialog | Qt::FramelessWindowHint};
static constexpr int LAYOUT_SPACING{15};
static constexpr int LAYOUT_MARGINS{20};
static constexpr int TOP_LAYOUT_SPACING{0};
static constexpr int COMBO_BOX_MIN_WIDTH{140};
static constexpr int COMBO_BOX_MIN_HEIGHT{32};
static constexpr int LABEL_MIN_WIDTH{80};
static constexpr int CLOSE_BUTTON_WIDTH{30};
static constexpr int CLOSE_BUTTON_HEIGHT{30};
static constexpr int LIGHT_THRESHOLD{128};

SettingsDialog::SettingsDialog(ApplicationContext *context, QWidget *parent)
    : QDialog{parent},
      m_context{context} {
    m_setupUI();
    m_loadSettings();

    this->setWindowFlags(DIALOG_WINDOW_FLAGS);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setModal(true);
}

SettingsDialog::~SettingsDialog() {
}

void SettingsDialog::m_setupUI() {
    // Create main layout to hold the dialog content
    QVBoxLayout *mainLayout{new QVBoxLayout(this)};
    mainLayout->setSpacing(TOP_LAYOUT_SPACING);
    mainLayout->setContentsMargins(TOP_LAYOUT_SPACING,
                                   TOP_LAYOUT_SPACING,
                                   TOP_LAYOUT_SPACING,
                                   TOP_LAYOUT_SPACING);

    // Create a container widget with white background for the settings
    QWidget *containerWidget{new QWidget(this)};
    containerWidget->setStyleSheet("QWidget { background-color: #ffffff; }");

    QVBoxLayout *containerLayout{new QVBoxLayout(containerWidget)};
    containerLayout->setSpacing(LAYOUT_SPACING);
    containerLayout->setContentsMargins(LAYOUT_MARGINS,
                                        LAYOUT_MARGINS,
                                        LAYOUT_MARGINS,
                                        LAYOUT_MARGINS);

    // Create top bar with title and close button
    QHBoxLayout *topLayout{new QHBoxLayout()};
    topLayout->setSpacing(TOP_LAYOUT_SPACING);
    topLayout->setContentsMargins(TOP_LAYOUT_SPACING,
                                  TOP_LAYOUT_SPACING,
                                  TOP_LAYOUT_SPACING,
                                  TOP_LAYOUT_SPACING);

    QLabel *titleLabel{new QLabel("Settings", this)};
    titleLabel->setStyleSheet("");

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // Create close button with styling
    m_closeButton = new QPushButton("✕", this);
    m_closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;"
        "  border: none;"
        "  color: #666666;"
        "  font-size: 20px;"
        "  width: 30px;"
        "  height: 30px;"
        "  padding: 0px;"
        "  margin: 0px;"
        "}"
        "QPushButton:hover {"
        "  color: #1a1a1a;"
        "}");
    m_closeButton->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(m_closeButton);

    containerLayout->addLayout(topLayout);

    // Add separator line between title and content
    QFrame *separatorLine{new QFrame(this)};
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Sunken);
    separatorLine->setStyleSheet("color: #e0e0e0;");
    containerLayout->addWidget(separatorLine);

    // Create theme selection layout
    QHBoxLayout *themeLayout{new QHBoxLayout()};
    themeLayout->setSpacing(10);

    QLabel *themeLabel{new QLabel("Theme Mode", this)};
    themeLabel->setStyleSheet("");
    themeLabel->setMinimumWidth(LABEL_MIN_WIDTH);

    m_themeComboBox = new QComboBox(this);
    m_themeComboBox->addItem("Auto", "auto");
    m_themeComboBox->addItem("Light", "light");
    m_themeComboBox->addItem("Dark", "dark");
    m_themeComboBox->setMinimumWidth(COMBO_BOX_MIN_WIDTH);
    m_themeComboBox->setMinimumHeight(COMBO_BOX_MIN_HEIGHT);

    m_themeComboBox->setStyleSheet(
        "QComboBox {"
        "  padding: 6px 10px;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 6px;"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  font-size: 12px;"
        "  selection-background-color: #27a9ff;"
        "}"
        "QComboBox:hover {"
        "  border: 1px solid #27a9ff;"
        "}"
        "QComboBox:focus {"
        "  border: 2px solid #27a9ff;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "  width: 20px;"
        "}"
        "QComboBox::down-arrow {"
        "  image: none;"
        "  border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  selection-background-color: #27a9ff;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 6px;"
        "  outline: none;"
        "  padding: 2px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "  padding: 8px 10px;"
        "  color: #1a1a1a;"
        "  background-color: #ffffff;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "  background-color: #e8f4ff;"
        "  color: #1a1a1a;"
        "}"
        "QComboBox QAbstractItemView::item:selected {"
        "  background-color: #27a9ff;"
        "  color: #ffffff;"
        "}");

    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(m_themeComboBox);
    themeLayout->addStretch();

    containerLayout->addLayout(themeLayout);

    // Create restore last file checkbox layout
    QHBoxLayout *restoreFileLayout{new QHBoxLayout()};
    restoreFileLayout->setSpacing(10);

    m_restoreLastFileCheckBox = new QCheckBox("Restore last file on startup", this);
    m_restoreLastFileCheckBox->setStyleSheet(
        "QCheckBox {"
        "  font-size: 12px;"
        "  color: #333333;"
        "  spacing: 6px;"
        "}"
        "QCheckBox::indicator {"
        "  width: 18px;"
        "  height: 18px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "  background-color: #ffffff;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "}"
        "QCheckBox::indicator:checked {"
        "  background-color: #27a9ff;"
        "  border: 1px solid #27a9ff;"
        "  border-radius: 4px;"
        "}");

    restoreFileLayout->addWidget(m_restoreLastFileCheckBox);
    restoreFileLayout->addStretch();

    containerLayout->addLayout(restoreFileLayout);
    containerLayout->addStretch();

    mainLayout->addWidget(containerWidget);

    // Connect signal handlers for settings changes
    QObject::connect(m_themeComboBox,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,
                     [this]() { m_saveSettings(); });

    QObject::connect(m_restoreLastFileCheckBox, &QCheckBox::checkStateChanged, this, [this]() {
        m_saveSettings();
    });

    QObject::connect(m_closeButton, &QPushButton::clicked, this, &SettingsDialog::m_onCloseClicked);

    this->setLayout(mainLayout);
}

QString SettingsDialog::m_getSettingsFilePath() const {
    // Build the path to the settings file in the ~/.config/drawy directory
    QString drawyDirectoryPath{
        QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/drawy"};
    QDir drawyDir{drawyDirectoryPath};

    // Create the directory if it doesn't exist
    if (!drawyDir.exists()) {
        drawyDir.mkpath(".");
    }

    return drawyDir.filePath("settings.json");
}

void SettingsDialog::m_loadSettings() {
    // Get the path to the settings file
    QString settingsFilePath{m_getSettingsFilePath()};
    QFile settingsFile{settingsFilePath};

    // Set default values for UI elements
    m_themeComboBox->setCurrentIndex(0);  // Auto mode by default
    m_restoreLastFileCheckBox->setChecked(false);

    // Read settings from file if it exists
    if (settingsFile.open(QIODevice::ReadOnly)) {
        QByteArray fileContent{settingsFile.readAll()};
        settingsFile.close();

        QJsonDocument settingsDocument{QJsonDocument::fromJson(fileContent)};
        if (settingsDocument.isObject()) {
            QJsonObject settingsObject{settingsDocument.object()};

            // Load theme setting and set combo box index
            QString themeValue{settingsObject.value("theme").toString("auto")};
            int themeIndex{m_themeComboBox->findData(themeValue)};
            if (themeIndex >= 0) {
                m_themeComboBox->setCurrentIndex(themeIndex);
            }

            // Load restore last file setting
            bool restoreLastFileEnabled{settingsObject.value("restoreLastFile").toBool(false)};
            m_restoreLastFileCheckBox->setChecked(restoreLastFileEnabled);
            m_restoreLastFile = restoreLastFileEnabled;
        }
    }

    // Apply the loaded theme immediately
    Canvas &canvas{m_context->renderingContext().canvas()};
    QString currentTheme{m_themeComboBox->currentData().toString()};

    if (currentTheme == "light") {
        canvas.setBg(Common::lightBackgroundColor);
    } else if (currentTheme == "dark") {
        canvas.setBg(Common::darkBackgroundColor);
    } else if (currentTheme == "auto") {
        // Use system palette to determine appropriate theme
        QPalette systemPalette{QApplication::palette()};
        QColor windowBackgroundColor{systemPalette.color(QPalette::Window)};
        if (windowBackgroundColor.lightness() > LIGHT_THRESHOLD) {
            canvas.setBg(Common::lightBackgroundColor);
        } else {
            canvas.setBg(Common::darkBackgroundColor);
        }
    }

    m_context->renderingContext().markForRender();
    m_context->renderingContext().markForUpdate();
}

void SettingsDialog::m_saveSettings() {
    QString settingsFilePath{m_getSettingsFilePath()};

    QJsonObject settingsObject{};

    // Read existing settings to preserve other keys (e.g., lastOpenedFile)
    QFile settingsFile{settingsFilePath};
    if (settingsFile.exists() && settingsFile.open(QIODevice::ReadOnly)) {
        QByteArray fileContent{settingsFile.readAll()};
        settingsFile.close();

        QJsonDocument existingSettingsDoc{QJsonDocument::fromJson(fileContent)};
        if (existingSettingsDoc.isObject()) {
            settingsObject = existingSettingsDoc.object();
        }
    }

    // Update the settings we manage in this dialog
    settingsObject["theme"] = m_themeComboBox->currentData().toString();
    settingsObject["restoreLastFile"] = m_restoreLastFileCheckBox->isChecked();

    QJsonDocument settingsDocument{settingsObject};

    // Write settings back to file
    if (settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        settingsFile.write(settingsDocument.toJson());
        settingsFile.close();
    }

    m_restoreLastFile = m_restoreLastFileCheckBox->isChecked();

    // Apply the selected theme immediately to the canvas
    Canvas &canvas{m_context->renderingContext().canvas()};
    QString selectedTheme{m_themeComboBox->currentData().toString()};

    if (selectedTheme == "light") {
        canvas.setBg(Common::lightBackgroundColor);
    } else if (selectedTheme == "dark") {
        canvas.setBg(Common::darkBackgroundColor);
    } else if (selectedTheme == "auto") {
        // Use system palette to determine appropriate theme
        QPalette systemPalette{QApplication::palette()};
        QColor windowBackgroundColor{systemPalette.color(QPalette::Window)};
        if (windowBackgroundColor.lightness() > LIGHT_THRESHOLD) {
            canvas.setBg(Common::lightBackgroundColor);
        } else {
            canvas.setBg(Common::darkBackgroundColor);
        }
    }

    m_context->renderingContext().markForRender();
    m_context->renderingContext().markForUpdate();
}

void SettingsDialog::showCentered() {
    // Get the primary screen and calculate center position
    QScreen *primaryScreen{QApplication::primaryScreen()};
    if (primaryScreen) {
        QRect screenGeometry{primaryScreen->geometry()};
        int centerX{screenGeometry.center().x() - this->width() / 2};
        int centerY{screenGeometry.center().y() - this->height() / 2};
        this->move(centerX, centerY);
    }

    this->exec();
}

bool SettingsDialog::shouldRestoreLastFile() const {
    return m_restoreLastFile;
}

void SettingsDialog::m_onCloseClicked() {
    this->accept();
}