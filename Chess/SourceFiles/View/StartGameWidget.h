#pragma once

#include "MainWindow.h"

#include <QWidget>
#include <QString>
#include <QCheckBox>
#include <QRegularExpressionValidator>

class StartGameWidget : public QWidget
{
    Q_OBJECT

public:
    StartGameWidget(QWidget* parent = nullptr);

signals:
    void mainWindowCreated(MainWindow* mainWindow);

private:
    void connectButtons(QCheckBox* clickedButton, QCheckBox* connectedButton) const;
    QRegularExpressionValidator getNameValidator() const;

private:
    const int nameMaxLength = 10, nameMinLength = 3;
};