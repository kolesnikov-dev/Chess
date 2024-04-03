#include <QObject>
#include <QDialog>
#include <QEvent>

class EventFilter : public QObject
{
    Q_OBJECT

public:
    EventFilter(QObject* parent = nullptr);

signals:
    void needToSave();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QDialog* quitDialog;
};

