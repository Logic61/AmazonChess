#include "mainwindow.h"
//#include "./ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include "chessboard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    const int height_of_button = 60;
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
    mainLayout->addWidget(board);

    //右边区域，vertical布局
    QVBoxLayout *rightPanel = new QVBoxLayout();
    mainLayout->addLayout(rightPanel);

    //回合
    QLabel *playerLabel = new QLabel("当前玩家：白方");
    playerLabel->setAlignment(Qt::AlignCenter);
    playerLabel->setStyleSheet("color: blue; font-weight: bold; font-size: 18px;");

    connect(board, &ChessBoard::turnChanged, this, [=](int newTurn){
        if(newTurn == 1)
            playerLabel->setText("当前玩家：白方");
        else
            playerLabel->setText("当前玩家：黑方");
    });

    rightPanel->addWidget(playerLabel);

    //调左右比例
    mainLayout->setStretch(0,7);
    mainLayout->setStretch(1,2);

    // 按钮区域
    QGroupBox *box = new QGroupBox("");
    QVBoxLayout *boxLayout = new QVBoxLayout(box);
    //boardButton->setMinimumHeight(200);
    QPushButton *resetbtn = new QPushButton("开始新对局");
    QPushButton *btn2 = new QPushButton("存储当前棋盘");
    QPushButton *btn3 = new QPushButton("读取对局");
    QPushButton *undobtn = new QPushButton("悔棋");
    QPushButton *btn5 = new QPushButton("认输");
    QPushButton *modebtn = new QPushButton("模式选择");
    boxLayout->addWidget(modebtn);
    boxLayout->addWidget(resetbtn);
    boxLayout->addWidget(btn2);
    boxLayout->addWidget(btn3);
    boxLayout->addWidget(undobtn);
    boxLayout->addWidget(btn5);
    //按钮高度
    resetbtn->setFixedHeight((height_of_button));
    btn2->setFixedHeight((height_of_button));
    btn3->setFixedHeight((height_of_button));
    undobtn->setFixedHeight((height_of_button));
    btn5->setFixedHeight((height_of_button));
    modebtn->setFixedHeight((height_of_button));

    rightPanel->addWidget(box);

    //按钮实现
    connect(resetbtn, &QPushButton::clicked, board, &ChessBoard::reset);
    connect(modebtn, &QPushButton::clicked, board, &ChessBoard::modeSet);
    connect(undobtn, &QPushButton::clicked, board, &ChessBoard::undomove);
}

MainWindow::~MainWindow()
{
    //delete ui;
}
