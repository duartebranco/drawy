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

#include "serializer.hpp"

#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QFile>
#include <format>
#include <memory>

#include "../common/constants.hpp"
#include "../common/utils/compression.hpp"
#include "../context/applicationcontext.hpp"
#include "../context/renderingcontext.hpp"
#include "../context/spatialcontext.hpp"
#include "../data-structures/quadtree.hpp"
#include "../item/freeform.hpp"
#include "../item/item.hpp"
#include "../item/polygon.hpp"
#include "../item/text.hpp"

Serializer::Serializer() {
}

void Serializer::serialize(ApplicationContext *context) {
    QVector<std::shared_ptr<Item>> items{context->spatialContext().quadtree().getAllItems()};

    QJsonArray array{};
    for (auto &item : items) {
        array.push_back(toJson(item));
    }

    m_object["items"] = array;

    QPointF offsetPos{context->spatialContext().offsetPos()};
    m_object["offset_pos"] = toJson(offsetPos);

    qreal zoomFactor{context->renderingContext().zoomFactor()};
    m_object["zoom_factor"] = zoomFactor;
}

QJsonObject Serializer::toJson(const std::shared_ptr<Item>& item) {
    QJsonObject obj{};

    obj["type"] = QJsonValue(static_cast<int>(item->type()));
    obj["bounding_box"] = toJson(item->boundingBox());
    obj["bounding_box_padding"] = QJsonValue(item->boundingBoxPadding());
    obj["properties"] = toJson(item->properties());

    switch (item->type()) {
        case Item::Freeform: {
            std::shared_ptr<FreeformItem> freeform{std::dynamic_pointer_cast<FreeformItem>(item)};
            obj["points"] = toJson(freeform->points());
            obj["pressures"] = toJson(freeform->pressures());
            break;
        }
        case Item::Rectangle:
        case Item::Ellipse:
        case Item::Arrow:
        case Item::Line: {
            std::shared_ptr<PolygonItem> polygon{std::dynamic_pointer_cast<PolygonItem>(item)};
            obj["start"] = toJson(polygon->start());
            obj["end"] = toJson(polygon->end());
            break;
        }
        case Item::Text: {
            std::shared_ptr<TextItem> text{std::dynamic_pointer_cast<TextItem>(item)};
            obj["text"] = QJsonValue(text->text());
        }
    }

    return obj;
}

QJsonObject Serializer::toJson(const Property &property) {
    QJsonObject result{};

    result["type"] = property.type();
    result["value"] = QJsonValue::fromVariant(property.variant());

    return result;
}

QJsonObject Serializer::toJson(const QRectF &rect) {
    QJsonObject result{};
    result["x"] = QJsonValue(rect.x());
    result["y"] = QJsonValue(rect.y());
    result["width"] = QJsonValue(rect.width());
    result["height"] = QJsonValue(rect.height());

    return result;
}

QJsonObject Serializer::toJson(const QPointF &point) {
    QJsonObject result{};
    result["x"] = QJsonValue(point.x());
    result["y"] = QJsonValue(point.y());

    return result;
}

QString Serializer::getCurrentFilePath() const {
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.drawy/settings.json";
    QFile settingsFile(settingsPath);
    
    if (settingsFile.exists() && settingsFile.open(QIODevice::ReadOnly)) {
        QByteArray data = settingsFile.readAll();
        settingsFile.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            return obj.value("lastOpenedFile").toString("");
        }
    }
    
    return QString();
}

void Serializer::saveToFile() {
    QJsonDocument doc{m_object};

    qDebug() << "Saving...";

    QDir homeDir{QDir::home()};

    auto text = std::format("Untitled.{}", Common::drawyFileExt);
    QString defaultFilePath = homeDir.filePath(text.data());

    text = std::format("Drawy (*.{})", Common::drawyFileExt);
    QString fileName{
        QFileDialog::getSaveFileName(nullptr, "Save File", defaultFilePath, text.data())};

    if (fileName.isEmpty()) {
        qDebug() << "Save cancelled by user";
        return;
    }

    auto data{doc.toJson(QJsonDocument::Compact)};
    auto compressedData{Common::Utils::Compression::compressData(data)};

    QFile file{fileName};
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << file.errorString();
        return;
    }
    qint64 written = file.write(compressedData);
    file.close();

    if (written != compressedData.size()) {
        qWarning() << "Warning: not all bytes were written";
        return;
    }

    qDebug() << "Saved to file: " << fileName;

    // Save the file path to settings
    saveLastOpenedFile(fileName);
}

bool Serializer::saveCurrentFile() {
    QString fileName = getCurrentFilePath();
    
    // If no file is currently open, return false
    if (fileName.isEmpty() || !QFile::exists(fileName)) {
        qDebug() << "No current file to save";
        return false;
    }

    qDebug() << "Saving to current file:" << fileName;

    QJsonDocument doc{m_object};
    auto data{doc.toJson(QJsonDocument::Compact)};
    auto compressedData{Common::Utils::Compression::compressData(data)};

    QFile file{fileName};
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << file.errorString();
        return false;
    }
    qint64 written = file.write(compressedData);
    file.close();

    if (written != compressedData.size()) {
        qWarning() << "Warning: not all bytes were written";
        return false;
    }

    qDebug() << "Saved to file: " << fileName;
    return true;
}

void Serializer::saveLastOpenedFile(const QString &filePath) const {
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.drawy/settings.json";
    QFile settingsFile(settingsPath);
    
    QJsonDocument settingsDoc;
    QJsonObject settingsObj;
    
    // Read existing settings
    if (settingsFile.exists() && settingsFile.open(QIODevice::ReadOnly)) {
        QByteArray data = settingsFile.readAll();
        settingsFile.close();
        settingsDoc = QJsonDocument::fromJson(data);
        if (settingsDoc.isObject()) {
            settingsObj = settingsDoc.object();
        }
    }
    
    // Update with new file path
    settingsObj.insert("lastOpenedFile", filePath);
    settingsDoc.setObject(settingsObj);
    
    // Write back
    if (settingsFile.open(QIODevice::WriteOnly)) {
        settingsFile.write(settingsDoc.toJson());
        settingsFile.close();
    }
}