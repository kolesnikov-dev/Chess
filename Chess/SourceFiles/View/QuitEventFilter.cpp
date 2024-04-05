#include "QuitEventFilter.h"

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

EventFilter::EventFilter(QObject* parent) : QObject(parent), quitDialog(nullptr) {}

bool EventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Quit)
    {
        if (!quitDialog)
        {
            quitDialog = new QDialog;
            quitDialog->setModal(true);

            QPushButton* yesButton = new QPushButton("Yes");
            QPushButton* noButton = new QPushButton("No");
            noButton->setMinimumSize(yesButton->minimumSizeHint());
            connect(noButton, &QPushButton::clicked, quitDialog, &QDialog::reject);
            connect(yesButton, &QPushButton::clicked, quitDialog, &QDialog::accept);

            QLabel* messageLabel = new QLabel("Do you want to save game before exit?");
            messageLabel->setFont(QFont(qApp->font().family(), 16));
            messageLabel->setAlignment(Qt::AlignCenter);

            QGridLayout* popupLayout = new QGridLayout;
            popupLayout->addWidget(messageLabel, 0, 0, 1, 2, Qt::AlignHCenter);
            popupLayout->addWidget(yesButton, 1, 0);
            popupLayout->addWidget(noButton, 1, 1);
            quitDialog->setLayout(popupLayout);
        }
        auto returnValue = quitDialog->exec();
        if (quitDialog)
            delete quitDialog;
        if (returnValue == QDialog::Accepted)
        {
            emit needToSave();
        }

    }
    return QObject::eventFilter(obj, event);
}

