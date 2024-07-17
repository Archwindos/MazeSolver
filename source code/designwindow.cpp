#include "designwindow.h"
#include "ui_designwindow.h"
#include "inputmapsize.h"

DesignWindow::DesignWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DesignWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1400, 800);
    ui->frameMapDesign->installEventFilter(this);

    //绘制背景贴图
    QPalette PAllbackground = this->palette();
    QImage ImgAllbackground(":/back1.jpg");
    QImage fitimgpic=ImgAllbackground.scaled(this->width(),this->height(), Qt::IgnoreAspectRatio);
    PAllbackground.setBrush(QPalette::Window, QBrush(fitimgpic));
    this->setPalette(PAllbackground);
    updateMap = new QTimer(this);
    connect(updateMap, SIGNAL(timeout()), this, SLOT(update()));

    maxLength = 980;
    maxHeight = 740;
    colorWall  = QColor(qRgba(0,0,0,1.0));
    colorWay = QColor(qRgba(255,255,255,0));
    colorStart = QColor(qRgba(30,144,255, 1.0));
    colorEnd = QColor(qRgba(127,255,0,1.0));
    colorSearched = QColor(qRgba(230,206,172, 1.0));

    map = NULL;
    initDesigner();
}

DesignWindow::~DesignWindow()
{
    delete ui;
}

void DesignWindow::on_pushButtonBack_clicked()
{
    initDesigner();
    emit(DesignReturn());
}

void DesignWindow::initDesigner()
{
    sizeLength = 0;
    sizeHeight = 0;
    sizeSide = 0;
    mazeLT = QPoint(0, 0);
    sizeBlock = 0;
    shiftBlock = 0;
    radius = 0;
    lineWidth = 0;
    clickPosRow = -2;
    clickPosCol = -2;
    filePath = "";
    flagMousePressed = false;

    if(map != NULL){
        delete map;
    }
    map = NULL;

    ui->radioButtonWall->setChecked(true);
    curType = '#';
    ui->frameMapDesign->setMouseTracking(false);
    updateMap->stop();
    ui->groupBox->setEnabled(false);
    ui->pushButtonClearMap->setEnabled(false);
    ui->pushButtonSaveMap->setEnabled(false);
    ui->pushButtonChooseFile->setEnabled(true);
    ui->pushButtonNewMap->setEnabled(true);

    update();
}

bool DesignWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->frameMapDesign && event->type() == QEvent::Paint)
    {
        paintMap(); //响应函数
    }
    if(watched == ui->frameMapDesign && event->type() == QEvent::MouseMove)
    {
        moveCoordinate(static_cast<QMouseEvent *>(event)); //响应函数
        if(flagMousePressed == true) releaseEdit(static_cast<QMouseEvent *>(event));
    }
    if(watched == ui->frameMapDesign && event->type() == QEvent::MouseButtonPress){
        flagMousePressed = true;
        releaseEdit(static_cast<QMouseEvent *>(event));
    }
    if(watched == ui->frameMapDesign && event->type() == QEvent::MouseButtonRelease){
        flagMousePressed = false;
    }
    return QWidget::eventFilter(watched,event);
}

void DesignWindow::calculateLattice()
{
    int sizeSideL = maxLength / sizeLength;
    int sizeSideH = maxHeight / sizeHeight;
    if(sizeSideL >= sizeSideH) sizeSide = sizeSideH;
    else sizeSide = sizeSideL;

    int paddingL = (1000 - sizeSide * sizeLength)/2;
    int paddingH = (760 - sizeSide * sizeHeight)/2;

    mazeLT.setX(paddingL);
    mazeLT.setY(paddingH);

    lineWidth = sizeSide / 15 <= 1 ? 1 : sizeSide / 15;
    shiftBlock = sizeSide / 15 <= 1 ? 1 : sizeSide / 15;
    radius = shiftBlock * 2;
    sizeBlock = sizeSide - lineWidth - shiftBlock;

    ui->frameMapDesign->setMouseTracking(true);
    updateMap->start(40);
    ui->groupBox->setEnabled(true);
    ui->pushButtonClearMap->setEnabled(true);
    ui->pushButtonSaveMap->setEnabled(true);
    ui->pushButtonChooseFile->setEnabled(false);
    ui->pushButtonNewMap->setEnabled(false);
}

void DesignWindow::paintMap()
{
    if(map != NULL){
        QPainter painter(ui->frameMapDesign);
        // 绘制网格
        painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        QPen pen(QColor(qRgba(100,100,100,1.0)) ,lineWidth);
        painter.setPen(pen);

        int length = sizeLength * sizeSide;
        int height = sizeHeight * sizeSide;
        for (int r = 0; r <= sizeHeight; r++)//n个格子，n+1条线
            painter.drawLine(mazeLT.x(), mazeLT.y() + r*sizeSide, mazeLT.x()+length, mazeLT.y() + r*sizeSide);//横着

        for (int c = 0; c <= sizeLength; c++)
            painter.drawLine(mazeLT.x() + c*sizeSide, mazeLT.y(), mazeLT.x() + c*sizeSide, mazeLT.y() + height);//竖着

        painter.setPen(QPen(QColor(qRgba(255,255,255,0)) ,0));
        //绘制格子类型
        for (int r = 0; r < sizeHeight; r++){
            for (int c = 0; c < sizeLength; c++)
            {
                if (map[r][c] == '#'){
                    brush.setColor(colorWall);
                }
                else if (map[r][c] == '0'){
                    brush.setColor(colorWay);
                }else if (map[r][c] == 's'){
                    brush.setColor(colorStart);
                }else{
                    brush.setColor(colorEnd);
                }
                painter.setBrush(brush);
                painter.setOpacity(1.00);
                painter.drawRoundedRect(mazeLT.x()+ c*sizeSide + shiftBlock, mazeLT.y() + r*sizeSide + shiftBlock, sizeBlock, sizeBlock, radius, radius);
            }
        }

        // 绘制虚化的鼠标
        if(clickPosCol > -1){
            if(clickPosCol == 0 || clickPosCol == sizeLength - 1 || clickPosRow == 0 || clickPosRow == sizeHeight-1){
                if(curType != '#'){
                    painter.eraseRect(mazeLT.x()+ clickPosCol*sizeSide + shiftBlock, mazeLT.y() + clickPosRow*sizeSide + shiftBlock, sizeBlock, sizeBlock);
                    if(curType == '0') brush.setColor(qRgba(220,220,220, 1.0));
                    else if(curType == 's') brush.setColor(colorStart);
                    else  brush.setColor(colorEnd);
                    painter.setOpacity(0.5);
                    painter.setBrush(brush);
                    painter.drawRoundedRect(mazeLT.x() + clickPosCol*sizeSide + shiftBlock, mazeLT.y() + clickPosRow*sizeSide + shiftBlock, sizeBlock, sizeBlock, radius, radius);
                }
            }else{
                if(curType == '0' || curType == '#'){
                    painter.eraseRect(mazeLT.x()+ clickPosCol*sizeSide + shiftBlock, mazeLT.y() + clickPosRow*sizeSide + shiftBlock, sizeBlock, sizeBlock);
                    if(curType == '0') brush.setColor(qRgba(220,220,220, 1.0));
                    else brush.setColor(colorWall);
                    painter.setOpacity(0.5);
                    painter.setBrush(brush);
                    painter.drawRoundedRect(mazeLT.x() + clickPosCol*sizeSide + shiftBlock, mazeLT.y() + clickPosRow*sizeSide + shiftBlock, sizeBlock, sizeBlock, radius, radius);
                }
            }
        }
    }
}

void DesignWindow::moveCoordinate(QMouseEvent *event)
{
    // // 均以相对于地图起始位置mazeLT计算
    // // 鼠标当前的坐标
    int x = event->pos().x()-mazeLT.x();
    int y = event->pos().y()-mazeLT.y();

    clickPosRow = -2;
    clickPosCol = -2;
    // 获取最近的左上角的点索引
    int row = y / sizeSide;
    int col = x / sizeSide;
    // 左上角的点坐标
    if(row < sizeHeight && row >= 0 && col < sizeLength && col >= 0){
        clickPosRow = row;
        clickPosCol = col;
    }else{
        flagMousePressed = false;
    }

    // 存了坐标后也要重绘
    //update();
}

void DesignWindow::releaseEdit(QMouseEvent *event)
{
    if(clickPosCol == 0 || clickPosCol == sizeLength-1 || clickPosRow == 0 || clickPosRow == sizeHeight-1){
        if(curType == '0'){
            map[clickPosRow][clickPosCol] = '#';
        }else if(curType == 's'){
            map[clickPosRow][clickPosCol] = curType;
        }else if(curType == 'e'){
            map[clickPosRow][clickPosCol] = curType;
        }
    }else{
        if(curType == '0' || curType == '#'){
            map[clickPosRow][clickPosCol] = curType;
        }
    }

    //update();
}

//载入地图
void DesignWindow::on_pushButtonChooseFile_clicked()
{
    loadMap();
}

void DesignWindow::loadMap()
{
    filePath = QFileDialog::getOpenFileName(this,
                                            tr("选择地图文件"),
                                            "../",
                                            tr("files(*.txt)")
                                            );
    if(filePath == "") return;
    if(readMap(map, sizeHeight, sizeLength, filePath.toStdString()) == -1){
        QMessageBox::warning(this,tr("错误"),tr("载入的txt不是地图文件！"),QMessageBox::Ok);
        return;
    }
    calculateLattice();
}

//创建新地图
void DesignWindow::on_pushButtonNewMap_clicked()
{
    newMap();
}

void DesignWindow::newMap()
{
    inputMapSize* inputPage = new inputMapSize();
    inputPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(inputPage,SIGNAL(InputReturn(int , int )),this,SLOT(InputToDesignSlot(int , int )));
    inputPage->setModal(true);
    inputPage->show();
}

void DesignWindow::InputToDesignSlot(int l, int h)
{
    if(l == -1) return;//创建取消

    sizeLength = l;
    sizeHeight = h;
    map = new char*[sizeHeight];
    for(int i = 0; i < sizeHeight; i++){
        map[i] = new char[sizeLength + 1];
        map[i][0] = map[i][sizeLength-1] = '#';
    }
    for(int j = 0; j < sizeLength; j++){
        map[0][j] = map[sizeHeight - 1][j] = '#';
    }
    for(int i = 1; i < sizeHeight - 1; i++){
        for(int j = 1; j < sizeLength - 1; j++){
            map[i][j] = '0';
        }
    }
    calculateLattice();
}

//保存地图
void DesignWindow::on_pushButtonSaveMap_clicked()
{
    saveMapDes();
}

void DesignWindow::saveMapDes()
{
    if(checkMap() == -1){
        QMessageBox::warning(this,tr("警告"),tr("地图出入口有且只能有一个"),QMessageBox::Ok);
        return;
    }
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    tr("保存地图"),
                                                    "../",
                                                    tr("files(*.txt)"));
    if(savePath == "") return;
    if(saveMap(map, sizeHeight, sizeLength, savePath.toStdString()) == -1){
        QMessageBox::warning(this,tr("错误"),tr("地图保存失败！"),QMessageBox::Ok);
        return;
    }
}

int DesignWindow::checkMap()
{
    int sFlag = 0;
    int eFlag = 0;
    for(int i = 0; i < sizeHeight; i++){
        if(map[i][0] == 's') sFlag++;
        if(map[i][sizeLength-1] == 's') sFlag++;
        if(map[i][0] == 'e') eFlag++;
        if(map[i][sizeLength-1] == 'e') eFlag++;
    }
    for(int j = 0; j < sizeLength; j++){
        if(map[0][j] == 's') sFlag++;
        if(map[sizeHeight - 1][j] == 's') sFlag++;
        if(map[0][j] == 'e') eFlag++;
        if(map[sizeHeight - 1][j] == 'e') eFlag++;
    }
    if(sFlag == 1 && eFlag == 1)
        return 1;
    else
        return -1;
}

//清空地图
void DesignWindow::on_pushButtonClearMap_clicked()
{
    initDesigner();
}

//单选按钮
void DesignWindow::on_radioButtonWall_clicked()
{
    curType = '#';
}

void DesignWindow::on_radioButtonRemoveWall_clicked()
{
    curType = '0';
}

void DesignWindow::on_radioButtonStart_clicked()
{
    curType = 's';
}

void DesignWindow::on_radioButtonEnd_clicked()
{
    curType = 'e';
}
