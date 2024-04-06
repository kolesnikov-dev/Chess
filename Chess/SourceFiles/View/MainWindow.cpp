#include "MainWindow.h"

#include <fstream>

#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QTextCursor>
#include <QTextTableCell>
#include <QTextTableFormat>
#include <QTextTable>
#include <QString>
#include <QScrollBar>
#include <QSize>
#include <QLabel>
#include <QGridLayout>
#include <QStatusBar>
#include <QToolBar>
#include <QIcon>
#include <QPushButton>

MainWindow::MainWindow(const QString& firstPlayerName, const QString& secondPlayerName, const Color firstPlayerColor, const Color secondPlayerColor) :
    QIconsFolderName(iconsFolderName)
{
    chBController = new ChessBoardController(*new ChessBoardModel(firstPlayerColor, secondPlayerColor));
    chBView = chBController->getChessboardView();

    const QString& whiteFiguresPlayerName = firstPlayerColor == Color::White ? firstPlayerName : secondPlayerName;
    const QString& blackFiguresPlayerName = firstPlayerColor == Color::Black ? firstPlayerName : secondPlayerName;
    createMovesHistoryEdit(whiteFiguresPlayerName, blackFiguresPlayerName);
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();

    QPushButton* giveUpButton = new QPushButton(this);
    giveUpButton->setIcon(QIcon(QIconsFolderName + "white-flag_.png"));
    giveUpButton->setIconSize(QSize(30, 30));
    connect(giveUpButton, &QPushButton::clicked, [this]()
        {
            chBController->giveUp();
        });

    QVBoxLayout* movesHistoryLayout = new QVBoxLayout;
    movesHistoryLayout->addStretch(1);
    movesHistoryLayout->addWidget(movesHistoryEdit, 8);
    movesHistoryLayout->addStretch(1); 

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addLayout(movesHistoryLayout, 0, 0);
    gridLayout->addWidget(chBView, 0, 1);
    gridLayout->addWidget(giveUpButton, 0, 2, Qt::AlignCenter);

    QWidget* centralWidget = new QWidget();
    centralWidget->setLayout(gridLayout);
    setCentralWidget(centralWidget);

    connect(chBView, &ChessBoardView::notatedLastMove, this, &MainWindow::addToTextEdit);
    connect(chBView, &ChessBoardView::undoMove, movesHistoryEdit, &QTextEdit::undo);
    connect(chBView, &ChessBoardView::redoMove, movesHistoryEdit, &QTextEdit::redo);
    connect(chBView, &ChessBoardView::showApplicationMenu, this, &MainWindow::showApplicationMenu);

    eventFilter = new EventFilter;
    connect(eventFilter, &EventFilter::needToSave, this, &MainWindow::save);
    connect(qApp, &QApplication::aboutToQuit, [this]() { delete eventFilter; });
    qApp->installEventFilter(eventFilter);
}

MainWindow::~MainWindow()
{
    qApp->removeEventFilter(eventFilter);
    delete chBController; 
}

void MainWindow::createToolBar()
{
    auto* toolBar = addToolBar("Edit");
    toolBar->setMovable(false);
    toolBar->addAction(undoAct);
    toolBar->addAction(redoAct);
    setIconSize(QSize(20, 20));
}

void MainWindow::createStatusBar()
{
    QLabel* infoMessageLabel = new QLabel();
    infoMessageLabel->setAlignment(Qt::AlignCenter);
    QStatusBar* statusBar = new QStatusBar();
    statusBar->setSizeGripEnabled(false);
    statusBar->addWidget(infoMessageLabel, 1);
    infoMessageLabel->setText("It is white figures turn to move");
    connect(chBView, &ChessBoardView::infoMessage, infoMessageLabel, &QLabel::setText);
    setStatusBar(statusBar);
}

void MainWindow::createActions()
{
    saveAct = new QAction("Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip("Save a current file");
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    saveAsAct = new QAction("Save as", this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setStatusTip("Choose a file path to save a current file in");
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    openAct = new QAction("Open", this);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip("Open a new file");
    openAct->setIcon(QIcon(QIconsFolderName + "folder-open_.png"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    undoAct = new QAction("Undo", this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setStatusTip("Undo the last move");
    undoAct->setIcon(QIcon(QIconsFolderName + "undo_.png"));
    undoAct->setEnabled(false);
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction("Redo", this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setStatusTip("Redo canceled move");
    redoAct->setIcon(QIcon(QIconsFolderName + "redo_.png"));
    redoAct->setEnabled(false);
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    clearAct = new QAction("Clear", this);
    clearAct->setStatusTip("Undo all moves");
    clearAct->setIcon(QIcon(QIconsFolderName + "clear_.png"));
    connect(clearAct, &QAction::triggered, this, &MainWindow::clear);

    moveTipsAct = new QAction("Move tips", this);
    moveTipsAct->setStatusTip("Highlight available moves positions");
    moveTipsAct->setCheckable(true);
    moveTipsAct->setChecked(true);
    connect(moveTipsAct, &QAction::toggled, chBView, &ChessBoardView::moveTips);

    lastMoveTipsAct = new QAction("Last move tips", this);
    lastMoveTipsAct->setStatusTip("Highlight last move positions");
    lastMoveTipsAct->setCheckable(true);
    lastMoveTipsAct->setChecked(true);
    connect(lastMoveTipsAct, &QAction::triggered, chBView, &ChessBoardView::lastMoveTip);

    backToMenuAct = new QAction("Back to menu", this);
    backToMenuAct->setStatusTip("Back to application menu");
    backToMenuAct->setIcon(QIcon(QIconsFolderName + "back_.png"));
    connect(backToMenuAct, &QAction::triggered, this, &MainWindow::showApplicationMenu);

    exitAct = new QAction("Quit", this);
    exitAct->setStatusTip("Quit the application");
    exitAct->setIcon(QIcon(QIconsFolderName + "exit-filled_.png"));
    connect(exitAct, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("File");
    saveMenu = fileMenu->addMenu("Save");
    saveMenu->addAction(saveAct);
    saveMenu->addAction(saveAsAct);
    saveMenu->setIcon(QIcon(QIconsFolderName + "save_.png"));
    fileMenu->addSeparator();
    fileMenu->addAction(openAct);

    editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(clearAct);

    generalMenu = menuBar()->addMenu("General");
    generalMenu->addAction(moveTipsAct);
    generalMenu->addAction(lastMoveTipsAct);
    generalMenu->addSeparator();
    generalMenu->addAction(backToMenuAct);
    generalMenu->addAction(exitAct);
}

void MainWindow::createMovesHistoryEdit(const QString& whiteFiguresPlayerName, const QString& blackFiguresPlayerName)
{
    movesHistoryEdit = new QTextEdit;
    movesHistoryEdit->document()->setDocumentMargin(0);
    movesHistoryEdit->setLineWrapMode(QTextEdit::NoWrap);
    movesHistoryEdit->setDocumentTitle("Moves history");
    movesHistoryEdit->setReadOnly(true);
    movesHistoryEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    movesHistoryEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QTextTableFormat movesTableEditFormat;
    movesTableEditFormat.setBorderCollapse(true);
    movesTableEditFormat.setCellPadding(4);
    movesTableEditFormat.setBorderBrush(QColorConstants::Svg::blanchedalmond);
    movesTableEditFormat.setAlignment(Qt::AlignHCenter);

    QTextCursor cursor(movesHistoryEdit->textCursor());
    cursor.movePosition(QTextCursor::Start);
    movesHistoryTable = cursor.insertTable(1, 3, movesTableEditFormat);

    QTextCharFormat cellFormat(cursor.charFormat());
    cellFormat.setFont(QFont(qApp->font().family(), 10, QFont::Bold));
    cellFormat.setBackground(QColorConstants::Svg::darkkhaki);

    const QChar numberSymb(8470);
    addTextToTable(0, 0, numberSymb, cellFormat);
    cellFormat.setForeground(QBrush(QColorConstants::White));
    addTextToTable(0, 1, whiteFiguresPlayerName + " moves", cellFormat);
    cellFormat.setForeground(QBrush(QColorConstants::Black));
    addTextToTable(0, 2, blackFiguresPlayerName + " moves", cellFormat);

    movesHistoryEdit->document()->adjustSize();
    movesHistoryEdit->setFixedWidth(movesHistoryEdit->document()->size().width() + 4);

    auto palette = movesHistoryEdit->palette();
    palette.setColor(QPalette::Base, QColorConstants::Svg::darkgoldenrod);
    palette.setColor(QPalette::Light, QColorConstants::Svg::white);
    palette.setColor(QPalette::Dark, QColorConstants::Svg::brown);
    movesHistoryEdit->setPalette(palette);

    movesHistoryEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    movesHistoryEdit->setFrameStyle(QTextEdit::Box | QTextEdit::Raised);
}

void MainWindow::addToTextEdit(const QString& move)
{
    if (move == QString())
        return;

    undoAct->setEnabled(true);
    redoAct->setEnabled(false);
    qApp->installEventFilter(eventFilter);

    auto lastRowNumber = movesHistoryTable->rows() - 1;
    auto cursor = movesHistoryTable->firstCursorPosition();

    QTextCharFormat cellFormat(cursor.charFormat());
    cellFormat.setFont(QFont(qApp->font().family(), 10, QFont::Normal));
    auto secondPlayerMoveCell = movesHistoryTable->cellAt(lastRowNumber, 2);

    cursor.beginEditBlock();
    if (secondPlayerMoveCell.isValid() && secondPlayerMoveCell.firstCursorPosition() != secondPlayerMoveCell.lastCursorPosition())
    {
        movesHistoryTable->appendRows(1);
        ++lastRowNumber;
        addTextToTable(lastRowNumber, 0, QString::number(lastRowNumber), cellFormat);
        addTextToTable(lastRowNumber, 1, move, cellFormat);
    }
    else
    {
        addTextToTable(lastRowNumber, 2, move, cellFormat);
    }
    cursor.endEditBlock();
}

void MainWindow::addTextToTable(int row, int column, const QString& text, const QTextCharFormat& format)
{
    auto tableCursor = movesHistoryTable->firstCursorPosition();
    tableCursor.beginEditBlock();
    auto cell = movesHistoryTable->cellAt(row, column);
    cell.setFormat(format);
    auto cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(text);
    tableCursor.endEditBlock();
    changeMovesHistoryEditSize();
}

void MainWindow::changeMovesHistoryEditSize()
{
    movesHistoryEdit->document()->adjustSize();
    if (offsetX == 4 && movesHistoryEdit->size().height() < movesHistoryEdit->document()->size().height())
    {
        movesHistoryEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        offsetX += movesHistoryEdit->verticalScrollBar()->width();
        movesHistoryEdit->setFixedWidth(movesHistoryEdit->document()->size().width() + offsetX);
    }
    else if (offsetX == movesHistoryEdit->verticalScrollBar()->width() + 4 && movesHistoryEdit->size().height() > movesHistoryEdit->document()->size().height())
    {
        movesHistoryEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        offsetX = 4;
        movesHistoryEdit->setFixedWidth(movesHistoryEdit->document()->size().width() + offsetX);
    }
    else if (movesHistoryEdit->size().width() != movesHistoryEdit->document()->size().width() + offsetX)
    {
        movesHistoryEdit->setFixedWidth(movesHistoryEdit->document()->size().width() + offsetX);
    }
}

bool MainWindow::save()
{
    qApp->removeEventFilter(eventFilter);
    return chBController->writeMoves(movesHistoryTable);
}

bool MainWindow::saveAs()
{
    auto fileName = QFileDialog::getSaveFileName(this, "Save chess game", gameArchiveDirectory, "Chess game file (*.txt)");
    return chBController->writeMoves(movesHistoryTable, fileName.toStdString());
}

bool MainWindow::open()
{
    auto fileName = QFileDialog::getOpenFileName(this, "Open chess game", gameArchiveDirectory, "Chess game file (*.txt)");
    if (fileName != QString())
    {
        clear();
        return chBController->readMoves(fileName.toStdString());
    }
    return false;
}

void MainWindow::undo()
{
    chBController->undoMovement();
    qApp->installEventFilter(eventFilter);
    redoAct->setEnabled(true);
    if (!chBController->isPreviousMove())
    {
        undoAct->setEnabled(false);
    }
    changeMovesHistoryEditSize();
}

void MainWindow::redo()
{
    chBController->redoMovement();
    qApp->installEventFilter(eventFilter);
    undoAct->setEnabled(true);
    if (!chBController->isNextMove())
    {
        redoAct->setEnabled(false);
    }
    changeMovesHistoryEditSize();
}

void MainWindow::clear()
{
    undoAct->setEnabled(false);
    redoAct->setEnabled(false);
    chBController->clearView();
    changeMovesHistoryEditSize();
}

ChessBoardController* MainWindow::getChessboardController() const
{
    return chBController;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    changeMovesHistoryEditSize();
}