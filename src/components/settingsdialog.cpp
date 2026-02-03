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
#include <QSettings>
#include <QApplication>
#include <QPalette>
#include <QScreen>

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
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Create a container widget with background
    QWidget *containerWidget = new QWidget(this);
    containerWidget->setStyleSheet(
        "QWidget {"
        "  background-color: #ffffff;"
        "  border: 1px solid #e0e0e0;"
        "  border-radius: 12px;"
        "}"
    );
    
    QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);
    containerLayout->setSpacing(15);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    QLabel *titleLabel = new QLabel("Settings", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  color: #1a1a1a;"
        "  margin-bottom: 10px;"
        "}"
    );
    containerLayout->addWidget(titleLabel);
    
    // Separator line
    QFrame *separatorLine = new QFrame(this);
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Sunken);
    separatorLine->setStyleSheet("color: #e0e0e0;");
    containerLayout->addWidget(separatorLine);
    
    // Theme option layout
    QHBoxLayout *themeLayout = new QHBoxLayout();
    themeLayout->setSpacing(10);
    
    QLabel *themeLabel = new QLabel("Theme Mode:", this);
    themeLabel->setStyleSheet(
        "QLabel {"
        "  font-size: 12px;"
        "  color: #333333;"
        "  font-weight: 500;"
        "}"
    );
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
    containerLayout->addStretch();
    
    mainLayout->addWidget(containerWidget);
    
    // Connect changes
    QObject::connect(m_themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this, [this]() {
        m_saveSettings();
    });
    
    this->setLayout(mainLayout);
    this->setFixedSize(400, 180);
}

void SettingsDialog::m_loadSettings() {
    QSettings settings("io.github.prayag2", "Drawy");
    QString theme = settings.value("theme", "auto").toString();
    
    int index = m_themeComboBox->findData(theme);
    if (index >= 0) {
        m_themeComboBox->setCurrentIndex(index);
    }
    
    // Apply theme on load
    Canvas &canvas = m_context->renderingContext().canvas();
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
    QSettings settings("io.github.prayag2", "Drawy");
    QString selectedTheme = m_themeComboBox->currentData().toString();
    settings.setValue("theme", selectedTheme);
    settings.sync();
    
    // Apply the selected theme immediately
    Canvas &canvas = m_context->renderingContext().canvas();
    
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