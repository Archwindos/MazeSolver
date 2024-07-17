#ifndef INPUTMAPSIZE_H
#define INPUTMAPSIZE_H

#include <QDialog>

namespace Ui {
class inputMapSize;
}

class inputMapSize : public QDialog
{
    Q_OBJECT

private:
    bool flag;

public:
    explicit inputMapSize(QWidget *parent = nullptr);
    ~inputMapSize();

signals:
    void InputReturn(int l, int h);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::inputMapSize *ui;
};

#endif // INPUTMAPSIZE_H
