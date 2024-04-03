#include "View/MenuWidget.h"
#include "Controller/ChessBoardController.h"

#include <QApplication>
#include <QPalette>
#include <QFont>
#include <QStyleFactory>
#include <QFontDatabase>

#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

void setStyle();
void setPalette();
void setFont();

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    setStyle();
    setPalette();
    setFont();

    app.setWindowIcon(QIcon((iconsFolderName + "app-icon_.png").c_str()));
    app.setApplicationName("Chess");
    app.setApplicationVersion("1.0");

    MenuWidget menu;
    menu.showMaximized();

    _CrtDumpMemoryLeaks();

    return app.exec();
}

void setStyle()
{
    if (QStyleFactory::keys().contains("Fusion", Qt::CaseInsensitive))
    {
        QApplication::setStyle(QStyleFactory::create("Fusion"));
    }
}

void setPalette()
{
    QPalette palette = qApp->palette();
    palette.setColor(QPalette::Window, QColorConstants::Svg::goldenrod);
    palette.setColor(QPalette::WindowText, QColorConstants::Svg::ghostwhite);
    palette.setColor(QPalette::Active, QPalette::Button, QColorConstants::Svg::olivedrab);
    palette.setColor(QPalette::Active, QPalette::ButtonText, QColorConstants::Svg::ghostwhite);
    qApp->setPalette(palette);
}

void setFont()
{
    QFont font = qApp->font();
    if (!QFontDatabase::isSmoothlyScalable(font.family()))
    {
        font = QFont("Arial");
        font.setStyleStrategy(QFont::PreferOutline);
    }
    qApp->setFont(font);
    qApp->setFont(QFont(font.family(), 16), "QAbstractButton");
    qApp->setFont(QFont(font.family(), 24, QFont::Bold), "QGroupBox");
}
