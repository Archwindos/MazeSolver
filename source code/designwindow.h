#ifndef DESIGNWINDOW_H
#define DESIGNWINDOW_H

#include <QWidget>
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

#include "fileprocess.h"

namespace Ui {
class DesignWindow;
}

class DesignWindow : public QWidget
{
    Q_OBJECT

protected:
    //初始化界面
    void initDesigner();
    // 绘制
    void paintMap();
    //鼠标移动
    void moveCoordinate(QMouseEvent *event);
    //鼠标释放，修改map
    void releaseEdit(QMouseEvent *event);
    //子控件事件过滤器
    bool eventFilter(QObject *watched, QEvent *event);
    //计算网格显示参数
    void calculateLattice();
    //导入地图文件
    void loadMap();
    //创建地图
    void newMap();
    //保存地图
    void saveMapDes();
    //检查地图正确性
    int checkMap();

    QTimer* updateMap;

    int sizeLength;
    int sizeHeight;

    int sizeSide;//格子边长
    int sizeBlock;//格子中填充矩形的边长
    int shiftBlock;
    int radius;
    QPoint mazeLT;//左上点
    int lineWidth;

    int clickPosRow;
    int clickPosCol;

    char curType;
    char** map;
    QString filePath;

    bool flagMousePressed;

    int maxLength;
    int maxHeight;
    QColor colorWall;
    QColor colorWay;
    QColor colorStart;
    QColor colorEnd;
    QColor colorSearched;

public:
    explicit DesignWindow(QWidget *parent = nullptr);
    ~DesignWindow();

signals:
    void DesignReturn();

private slots:
    void InputToDesignSlot(int l, int h);

    void on_pushButtonBack_clicked();

    void on_pushButtonChooseFile_clicked();

    void on_pushButtonNewMap_clicked();

    void on_pushButtonSaveMap_clicked();

    void on_pushButtonClearMap_clicked();

    void on_radioButtonWall_clicked();

    void on_radioButtonRemoveWall_clicked();

    void on_radioButtonStart_clicked();

    void on_radioButtonEnd_clicked();

private:
    Ui::DesignWindow *ui;
};

#endif // DESIGNWINDOW_H
