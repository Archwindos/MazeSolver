#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "designwindow.h"
#include "fileprocess.h"

int assumeRunTime = 5;
DesignWindow* designPage;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1400, 800);
    designPage = new DesignWindow();
    ui->frameMapMain->installEventFilter(this);
    connect(designPage,SIGNAL(DesignReturn()),this,SLOT(DesignToMainSlot()));
    searchStepTimer = new QTimer(this);
    connect(searchStepTimer, SIGNAL(timeout()), this, SLOT(searchOneStep()));
    drawTimer = new QTimer(this);
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(update()));

    //绘制背景贴图
    QPalette PAllbackground = this->palette();
    QImage ImgAllbackground(":/back2.jpg");
    QImage fitimgpic=ImgAllbackground.scaled(this->width(),this->height(), Qt::IgnoreAspectRatio);
    PAllbackground.setBrush(QPalette::Window, QBrush(fitimgpic));
    this->setPalette(PAllbackground);
    //设置进度条样式
    ui->progressBar->setStyleSheet("QProgressBar{text-align:center;background-color:#DDDDDD;border: 0px solid #DDDDDD;border-radius:5px;}"
                                     "QProgressBar::chunk{background-color:#e97c07;border-radius:5px; width:8px;margin:0.5px;}");
    ui->lcdNumberTime->setDecMode();
    ui->lcdNumberPath->setDecMode();
    ui->lineEditResult->setReadOnly(true);
    ui->lineEditResult->setAlignment(Qt::AlignCenter);

    maxLength = 980;
    maxHeight = 740;
    colorWall  = QColor(qRgba(0,0,0,1.0));
    colorWay = QColor(qRgba(255,255,255,0));
    colorStart = QColor(qRgba(30,144,255, 1.0));
    colorEnd = QColor(qRgba(127,255,0,1.0));
    colorSearched = QColor(qRgba(230,206,172, 1.0));
    colorWayFind = QColor(qRgba(255,182,193, 1.0));

    map = NULL;
    inputMap = NULL;
    mazeMain = NULL;
    initMainPage();
}

MainWindow::~MainWindow()
{
    initMainPage();
    delete ui;
    delete designPage;
    designPage = NULL;
}

void MainWindow::calculateLattice()
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
    radiusCircle = sizeBlock/2;

    assumedTotalStep = sizeLength * sizeHeight * 0.75;
    speed = 2000 * assumeRunTime / (sizeLength * sizeHeight);
    speed = speed > 150 ? 150 : speed;
    speed = speed < 1 ? 1 : speed;
}

void MainWindow::paintMap()
{
    if(map != NULL){
        QPainter painter(ui->frameMapMain);
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
                if(map[r][c] == '.' || map[r][c] == '@'){
                    if(map[r][c] == '.')
                        brush.setColor(colorSearched);
                    else
                        brush.setColor(colorWayFind);
                    painter.setBrush(brush);
                    painter.setOpacity(1.00);
                    painter.drawRoundedRect(mazeLT.x()+ c*sizeSide + shiftBlock, mazeLT.y() + r*sizeSide + shiftBlock, sizeBlock, sizeBlock, radiusCircle, radiusCircle);
                }else{
                    if (map[r][c] == '#'){
                        brush.setColor(colorWall);
                    }else if (map[r][c] == '0'){
                        brush.setColor(colorWay);
                    }else if (map[r][c] == 's'){
                        brush.setColor(colorStart);
                    }else {
                        brush.setColor(colorEnd);
                    }
                    painter.setBrush(brush);
                    painter.setOpacity(1.00);
                    painter.drawRoundedRect(mazeLT.x()+ c*sizeSide + shiftBlock, mazeLT.y() + r*sizeSide + shiftBlock, sizeBlock, sizeBlock, radius, radius);
                }

            }
        }

    }
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->frameMapMain && event->type() == QEvent::Paint)
    {
        paintMap(); //响应函数
    }
    return QWidget::eventFilter(watched,event);
}

void MainWindow::initMainPage()
{
    ui->lineEditFilePath_main->setEnabled(true);
    ui->pushButtonChooseFile->setEnabled(true);
    ui->pushButtonLoadMap->setEnabled(true);
    ui->pushButtonClearMap->setEnabled(false);
    ui->widget->setEnabled(false);
    ui->lineEditFilePath_main->clear();
    ui->progressBar->setValue(0);

    //类参数
    sizeLength = 0;
    sizeHeight = 0;
    //绘图参数
    sizeSide = 0;
    sizeBlock = 0;
    shiftBlock = 0;
    radius = 0;
    radiusCircle = 0;
    mazeLT = QPoint(0, 0);
    lineWidth = 0;
    assumedTotalStep = 0;
    speed = 0;

    filePath = "";
    ui->lcdNumberTime->display(QString::number(0));
    ui->lcdNumberPath->display(QString::number(0));

    curAlg = BFS;
    isSearching = false;
    step = 0;

    //删除地图
    if(map != NULL)
        delete map;
    map = NULL;
    if(inputMap != NULL){
        delete inputMap;
    }
    inputMap = NULL;
    //删除类
    if(mazeMain != NULL)
        delete mazeMain;
    mazeMain = NULL;

    update();
}

//初始化类
void MainWindow::createMaze()
{
    if(readMap(inputMap, sizeHeight, sizeLength, filePath.toStdString()) == -1){
        QMessageBox::warning(this,tr("错误"),tr("载入txt的不是地图文件！"),QMessageBox::Ok);
        return;
    }
    mazeMain = new Maze(sizeHeight, sizeLength);
    if(mazeMain->setMap(inputMap) == -1){
        QMessageBox::warning(this,tr("警告"),tr("所选地图存在问题！请在地图设计页面检查"),QMessageBox::Ok);
        return;
    }
    //拷贝地图
    if(map != NULL)
        delete map;
    map = NULL;
    map = new char*[sizeHeight];
    for(int i = 0; i < sizeHeight; i++){
        map[i] = new char[sizeLength + 1];
        for(int j = 0; j < sizeLength; j++){
            map[i][j] = inputMap[i][j];
        }
    }
    calculateLattice();

    ui->comboBoxChooseAlg->setCurrentIndex(0);//默认是BFS

    ui->lineEditFilePath_main->setEnabled(false);
    ui->pushButtonChooseFile->setEnabled(false);
    ui->pushButtonLoadMap->setEnabled(false);
    ui->pushButtonClearMap->setEnabled(true);

    ui->widget->setEnabled(true);
    ui->progressBar->setValue(0);
    ui->lineEditResult->setText("等待开始");
    ui->pushButtonStop->setEnabled(false);

    update();
}

void MainWindow::search()
{
    if(map != NULL)
        delete map;
    map = NULL;

    map = new char*[sizeHeight];
    for(int i = 0; i < sizeHeight; i++){
        map[i] = new char[sizeLength + 1];
        for(int j = 0; j < sizeLength; j++){
            map[i][j] = inputMap[i][j];
        }
    }

    isSearching = false;
    step = 0;

    mazeMain->clearSearch();
    mazeMain->setAlgorithm(curAlg);

    ui->progressBar->setValue(0);
    ui->lcdNumberTime->display(QString::number(0));
    ui->lcdNumberPath->display(QString::number(0));
    ui->lineEditResult->setText("搜索中...");

    searchStepTimer->start(speed);
    drawTimer->start(100);
}

void MainWindow::searchOneStep()
{
    queue<Point> newSearched;
    if(mazeMain->searchOneStep(newSearched) == false){
        //避免更改出口颜色
        if(mazeMain->isWayFind() == true){
            showResult();
            return;
        }
        while (newSearched.empty() == false) {
            map[newSearched.front().row][newSearched.front().col] = '.';
            newSearched.pop();
            step++;
        }
    }else{
        showResult();
        return;
    }

    ui->progressBar->setValue(min(step*100/assumedTotalStep, 99));
    return;
}

void MainWindow::showResult()
{
    searchStepTimer->stop();
    drawTimer->stop();

    ui->lineEditResult->setText("计算时间中");
    mazeMain->clearSearch();
    int fullSpeedSearchTime = mazeMain->search();
    ui->lcdNumberTime->display(QString::number(fullSpeedSearchTime));
    ui->progressBar->setValue(100);

    if(mazeMain->isWayFind() == true){
        ui->lineEditResult->setText("找到路径");
        stack<Point> wayFind;
        mazeMain->showWay(wayFind);
        //避免把出入口覆盖
        Point temp;
        wayFind.pop();

        int pathLength = 0;
        while(wayFind.size() != 1){
            temp = wayFind.top();
            wayFind.pop();
            map[temp.row][temp.col] = '@';
            pathLength++;
        }
        ui->lcdNumberPath->display(QString::number(pathLength));
    }else{
        ui->lineEditResult->setText("\"走投无路\"");
        ui->lcdNumberPath->display(QString::number(0));
    }

    ui->widget_2->setEnabled(true);
    ui->pushButtonDesigner->setEnabled(true);
    ui->pushButtonSearch->setEnabled(true);
    ui->comboBoxChooseAlg->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);

    update();
}

void MainWindow::stopSearch()
{
    searchStepTimer->stop();
    drawTimer->stop();
    ui->lineEditResult->setText("搜索终止");

    ui->widget_2->setEnabled(true);
    ui->pushButtonDesigner->setEnabled(true);
    ui->pushButtonSearch->setEnabled(true);
    ui->comboBoxChooseAlg->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
}

//设计界面
void MainWindow::DesignToMainSlot()
{
    designPage->close();
    this->show();
}

void MainWindow::on_pushButtonDesigner_clicked()
{
    this->close();
    designPage->show();
}

//加载地图
void MainWindow::on_pushButtonLoadMap_clicked()
{
    filePath = ui->lineEditFilePath_main->text();
    QFileInfo file(filePath);
    if(file.exists() == false){
        QMessageBox::warning(this,tr("错误"),tr("文件不存在！"),QMessageBox::Ok);
        return;
    }
    else if(file.isFile() == false){
        QMessageBox::warning(this,tr("错误"),tr("选中的不是文件！"),QMessageBox::Ok);
        return;
    }
    createMaze();
}

void MainWindow::on_pushButtonChooseFile_clicked()
{
    QString temp = QFileDialog::getOpenFileName(this,
                    tr("选择地图文件"),
                    "../",
                    tr("files(*.txt)")
    );
    ui->lineEditFilePath_main->setText(temp);
}


void MainWindow::on_pushButtonClearMap_clicked()
{
    initMainPage();
}


void MainWindow::on_comboBoxChooseAlg_currentIndexChanged(int index)
{
    if(index == 0){
        curAlg = BFS;
    }else if(index == 1){
        curAlg = DFS;
    }else if(index == 2){
        curAlg = DBFS;
    }
    else{
        curAlg = AST;
    }
}


void MainWindow::on_pushButtonSearch_clicked()
{
    ui->widget_2->setEnabled(false);
    ui->pushButtonDesigner->setEnabled(false);
    ui->pushButtonSearch->setEnabled(false);
    ui->comboBoxChooseAlg->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);

    search();
}


void MainWindow::on_pushButtonStop_clicked()
{
    stopSearch();
}
