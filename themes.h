#ifndef THEMES_H
#define THEMES_H

#include <QString>

namespace Theme {
    struct Style {
        QString MainWindow;
        QString TitleBar;
        QString ControlBar;
        QString PositionSlider;
        QString PlaylistPanel;
        QString Playlist;
        QString SearchEdit;
        QString VolumeSlider;
        QString Menu;
        QString HistoryPanel;
        QString HistoryList;
        QString HistoryLabel;
        QString ClearButton;
        QString CountLabel;
        QString PlaylistItemNormal;
        QString PlaylistItemSelected;
        QString PlaylistItemNumber;
        QString PlaylistItemTitle;
        QString PlaylistItemDuration;
        QString PlaylistItemCover;
        QString PlaylistItemDeleteBtn;
        QString MessageBox;  // 新增提示框样式
        QString ProgressDialog;

    };


    const Style Light = {
        // MainWindow
        R"(
            QMainWindow {
                background: #FFFFFF;
                margin: 0;
                padding: 0;
                border: none;
            }
            QGraphicsView {
                background: #FFFFFF;
                border: none;
            }
        )",

        // TitleBar
        R"(
            background: #FFFFFF;
            margin: 0;
            padding: 0;
            color: #000000;
            font-size: 14px;
            border: none;
        )",

        // ControlBar
        R"(
            QWidget#controlBar {
                background: #EEEEEE;
                padding: 4px 0;
                box-shadow: 0 -4px 12px rgba(0, 0, 0, 0.08);

            }
            QPushButton {
                background: #FAFAFA;
                border: 1px solid #E0E0E0;
                border-radius: 4px;
                padding: 5px;
                color: #424242;
                margin: 0 2px;
            }
            QPushButton:hover {
                background: #F5F5F5;
                border-color: #BDBDBD;
            }
            QSlider::groove:horizontal {
                background: #E0E0E0;
                margin: 0;
            }
            QSlider::sub-page:horizontal {
                background: #009688;
            }
        )",

        // PositionSlider
        R"(
            QSlider::groove:horizontal {
                height: 6px;
                background: #404040;
                margin: 2px 0;
            }
            QSlider::sub-page:horizontal {
                background: #009688;
            }
            QSlider::handle:horizontal {
                width: 12px;
                margin: -6px 0;
                background: #FFFFFF;
                border: 2px solid #009688;
                border-radius: 6px;
            }
        )",

        // PlaylistPanel
        R"(
            QWidget {
                background: #EEEEEE;
                padding: 4px;
            }
        )",

        // Playlist
        R"(
            QListWidget {
                background: transparent;
                border: none;
                outline: none;
            }
            QListWidget::item {
                background: transparent;
                border-radius: 4px;
                margin: 4px 0;
                padding: 0;
                height: 60px;
            }
            QListWidget::item:hover {
                background: #F5F5F5;
            }
            QListWidget::item:selected {
                background: #009688;
            }
        )",

        // SearchEdit
        R"(
            QLineEdit {
                border: 2px solid #999999;
                border-radius: 6px;
                padding: 5px 10px;
                font-size: 13px;
                min-width: 200px;
                max-width: 400px;
                background: #FFFFFF;
            }
            QLineEdit:focus, QLineEdit:!focus:hover {
                border: 2px solid #009688;
            }
            QLineEdit::placeholder {
                color: #9E9E9E;
            }
        )",

        // VolumeSlider
        R"(
            QSlider::groove:vertical {
                width: 4px;
                background: #009688;
                margin: 0 2px;
            }
            QSlider::sub-page:vertical {
                background: #404040;
            }
            QSlider::handle:vertical {
                height: 12px;
                width: 8px;
                margin: 0 -4px;
                background: #FFFFFF;
                border: 2px solid #009688;
                border-radius: 6px;
            }
        )",

        // Menu
        R"(
            QMenu {
                background: white;
                border: 1px solid #E0E0E0;
                padding: 4px;
            }
            QMenu::item {
                padding: 4px 20px;
            }
            QMenu::item:selected {
                background: #009688;
                color: white;
            }
        )",

        // HistoryPanel
        R"(
            QWidget {
                background: #EEEEEE;
                padding: 4px;
            }
        )",

        // HistoryList
        R"(
            QListWidget {
                background: transparent;
                border: none;
                outline: none;
                margin-right: 4px;
            }
            QListWidget::item {
                background: transparent;
                border-radius: 4px;
                margin: 4px 0;
                padding: 0;
                height: 60px;
            }
            QListWidget::item:hover {
                background: #F5F5F5;
            }
            QListWidget::item:selected {
                background: #009688;
            }
            QScrollBar:vertical {
                width: 8px;
                background: #F5F5F5;
            }
            QScrollBar::handle:vertical {
                background: #BDBDBD;
                min-height: 20px;
                border-radius: 4px;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
        )",

        // HistoryLabel
        R"(
            QLabel {
                font-size: 14px;
                font-weight: bold;
                color: #424242;
                margin-left: 4px;
            }
        )",
        // ClearButton
        R"(
            QPushButton {
                background: #009688;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 5px 10px;
                font-size: 13px;
                min-width: 70px;
            }
            QPushButton:hover {
                background: #009688;
            }
        )",

        // CountLabel
        R"(
            QLabel {
                font-size: 13px;
                font-weight: bold;
                color: #666666;
                margin-left: 10px;
        }
        )",

        // PlaylistItemNormal
            "", // 默认无样式
        // PlaylistItemSelected
        R"(
            background: #009688;
            border-radius: 4px;
            color: white;
        )",

        // PlaylistItemNumber
        R"(
            color: #666666;
            background: transparent;
        )",

        // PlaylistItemTitle
        R"(
            color: #333333;
            font-weight: 500;
            background: transparent;
        )",

        // PlaylistItemDuration
        R"(
            color: #333333;
            font-size: 13px;
            padding-right: 8px;
            background: transparent;
        )",

        // PlaylistItemCover
        R"(
            background: transparent;
            border-radius: 4px;
        )",

        // PlaylistItemDeleteBtn
        R"(
            QPushButton {
                background: transparent;
                border: none;
                color: white;
            }
        )",
        //MessageBox
        R"(
            QMessageBox {
                background-color: #FFFFFF;
            }
            QMessageBox QLabel {
                color: #333333;
            }
            QMessageBox QPushButton {
                background: #F5F5F5;
                border: 1px solid #E0E0E0;
                color: #333333;
                padding: 5px 10px;
                border-radius: 4px;
            }
            QMessageBox QPushButton:hover {
                background: #E0E0E0;
            }
        )",
        R"(
            QProgressDialog {
                background: white;
                border: 1px solid #E0E0E0;
                border-radius: 4px;
                padding: 8px;
            }
            QProgressDialog QLabel {
                color: #666666;
                font-size: 14px;
            }
            QProgressBar {
                border: 1px solid #999999;
                background: white;
                text-align: center;
                margin: 4px;
            }
            QProgressBar::chunk {
                background-color: #009688;
                width: 10px;
                margin: 0.5px;
            }
            QPushButton {
                background: #F5F5F5;
                border: 1px solid #E0E0E0;
                padding: 5px 10px;
                color: #424242;
            }
            QPushButton:hover {
                background: #E0E0E0;
                border-color: #BDBDBD;
            }


        )"


    };

    const Style Dark = {
        // MainWindow
        R"(
            QMainWindow {
                background: #121212;
                margin: 0;
                padding: 0;
                border: none;
            }
            QGraphicsView {
                background: #000000;
                border: none;
            }
        )",

        // TitleBar
        R"(
            background: #1E1E1E;
            margin: 0;
            padding: 0;
            color: #FFFFFF;
            font-size: 14px;
            border: none;
        )",

        // ControlBar
        R"(
            QWidget#controlBar {
                background: #2D2D2D;
                padding: 4px 0;
                box-shadow: 0 -4px 12px rgba(0, 0, 0, 0.3);
            }
            QPushButton {
                background: #424242;
                border: 1px solid #555555;
                border-radius: 4px;
                padding: 5px;
                color: #E0E0E0;
                margin: 0 2px;
            }
            QPushButton:hover {
                background: #555555;
                border-color: #777777;
            }
            QSlider::groove:horizontal {
                background: #555555;
                margin: 0;
            }
            QSlider::sub-page:horizontal {
                background: #009688;
            }
        )",

        // PositionSlider
        R"(
            QSlider::groove:horizontal {
                height: 6px;
                background: #555555;
                margin: 2px 0;
            }
            QSlider::sub-page:horizontal {
                background: #009688;
            }
            QSlider::handle:horizontal {
                width: 12px;
                margin: -6px 0;
                background: #222222;
                border: 2px solid #009688;
                border-radius: 6px;
            }
        )",

        // PlaylistPanel
        R"(
            QWidget {
                background: #2D2D2D;
                padding: 4px;
            }
        )",

        // Playlist
        R"(
            QListWidget {
                background: transparent;
                border: none;
                outline: none;
            }
            QListWidget::item {
                background: transparent;
                border-radius: 4px;
                margin: 4px 0;
                padding: 0;
                height: 60px;
            }
            QListWidget::item:hover {
                background: #3D3D3D;
            }
            QListWidget::item:selected {
                background: #009688;
            }
        QScrollBar:vertical {
                width: 8px;
                background: #3D3D3D;
            }
            QScrollBar::handle:vertical {
                background: #555555;
                min-height: 20px;
                border-radius: 4px;
            }
            QScrollBar::add-line:vertical,
            QScrollBar::sub-line:vertical {
                height: 0px;
            }
        )",

        // SearchEdit
        R"(
            QLineEdit {
                border: 2px solid #555555;
                border-radius: 6px;
                padding: 5px 10px;
                font-size: 13px;
                min-width: 200px;
                max-width: 400px;
                background: #424242;
                color: #E0E0E0;
            }
            QLineEdit:focus, QLineEdit:!focus:hover {
                border: 2px solid #009688;
            }
            QLineEdit::placeholder {
                color: #9E9E9E;
            }
        )",

        // VolumeSlider
        R"(
            QSlider::groove:vertical {
                width: 4px;
                background: #009688;
                margin: 0 2px;
            }
            QSlider::sub-page:vertical {
                background: #EEEEEE;
            }
            QSlider::handle:vertical {
                height: 12px;
                width: 8px;
                margin: 0 -4px;
                background: #000000;
                border: 2px solid #009688;
                border-radius: 6px;
            }
        )",

        // Menu
        R"(
            QMenu {
                background: #424242;
                border: 1px solid #555555;
                padding: 4px;
                color: #E0E0E0;
            }
            QMenu::item {
                padding: 4px 20px;
            }
            QMenu::item:selected {
                background: #009688;
                color: white;
            }
        )",

        // HistoryPanel
        R"(
            QWidget {
                background: #2D2D2D;
                padding: 4px;
            }
        )",

        // HistoryList
        R"(
            QListWidget {
                background: transparent;
                border: none;
                outline: none;
                margin-right: 4px;
            }
            QListWidget::item {
                background: transparent;
                border-radius: 4px;
                margin: 4px 0;
                padding: 0;
                height: 60px;
            }
            QListWidget::item:hover {
                background: #3D3D3D;
            }
            QListWidget::item:selected {
                background: #009688;
            }
        QScrollBar:vertical {
                width: 8px;
                background: #3D3D3D;
            }
            QScrollBar::handle:vertical {
                background: #555555;
                min-height: 20px;
                border-radius: 4px;
            }
            QScrollBar::add-line:vertical,
            QScrollBar::sub-line:vertical {
                height: 0px;
            }

        )",
        // HistoryLabel
        R"(
            QLabel {
                font-size: 14px;
                font-weight: bold;
                color: #FFFFFF;
                margin-left: 4px;
            }
        )",
        // ClearButton
        R"(
            QPushButton {
                background: #009688;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 5px 10px;
                font-size: 13px;
                min-width: 70px;
            }
            QPushButton:hover {
                background: #009688;
            }
        )",

        // CountLabel
        R"(
            QLabel {
                font-size: 13px;
                font-weight: bold;
                color: #FFFFFF;
                margin-left: 10px;
            }
        )",
        // PlaylistItemNormal
        "",
        // 默认无样式

        // PlaylistItemSelected
        R"(
            background: #009688;
            border-radius: 4px;
            color: white;
        )",

        // PlaylistItemNumber
        R"(
            color: #9E9E9E;
            background: transparent;
        )",

        // PlaylistItemTitle
        R"(
            color: #E0E0E0;
            font-weight: 500;
            background: transparent;
        )",

        // PlaylistItemDuration
        R"(
            color: #9E9E9E;
            font-size: 13px;
            padding-right: 8px;
            background: transparent;
        )",

        // PlaylistItemCover
        R"(
            background: transparent;
            border-radius: 4px;
        )",

        // PlaylistItemDeleteBtn
        R"(
            QPushButton {
                background: transparent;
                border: none;
                color: white;
            }
        )",
        //QString MessageBox;
        R"(
            QMessageBox {
                background-color: #2D2D2D;
            }
            QMessageBox QLabel {
                color: #E0E0E0;
            }
            QMessageBox QPushButton {
                background: #424242;
                border: 1px solid #555555;
                color: #E0E0E0;
                padding: 5px 10px;
                border-radius: 4px;
            }
            QMessageBox QPushButton:hover {
                background: #555555;
            }
        )",
        R"(
            QProgressDialog {
                background: #2D2D2D;
                border: 1px solid #555555;
                border-radius: 4px;
                padding: 8px;
                color: #E0E0E0;
            }
            QProgressDialog QLabel {
                color: #9E9E9E;
                font-size: 14px;
            }
            QProgressBar {
                border: 1px solid #555555;
                background: #3D3D3D;
                text-align: center;
                margin: 4px;
            }
            QProgressBar::chunk {
                background-color: #009688;
                width: 10px;
                margin: 0.5px;
            }
            QPushButton {
                background: #424242;
                border: 1px solid #555555;
                padding: 5px 10px;
                color: #E0E0E0;
                border-radius: 4px;
                font-size: 13px;
            }
            QPushButton:hover {
                background: #555555;
                border-color: #777777;
            }


        )"
    };


}

#endif // THEMES_H
