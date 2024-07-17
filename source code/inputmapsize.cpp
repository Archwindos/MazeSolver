#include "inputmapsize.h"
#include "ui_inputmapsize.h"

inputMapSize::inputMapSize(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::inputMapSize)
{
    ui->setupUi(this);
    flag = false;
}

inputMapSize::~inputMapSize()
{
    if(flag == false) emit(InputReturn(-1, -1));
    delete ui;
}

void inputMapSize::on_buttonBox_accepted()
{
    int l = ui->spinBoxLength->value();
    int h = ui->spinBoxHeight->value();
    flag = true;
    emit(InputReturn(l, h));
    this->close();
}


void inputMapSize::on_buttonBox_rejected()
{
    this->close();
}

