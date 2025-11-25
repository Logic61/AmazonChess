#include "mainwindow.h"
//#include "./ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include "chessboard.h"
#include <QFileDialog>
#include <qapplication.h>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include "ai_thread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    const int height_of_button = 70;
    //ui->setupUi(this);
    //放界面的地方
    resize(1000,720);

    QWidget *central = new QWidget(this);
    setCentralWidget(central); //把central 设置成中央控件

    //水平布局 Horizontal
    QHBoxLayout *mainLayout = new QHBoxLayout();
    central->setLayout(mainLayout);

    //棋盘按钮
    ChessBoard *board = new ChessBoard();
    QWidget *boardFrame = new QWidget(this);
    boardFrame->setObjectName("boardFrame");  // 给 QSS 用
    QVBoxLayout *boardLayout = new QVBoxLayout(boardFrame);
    boardLayout->setContentsMargins(0, 0, 0, 0);
    boardLayout->addWidget(board);

    mainLayout->addWidget(boardFrame);


    //右边区域，vertical布局
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setObjectName("rightPanel"); // 给 QSS 用
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    mainLayout->addWidget(rightPanel);


    //回合
    QLabel *playerLabel = new QLabel("当前玩家：黑方");
    playerLabel->setAlignment(Qt::AlignCenter);
    playerLabel->setStyleSheet("color: blue; font-weight: bold; font-size: 18px;");

    connect(board, &ChessBoard::turnChanged, this, [=](int newTurn){
        if(newTurn == 1)
            playerLabel->setText("当前玩家：白方");
        else
            playerLabel->setText("当前玩家：黑方");
    });

    rightLayout->addWidget(playerLabel);

    //调左右比例
    mainLayout->setStretch(0,7);
    mainLayout->setStretch(1,2);

    // 按钮区域
    QGroupBox *box = new QGroupBox("");
    QVBoxLayout *boxLayout = new QVBoxLayout(box);
    //boardButton->setMinimumHeight(200);
    QPushButton *resetbtn = new QPushButton("开始新对局");
    QPushButton *savebtn = new QPushButton("存储当前棋盘");
    QPushButton *readbtn = new QPushButton("读取对局");
    QPushButton *undobtn = new QPushButton("悔棋");
    QPushButton *modebtn = new QPushButton("模式选择");
    QPushButton *exitbtn = new QPushButton("结束游戏");
    boxLayout->addWidget(modebtn);
    boxLayout->addWidget(resetbtn);
    boxLayout->addWidget(savebtn);
    boxLayout->addWidget(readbtn);
    boxLayout->addWidget(undobtn);
    boxLayout->addWidget(exitbtn);
    //按钮高度
    resetbtn->setFixedHeight((height_of_button));
    savebtn->setFixedHeight((height_of_button));
    readbtn->setFixedHeight((height_of_button));
    undobtn->setFixedHeight((height_of_button));
    modebtn->setFixedHeight((height_of_button));
    exitbtn->setFixedHeight((height_of_button));
    qApp->setStyleSheet(R"(
QPushButton {
    background-color: #4A90E2;
    color: white;
    font-size: 20px;
    border-radius: 25px;
    border: none;
}
QPushButton:hover {
    background-color: #5aa0f2;
}
QPushButton:pressed {
    background-color: #3575c4;
}
)");

    playerLabel->setObjectName("playerLabel");
    modebtn->setObjectName("modebtn");
    resetbtn->setObjectName("resetbtn");
    savebtn->setObjectName("savebtn");
    readbtn->setObjectName("readbtn");
    undobtn->setObjectName("undobtn");
    exitbtn->setObjectName("exitbtn");


    rightLayout->addWidget(box);

    //按钮实现
    connect(resetbtn, &QPushButton::clicked, board, &ChessBoard::reset);
    connect(modebtn, &QPushButton::clicked, board, &ChessBoard::modeSet);
    connect(undobtn, &QPushButton::clicked, board, &ChessBoard::undomove);
    connect(exitbtn, &QPushButton::clicked, board, &ChessBoard::exit);
    connect(readbtn, &QPushButton::clicked, this, [=](){
        QString defaultPath = QDir::currentPath() + "/save";
        QString filename = QFileDialog::getOpenFileName(
            this,
            "选择存档文件",
            defaultPath,
            ""
            );
        if (!filename.isEmpty()) {
            board->readGame(filename);
        }
    });
    connect(savebtn, &QPushButton::clicked, this, [=](){
        QString defaultPath = QDir::currentPath() + "/save";
        QString filename = QFileDialog::getOpenFileName(
            this,
            "选择存档文件",
            defaultPath,
            ""
            );
        if (!filename.isEmpty()) {
            board->saveGame(filename);
        }
    });
    this->setStyleSheet(R"(
/* 整体背景 */
QWidget {
background-color: #f3f4f7;
font-family: 'Microsoft YaHei UI';
}

/* 右侧卡片面板 */
QWidget#rightPanel {
background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
stop:0 #ffffff, stop:1 #f9f9f9);
border-radius: 22px;
padding: 25px;
border: 1px solid #e5e5e5;
}

/* 当前玩家标签 */
QLabel#playerLabel {
font-size: 22px;
font-weight: bold;
color: #2b59ff;
text-align: center;
margin-bottom: 15px;
}

/* 棋盘外框 */
QWidget#boardFrame {
border: 4px solid #2c2c2c;
border-radius: 12px;
background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
stop:0 #ffffff, stop:1 #e8e8e8);
}

/* 清新薄荷绿按钮 */
QPushButton {
background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
stop:0 #6FCF97, stop:1 #52B76A);
color: white;
font-size: 18px;
border-radius: 18px;
border: none;
padding: 8px 20px;
min-width: 120px;
}

/* 鼠标悬停 */
QPushButton:hover {
background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
stop:0 #8FE2B5, stop:1 #6FCF97);
}

/* 鼠标按下 */
QPushButton:pressed {
background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
stop:0 #4BAF7C, stop:1 #399361);
}

/* 去掉 QGroupBox 边框 */
QGroupBox {
border: none;
}

/* 所有 QLabel 默认样式 */
QLabel {
color: #333333;
font-size: 16px;
}

/* 滑动条和其他控件轻微圆角风格 */
QSlider::groove:horizontal {
height: 8px;
background: #e0e0e0;
border-radius: 4px;
}
QSlider::handle:horizontal {
width: 16px;
background: #6FCF97;
border-radius: 8px;
margin: -4px 0;
}
)");
    // 给右侧面板添加阴影
    QGraphicsDropShadowEffect *panelShadow = new QGraphicsDropShadowEffect();
    panelShadow->setBlurRadius(15);
    panelShadow->setOffset(0,4);
    panelShadow->setColor(QColor(0,0,0,50));
    rightPanel->setGraphicsEffect(panelShadow);

    // 给棋盘外框添加阴影
    QGraphicsDropShadowEffect *boardShadow = new QGraphicsDropShadowEffect();
    boardShadow->setBlurRadius(20);
    boardShadow->setOffset(0,6);
    boardShadow->setColor(QColor(0,0,0,60));
    boardFrame->setGraphicsEffect(boardShadow);

    // 给按钮添加阴影
    QList<QPushButton*> buttons = {resetbtn, savebtn, readbtn, undobtn, modebtn, exitbtn};
    for(auto btn : buttons){
        QGraphicsDropShadowEffect *btnShadow = new QGraphicsDropShadowEffect();
        btnShadow->setBlurRadius(12);
        btnShadow->setOffset(0,3);
        btnShadow->setColor(QColor(0,0,0,80));
        btn->setGraphicsEffect(btnShadow);
    }
}
/*
void MainWindow::startAIMove()
{
    int type = chessBoard->getAIType();
    AIThread* aiThread = new AIThread(chessBoard, type, this);

    connect(aiThread, &AIThread::moveReady, this, [this, aiThread](ChessBoard::AIMove best){
        if (best.sx != -1) {
            chessBoard->applyMove(best);
            chessBoard->update();
            chessBoard->nextTurn();
            chessBoard->win();
        }
        aiThread->quit();
        aiThread->wait();
        aiThread->deleteLater();
    });

    aiThread->start();
}
*/


MainWindow::~MainWindow()
{
    //delete ui;
}
