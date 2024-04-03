#pragma once

#include "../Controller/ControllerStates.h"
#include "MainWindow.h"
#include "StartGameWidget.h"

#include <QWidget>
#include <QString>
#include <QPushButton>

class MenuWidget : public QWidget
{
    Q_OBJECT

public:
    MenuWidget(QWidget* parent = nullptr);
    ~MenuWidget();

private slots:
    void createMainWindow(MainWindow* mainWindow);
    void resumeGame();

    void showStartGameWidget();
    void showOpenFileDialog();

private:
    bool isPreviousGameFilename();

private:
    MainWindow* mainWindow;
    StartGameWidget* startGameWidget;
    QPushButton* resumeButton;

    QString previousGameFilename;
};
