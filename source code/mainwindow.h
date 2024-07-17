#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QAction>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include "maze.h"
#include "qrgb.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 绘制
    void paintMap();
    //在子控件上绘图
    bool eventFilter(QObject *watched, QEvent *event);
    //初始化界面
    void initMainPage();
    //创建迷宫类
    void createMaze();
    //执行搜索
    void search();
    //计算网格显示参数
    void calculateLattice();
    //终止搜索
    void stopSearch();
    //结果展示
    void showResult();


    Maze* mazeMain;//对于每一个地图，创建一次maze类
    char** map;//展示用
    char** inputMap;//中间介质

    int sizeLength;
    int sizeHeight;
    int sizeSide;//格子边长
    int sizeBlock;//格子中填充矩形的边长
    int shiftBlock;
    int radius;
    int radiusCircle;
    QPoint mazeLT;//左上点
    int lineWidth;
    QString filePath;
    searchAlgorithm curAlg;
    bool isSearching;
    int step;
    int assumedTotalStep;
    int speed;


    QTimer* searchStepTimer;
    QTimer* drawTimer;

    int maxLength;
    int maxHeight;
    QColor colorWall;
    QColor colorWay;
    QColor colorStart;
    QColor colorEnd;
    QColor colorSearched;
    QColor colorWayFind;

private slots:
    void DesignToMainSlot();

    void on_pushButtonDesigner_clicked();

    void on_pushButtonLoadMap_clicked();

    void on_pushButtonChooseFile_clicked();

    void on_pushButtonClearMap_clicked();

    void on_comboBoxChooseAlg_currentIndexChanged(int index);

    void on_pushButtonSearch_clicked();

    void on_pushButtonStop_clicked();

    void searchOneStep();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
