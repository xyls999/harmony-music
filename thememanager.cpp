#include "thememanager.h"
#include <QApplication>
#include <QWidget>

ThemeManager::ThemeManager(QObject *parent) : QObject(parent) {}

void ThemeManager::applyTheme(ThemeType type, const QColor& customColor) {
    switch(type) {
    case Light:
        m_backgroundColor = Qt::white;
        m_textColor = Qt::black;
        m_highlightColor = QColor("#009688").lighter(130);
        break;
    case Dark:
        m_backgroundColor = QColor("#4D4D4D");
        m_textColor = Qt::white;
        m_highlightColor = QColor("#4D4D4D").lighter(110);
        break;
    case green:
        m_backgroundColor = QColor("#E3EEE8");
        m_textColor = Qt::black;
        m_highlightColor = QColor("#558970").lighter(110);
        break;
    case Custom:
        m_backgroundColor = customColor;
        m_textColor = getContrastColor(customColor);
        m_highlightColor = isDarkTheme(m_backgroundColor)? customColor.lighter(130):customColor.darker(120);
        break;
    }

    m_currentTheme = type;
    emit themeChanged();

    // 全局应用样式
    QString styleSheet = QString(
        "QWidget {"
        "   background-color: %1;"
        "   color: %2;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: %3;"
        "}"
    ).arg(m_backgroundColor.name(),
          m_textColor.name(),
          m_highlightColor.name());

    qApp->setStyleSheet(styleSheet);
    saveSettings();
}

bool ThemeManager::isDarkTheme(const QColor &bgColor) {
    // 计算相对亮度（公式：0.299*R + 0.587*G + 0.114*B）
    return (bgColor.red()*0.299 + bgColor.green()*0.587 + bgColor.blue()*0.114) < 128;
}

QColor ThemeManager::getContrastColor(const QColor &color) {
    return isDarkTheme(color) ? Qt::white : Qt::black;
}

void ThemeManager::loadSettings() {
    m_currentTheme = static_cast<ThemeType>(
        m_settings.value("Theme", Light).toInt());
    applyTheme(m_currentTheme);
}

void ThemeManager::saveSettings() {
    m_settings.setValue("Theme", m_currentTheme);
}
