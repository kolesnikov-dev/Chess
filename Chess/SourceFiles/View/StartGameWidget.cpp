#include "StartGameWidget.h"

#include "MenuWidget.h"

#include <QApplication>
#include <QGroupBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>

StartGameWidget::StartGameWidget(QWidget* parent)
{
    QGroupBox* groupBox = new QGroupBox(this);
    groupBox->setFont(QFont(qApp->font().family(), 14, QFont::Bold));
    groupBox->setAlignment(Qt::AlignCenter);
    groupBox->setTitle("Choose players names and their figures color");

    QLineEdit* firstPlayerName = new QLineEdit();
    QLineEdit* secondPlayerName = new QLineEdit();
    firstPlayerName->setFont(QFont(qApp->font().family(), 10));
    secondPlayerName->setFont(QFont(qApp->font().family(), 10));
    firstPlayerName->setText("Player1");
    secondPlayerName->setText("Player2");
    firstPlayerName->setPlaceholderText("Enter 1st player name");
    secondPlayerName->setPlaceholderText("Enter 2nd player name");
    firstPlayerName->setAlignment(Qt::AlignCenter);
    secondPlayerName->setAlignment(Qt::AlignCenter);
    auto nameValidator = getNameValidator();
    firstPlayerName->setValidator(&nameValidator);
    secondPlayerName->setValidator(&nameValidator);

    QPushButton* startGameButton = new QPushButton("Start game");
    QPushButton* backButton = new QPushButton("Back");
    backButton->setMinimumWidth(startGameButton->minimumSizeHint().width());

    auto blockStartGameButton = [this, startGameButton, firstPlayerName, secondPlayerName](const QString& text)
        {
            if (firstPlayerName->text().size() >= nameMinLength && secondPlayerName->text().size() >= nameMinLength)
                startGameButton->setDisabled(false);
            else
                startGameButton->setDisabled(true);
        };
    connect(firstPlayerName, &QLineEdit::textChanged, blockStartGameButton);
    connect(secondPlayerName, &QLineEdit::textChanged, blockStartGameButton);

    QCheckBox* firstPlayerBlackCheckbox = new QCheckBox();
    QCheckBox* firstPlayerWhiteCheckbox = new QCheckBox();
    QCheckBox* secondPlayerBlackCheckbox = new QCheckBox();
    QCheckBox* secondPlayerWhiteCheckbox = new QCheckBox();
    firstPlayerWhiteCheckbox->setChecked(true);
    secondPlayerBlackCheckbox->setChecked(true);

    QButtonGroup* firstButtonGroup = new QButtonGroup(this);
    QButtonGroup* secondButtonGroup = new QButtonGroup(this);
    firstButtonGroup->addButton(firstPlayerBlackCheckbox, Color::Black);
    firstButtonGroup->addButton(firstPlayerWhiteCheckbox, Color::White);
    secondButtonGroup->addButton(secondPlayerBlackCheckbox, Color::Black);
    secondButtonGroup->addButton(secondPlayerWhiteCheckbox, Color::White);
    connectButtons(firstPlayerBlackCheckbox, secondPlayerWhiteCheckbox);
    connectButtons(secondPlayerBlackCheckbox, firstPlayerWhiteCheckbox);
    connectButtons(firstPlayerWhiteCheckbox, secondPlayerBlackCheckbox);
    connectButtons(secondPlayerWhiteCheckbox, firstPlayerBlackCheckbox);

    connect(startGameButton, &QPushButton::clicked, [=]() {
        MainWindow* mainWindow = new MainWindow(firstPlayerName->text(), secondPlayerName->text(), static_cast<Color>(firstButtonGroup->checkedId()), static_cast<Color>(secondButtonGroup->checkedId()));
        mainWindow->showMaximized();
        this->hide();
        emit mainWindowCreated(mainWindow);
        });

    connect(backButton, &QPushButton::clicked, qobject_cast<MenuWidget*>(parent), &MenuWidget::show);
    connect(backButton, &QPushButton::clicked, this, &StartGameWidget::hide);

    QPixmap whitePixmap(QString(figureImagesPath) + "white.jpg");
    QPixmap blackPixmap(QString(figureImagesPath) + "black.jpg");
    whitePixmap = whitePixmap.scaled(40, 40);
    blackPixmap = blackPixmap.scaled(40, 40);
    QLabel* firstPlayerWhiteLabel = new QLabel();
    QLabel* firstPlayerBlackLabel = new QLabel();
    QLabel* secondPlayerWhiteLabel = new QLabel();
    QLabel* secondPlayerBlackLabel = new QLabel();
    firstPlayerWhiteLabel->setPixmap(whitePixmap);
    firstPlayerBlackLabel->setPixmap(blackPixmap);
    secondPlayerWhiteLabel->setPixmap(whitePixmap);
    secondPlayerBlackLabel->setPixmap(blackPixmap);

    QGridLayout* boxGridLayout = new QGridLayout(this);
    boxGridLayout->setVerticalSpacing(boxGridLayout->verticalSpacing() + boxGridLayout->verticalSpacing() / 2);
    boxGridLayout->setRowMinimumHeight(6, startGameButton->sizeHint().height() * 2);
    boxGridLayout->addWidget(firstPlayerName, 0, 0, 1, 2, Qt::AlignCenter);
    boxGridLayout->addWidget(firstPlayerWhiteLabel, 1, 0, Qt::AlignCenter);
    boxGridLayout->addWidget(firstPlayerBlackLabel, 1, 1, Qt::AlignCenter);
    boxGridLayout->addWidget(firstPlayerWhiteCheckbox, 2, 0, Qt::AlignCenter);
    boxGridLayout->addWidget(firstPlayerBlackCheckbox, 2, 1, Qt::AlignCenter);
    boxGridLayout->addWidget(secondPlayerName, 3, 0, 1, 2, Qt::AlignCenter);
    boxGridLayout->addWidget(secondPlayerWhiteLabel, 4, 0, Qt::AlignCenter);
    boxGridLayout->addWidget(secondPlayerBlackLabel, 4, 1, Qt::AlignCenter);
    boxGridLayout->addWidget(secondPlayerWhiteCheckbox, 5, 0, Qt::AlignHCenter);
    boxGridLayout->addWidget(secondPlayerBlackCheckbox, 5, 1, Qt::AlignHCenter);
    boxGridLayout->addWidget(backButton, 6, 0, Qt::AlignCenter | Qt::AlignBottom);
    boxGridLayout->addWidget(startGameButton, 6, 1, Qt::AlignCenter | Qt::AlignBottom);
    groupBox->setLayout(boxGridLayout);

    QGridLayout* globalGridLayout = new QGridLayout(this);
    globalGridLayout->addWidget(groupBox, 1, 1);
    globalGridLayout->setColumnStretch(0, 1);
    globalGridLayout->setColumnStretch(2, 1);
    globalGridLayout->setRowStretch(0, 1);
    globalGridLayout->setRowStretch(2, 1);
    setLayout(globalGridLayout);
}

void StartGameWidget::connectButtons(QCheckBox* clickedButton, QCheckBox* connectedButton) const
{
    connect(clickedButton, &QCheckBox::toggled, [connectedButton](bool checked)
        {
            connectedButton->blockSignals(true);
            if (checked)
                connectedButton->setChecked(checked);
            connectedButton->blockSignals(false);
        });
}

QRegularExpressionValidator StartGameWidget::getNameValidator() const
{
    QRegularExpression regex;
    QString regexString("\\S{");
    regexString += QString::number(nameMinLength);
    regexString += ',';
    regexString += QString::number(nameMaxLength);
    regexString += '}';
    return QRegularExpressionValidator(QRegularExpression(regexString));
}