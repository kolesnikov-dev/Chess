#include "MenuWidget.h"
#include "StartGameWidget.h"

#include <fstream>

#include <QApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>

MenuWidget::MenuWidget(QWidget* parent) : mainWindow(nullptr), startGameWidget(nullptr)
{
    QPushButton* newGameButton = new QPushButton("&New Game", this);
    newGameButton->setMinimumWidth(newGameButton->minimumSizeHint().width() + newGameButton->minimumSizeHint().width() / 2);
    resumeButton = new QPushButton("&Resume", this);
    isPreviousGameFilename() ? resumeButton->setDisabled(false) : resumeButton->setDisabled(true);
    QPushButton* openButton = new QPushButton("&Open", this);
    QPushButton* quitButton = new QPushButton("&Quit", this);

    connect(newGameButton, &QPushButton::clicked, this, &MenuWidget::showStartGameWidget);
    connect(resumeButton, &QPushButton::clicked, this, &MenuWidget::resumeGame);
    connect(openButton, &QPushButton::clicked, this, &MenuWidget::showOpenFileDialog);
    connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    QVBoxLayout* buttonsVBox = new QVBoxLayout;
    auto margin = newGameButton->minimumSizeHint().width() / 2;
    buttonsVBox->setContentsMargins(margin, margin, margin, margin);
    buttonsVBox->setSpacing(newGameButton->size().height() / 2);
    buttonsVBox->addWidget(newGameButton);
    buttonsVBox->addWidget(resumeButton);
    buttonsVBox->addWidget(openButton);
    buttonsVBox->addSpacing(newGameButton->size().height() / 2);
    buttonsVBox->addWidget(quitButton);

    QGroupBox* groupBox = new QGroupBox("MENU", this);
    groupBox->setAlignment(Qt::AlignHCenter);
    groupBox->setLayout(buttonsVBox);

    QGridLayout* menuLayout = new QGridLayout;
    menuLayout->setRowStretch(0, 1);
    menuLayout->setRowStretch(2, 1);
    menuLayout->setColumnStretch(0, 1);
    menuLayout->setColumnStretch(2, 1);
    menuLayout->addWidget(groupBox, 1, 1);
    setLayout(menuLayout);
}

MenuWidget::~MenuWidget()
{
    if (mainWindow)
        delete mainWindow;

    if (startGameWidget)
        delete startGameWidget;
}

void MenuWidget::showStartGameWidget()
{
    if (!startGameWidget);
    {
        startGameWidget = new StartGameWidget(this);
        startGameWidget->showMaximized();
        hide();
        connect(startGameWidget, &StartGameWidget::mainWindowCreated, this, &MenuWidget::createMainWindow);
    }
}

void MenuWidget::createMainWindow(MainWindow* mainWindow)
{
    if (this->mainWindow)
    {
        delete this->mainWindow;
        if (startGameWidget)
            delete startGameWidget;
        startGameWidget = nullptr;
    }

    this->mainWindow = mainWindow;
    connect(this->mainWindow, &MainWindow::showApplicationMenu, this, &MenuWidget::show);
    connect(this->mainWindow, &MainWindow::showApplicationMenu, this->mainWindow, &MainWindow::hide);
    resumeButton->setDisabled(false);
}

void MenuWidget::showOpenFileDialog()
{
    auto fileName = QFileDialog::getOpenFileName(this, "Open chess game", gameArchiveDirectory, "Chess game file (*.txt)");
    if (fileName != QString())
    {
        if (mainWindow)
        {
            mainWindow->clear();
            mainWindow->showMaximized();
        }
        else
        {
            createMainWindow(new MainWindow);
            mainWindow->showMaximized();
        }
        hide();
        mainWindow->getChessboardController()->readMoves(fileName.toStdString());
    }
}

void MenuWidget::resumeGame()
{
    if (mainWindow)
    {
        mainWindow->showMaximized();
    }
    else
    {
        createMainWindow(new MainWindow);
        mainWindow->showMaximized();
        mainWindow->getChessboardController()->readMoves(previousGameFilename.toStdString());
    }
    hide();
}

bool MenuWidget::isPreviousGameFilename()
{
    previousGameFilename = ChessBoardController::getLastFilePath().c_str();
    return !previousGameFilename.isEmpty();
}