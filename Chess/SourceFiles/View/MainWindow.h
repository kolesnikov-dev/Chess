#pragma once

#include "../Controller/ChessBoardController.h"
#include "ChessBoardView.h"
#include "QuitEventFilter.h"

#include <QMainWindow>
#include <QMenu>
#include <QTextEdit>
#include <QAction>
#include <QResizeEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& firstPlayerName = "Player1", const QString& secondPlayerName = "Player2", const Color firstPlayerColor = Color::White, const Color secondPlayerColor = Color::Black);
    ~MainWindow();

signals:
    void showApplicationMenu();

public slots:
    void clear();

private slots:
    bool save();
    bool saveAs();
    bool open();

    void undo();
    void redo();

    void addToTextEdit(const QString& move);

public:
    ChessBoardController* getChessboardController() const;

    void resizeEvent(QResizeEvent* event) override;

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createMovesHistoryEdit(const QString& whiteFiguresPlayerName, const QString& blackFiguresPlayerName);

    void addTextToTable(int row, int column, const QString& text, const QTextCharFormat& format = QTextCharFormat());
    void changeMovesHistoryEditSize();

private:
    QMenu* fileMenu;
    QMenu* saveMenu;
    QMenu* generalMenu;
    QMenu* editMenu;

    QAction* saveAct;
    QAction* saveAsAct;
    QAction* openAct;

    QAction* undoAct;
    QAction* redoAct;
    QAction* clearAct;

    QAction* moveTipsAct;
    QAction* lastMoveTipsAct;
    QAction* backToMenuAct;
    QAction* exitAct;

private:
    int offsetX = 4;
    const QString QIconsFolderName;
    ChessBoardController* chBController;
    ChessBoardView* chBView;

    QTextTable* movesHistoryTable;
    QTextEdit* movesHistoryEdit;

    EventFilter* eventFilter;
};
