#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QColor>
#include <QSettings>

class ThemeManager : public QObject {
    Q_OBJECT
public:
    enum ThemeType {
        Light,
        Dark,
        green,
        Custom
    };
    Q_ENUM(ThemeType)

    static ThemeManager& instance() {
        static ThemeManager instance;
        return instance;
    }
    void applyTheme(ThemeType type, const QColor& customColor = Qt::white);
    void loadSettings();
    void saveSettings();

    // 颜色计算方法
    static bool isDarkTheme(const QColor& bgColor);
    static QColor getContrastColor(const QColor& color);

    // 当前主题属性
    Q_PROPERTY(ThemeType currentTheme READ currentTheme NOTIFY themeChanged)
    ThemeType currentTheme() const { return m_currentTheme; }

    // 颜色属性
    QColor backgroundColor() const { return m_backgroundColor; }
    QColor textColor() const { return m_textColor; }
    QColor highlightColor() const { return m_highlightColor; }

signals:
    void themeChanged();

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ThemeType m_currentTheme = Light;
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_highlightColor;
    QSettings m_settings{"MyCompany", "MyMusicApp"};
};

#endif // THEMEMANAGER_H
