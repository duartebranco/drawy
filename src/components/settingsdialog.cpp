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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QPalette>
#include <QScreen>
#include <QPushButton>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "../context/applicationcontext.hpp"
#include "../context/renderingcontext.hpp"
#include "../canvas/canvas.hpp"
#include "../common/renderitems.hpp"
#include "../common/constants.hpp"

SettingsDialog::SettingsDialog(ApplicationContext *context, QWidget *parent)
    : QDialog(parent), m_context(context) {
    m_setupUI();
    m_loadSettings();
    
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setModal(true);
}

SettingsDialog::~SettingsDialog() {
}

void SettingsDialog::m_setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create a container widget with background
    QWidget *containerWidget = new QWidget(this);
    containerWidget->setStyleSheet(
        "QWidget {"
        "  background-color: #ffffff;"
        "}"
    );
    
    QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);
    containerLayout->setSpacing(15);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    
    // Top bar with title and close button
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *titleLabel = new QLabel("Settings", this);
    titleLabel->setStyleSheet("");
    
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    
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
        "}"
    );
    m_closeButton->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(m_closeButton);
    
    containerLayout->addLayout(topLayout);
    
    // Separator line
    QFrame *separatorLine = new QFrame(this);
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Sunken);
    separatorLine->setStyleSheet("color: #e0e0e0;");
    containerLayout->addWidget(separatorLine);
    
    // Theme option layout
    QHBoxLayout *themeLayout = new QHBoxLayout();
    themeLayout->setSpacing(10);
    
    QLabel *themeLabel = new QLabel("Theme Mode", this);
    themeLabel->setStyleSheet("");
    themeLabel->setMinimumWidth(80);
    
    m_themeComboBox = new QComboBox(this);
    m_themeComboBox->addItem("Auto", "auto");
    m_themeComboBox->addItem("Light", "light");
    m_themeComboBox->addItem("Dark", "dark");
    m_themeComboBox->setMinimumWidth(140);
    m_themeComboBox->setMinimumHeight(32);
    
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
        "}"
    );
    
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(m_themeComboBox);
    themeLayout->addStretch();
    
    containerLayout->addLayout(themeLayout);
    
    // Restore last file option
    QHBoxLayout *restoreFileLayout = new QHBoxLayout();
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
        "}"
    );
    
    restoreFileLayout->addWidget(m_restoreLastFileCheckBox);
    restoreFileLayout->addStretch();
    
    containerLayout->addLayout(restoreFileLayout);
    containerLayout->addStretch();
    
    mainLayout->addWidget(containerWidget);
    
    // Connect changes
    QObject::connect(m_themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this, [this]() {
        m_saveSettings();
    });
    
    QObject::connect(m_restoreLastFileCheckBox, &QCheckBox::checkStateChanged,
                     this, [this]() {
        m_saveSettings();
    });
    
    QObject::connect(m_closeButton, &QPushButton::clicked,
                     this, &SettingsDialog::m_onCloseClicked);
    
    this->setLayout(mainLayout);
}

QString SettingsDialog::m_getSettingsFilePath() const {
    QDir drawyDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.drawy");
    if (!drawyDir.exists()) {
        drawyDir.mkpath(".");
    }
    return drawyDir.filePath("settings.json");
}

void SettingsDialog::m_loadSettings() {
    QString filePath = m_getSettingsFilePath();
    QFile file(filePath);
    
    // Default values
    m_themeComboBox->setCurrentIndex(0); // Auto
    m_restoreLastFileCheckBox->setChecked(false);
    
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            
            // Load theme
            QString theme = obj.value("theme").toString("auto");
            int index = m_themeComboBox->findData(theme);
            if (index >= 0) {
                m_themeComboBox->setCurrentIndex(index);
            }
            
            // Load restore last file setting
            bool restoreLastFile = obj.value("restoreLastFile").toBool(false);
            m_restoreLastFileCheckBox->setChecked(restoreLastFile);
            m_restoreLastFile = restoreLastFile;
        }
    }
    
    // Apply theme on load
    Canvas &canvas = m_context->renderingContext().canvas();
    QString theme = m_themeComboBox->currentData().toString();
    
    if (theme == "light") {
        canvas.setBg(Common::lightBackgroundColor);
    } else if (theme == "dark") {
        canvas.setBg(Common::darkBackgroundColor);
    } else if (theme == "auto") {
        // Use system palette to determine
        QPalette systemPalette = QApplication::palette();
        QColor bgColor = systemPalette.color(QPalette::Window);
        if (bgColor.lightness() > 128) {
            canvas.setBg(Common::lightBackgroundColor);
        } else {
            canvas.setBg(Common::darkBackgroundColor);
        }
    }
    
    m_context->renderingContext().markForRender();
    m_context->renderingContext().markForUpdate();
}

void SettingsDialog::m_saveSettings() {
    QString filePath = m_getSettingsFilePath();
    
    QJsonObject obj;
    
    // Read existing settings first to preserve other keys (like lastOpenedFile)
    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument existingDoc = QJsonDocument::fromJson(data);
        if (existingDoc.isObject()) {
            obj = existingDoc.object();
        }
    }
    
    // Update only the settings we manage in this dialog
    obj["theme"] = m_themeComboBox->currentData().toString();
    obj["restoreLastFile"] = m_restoreLastFileCheckBox->isChecked();
    
    QJsonDocument doc(obj);
    
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson());
        file.close();
    }
    
    m_restoreLastFile = m_restoreLastFileCheckBox->isChecked();
    
    // Apply the selected theme immediately
    Canvas &canvas = m_context->renderingContext().canvas();
    QString selectedTheme = m_themeComboBox->currentData().toString();
    
    if (selectedTheme == "light") {
        canvas.setBg(Common::lightBackgroundColor);
    } else if (selectedTheme == "dark") {
        canvas.setBg(Common::darkBackgroundColor);
    } else if (selectedTheme == "auto") {
        QPalette systemPalette = QApplication::palette();
        QColor bgColor = systemPalette.color(QPalette::Window);
        if (bgColor.lightness() > 128) {
            canvas.setBg(Common::lightBackgroundColor);
        } else {
            canvas.setBg(Common::darkBackgroundColor);
        }
    }
    
    m_context->renderingContext().markForRender();
    m_context->renderingContext().markForUpdate();
}

void SettingsDialog::showCentered() {
    // Center the dialog on the screen
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = screenGeometry.center().x() - this->width() / 2;
        int y = screenGeometry.center().y() - this->height() / 2;
        this->move(x, y);
    }
    
    this->exec();
}

bool SettingsDialog::shouldRestoreLastFile() const {
    return m_restoreLastFile;
}

void SettingsDialog::m_onCloseClicked() {
    this->accept();
}