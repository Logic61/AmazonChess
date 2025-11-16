#include "chessboard.h"
#include "gamedialog.h"
#include <QWidget>
#include <QPainter>
//#include <QGraphicsEffect>
#include <QMouseEvent>
#include <cmath>
//#include <algorithm>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>

const int size_of_board = 10;
std::vector<std::vector<std::vector<int> > > boardHistory{};
std::vector<int > playerHistory;

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget{parent}
{
    //设置大小
    setMinimumSize(400,400);

    //棋子初始化
    for(int i = 0;i < size_of_board; ++i)
        for(int j = 0;j < size_of_board; ++j)
            board[i][j] = 0;

    board[0][3] = 2;  // 黑棋
    board[0][6] = 2;
    board[3][0] = 2;
    board[3][9] = 2;

    board[6][0] = 1;  // 白棋
    board[6][9] = 1;
    board[9][3] = 1;
    board[9][6] = 1;

    std::vector<std::vector<int>> temp(size_of_board, std::vector<int>(size_of_board, 0));
    for(int i = 0;i < size_of_board; ++i)
        for(int j = 0;j < size_of_board; ++j)
            temp[i][j] = board[i][j];
    boardHistory.push_back(temp);
}

//画画这一块
void ChessBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    int rows = 10;
    int cols = 10;

    int cellWidth = width() / cols;
    int cellHeight = height() / rows;

    //棋盘
    for(int i = 0; i < rows; ++i) {
        for(int j = 0;j < cols; ++j) {

            QColor A(240, 217, 181);
            QColor B(181, 136, 99);
            QColor C(105, 105, 105);
            if(board[i][j] == 3)
                painter.setBrush(C);
            else if((i + j) % 2 == 0)
                painter.setBrush(A);
            else
                painter.setBrush(B);

            painter.drawRect(j * cellWidth, i * cellHeight, cellWidth, cellHeight);
        }
    }

    //画个框
    painter.setPen(QPen(QColor(50,50,50), 6, Qt::SolidLine));
    painter.setBrush(Qt::NoBrush);
    int margin = 5;
    painter.drawRect(margin-3, margin-2, width()-2*margin+2, 2+height()-2*margin);

    //画棋子
    for(int i = 0;i < size_of_board; ++i) {
        for(int j = 0;j < size_of_board; ++j) {
            if(board[i][j] == 0 || board[i][j] == 3) continue;

            //设置颜色
            if(board[i][j] == 1) {
                painter.setBrush(Qt::white);
            }
            else if(board[i][j] == 2){
                painter.setBrush(Qt::black);
            }

            //计算棋子位置和大小
            int margin = 10;
            int x = j * cellWidth + margin;
            int y = i * cellHeight + margin;
            int w = cellWidth - 2 * margin;
            int h = cellHeight - 2 * margin;

            // 用径向渐变，让棋子看起来有立体感
            QRadialGradient gradient(x + w/2, y + h/2, w/2); // 圆心 + 半径
            if(board[i][j] == 1) {
                gradient.setColorAt(0, QColor(255,255,255));   // 中心白
                gradient.setColorAt(1, QColor(200,200,200));   // 边缘灰
            } else {
                gradient.setColorAt(0, QColor(50,50,50));      // 中心黑
                gradient.setColorAt(1, QColor(0,0,0));         // 边缘更黑
            }
            /*失败的尝试之绘制阴影
            QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
            shadow->setBlurRadius(10);
            shadow->setOffset(2, 2);
            setGraphicsEffect(shadow);
            */

            painter.setBrush(gradient);
            painter.setPen(Qt::NoPen);  // 去掉边框
            painter.drawEllipse(x, y, w, h);

            /*备选方案：拿图片
            QPixmap whitePiece(":/images/white.png");  // 自己准备棋子图片
            QPixmap blackPiece(":/images/black.png");

            QPixmap piece = (board[i][j] == 1) ? whitePiece : blackPiece;
            painter.drawPixmap(x, y, w, h, piece);
            */

        }
    }

    //给选中棋子高亮
    if(selectedRow != -1) {
        painter.setPen(QPen(Qt::red, 3));
        int x = selectedCol * cellWidth + 5;
        int y = selectedRow * cellHeight + 5;
        int w = cellWidth - 10;
        int h = cellHeight - 10;
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(x, y, w, h);
    }

    //标出可以走的位置
    if(selectedRow != -1 || toShoot == 1) {
        for(int i = 0; i < rows; ++i) {
            for(int j = 0;j < cols; ++j) {
                if(moveAvailable(selectedRow, selectedCol, i, j)) {
                    int margin = 30;
                    int x = j * cellWidth + margin;
                    int y = i * cellHeight + margin;
                    int w = cellWidth - 2 * margin;
                    int h = cellWidth - 2 * margin;

                    QColor deepGreen(0, 100, 0);   // 深绿色
                    painter.setPen(QPen(deepGreen, 5));
                    painter.setBrush(deepGreen);
                    painter.drawEllipse(x, y, w, h);
                }
            }
        }
    }
}

//鼠标活动这一块
void ChessBoard::mousePressEvent(QMouseEvent *event) {
    int cellWidth = width() / size_of_board;
    int cellHeight = height() / size_of_board;

    int j = event->x() / cellWidth;     //列
    int i = event->y() / cellHeight;    //行

    //没选中棋子，点击到棋子，选中
    if(selectedRow == -1 && board[i][j] == turn) {
        selectedRow = i;
        selectedCol = j;
        update();
        return;
    }

    //选中了棋子，点击代表移动
    if(selectedRow != -1 && toShoot == 0) {
        //可以移动
        if(moveAvailable(selectedRow, selectedCol,i, j)) {
            board[i][j] = board[selectedRow][selectedCol];
            board[selectedRow][selectedCol] = 0;

            selectedRow = i;
            selectedCol = j;

            toShoot = 1;

            /*
            //存盘
            std::vector<std::vector<int>> temp(10, std::vector<int>(10,0));
            for(int i=0;i<10;i++)
                for(int j=0;j<10;j++)
                    temp[i][j] = board[i][j];
            boardHistory.push_back(temp);
            playerHistory.push_back(turn);
            */

            win();
        }
        else {
            selectedRow = -1;
            selectedCol = -1;
        }
        update();
    }

    //射箭
    if(toShoot == 1) {
        if(moveAvailable(selectedRow, selectedCol,i, j)) {
            board[i][j] = 3;
            toShoot = 0;

            selectedRow = -1;
            selectedCol = -1;

            //存盘
            std::vector<std::vector<int>> temp(10, std::vector<int>(10,0));
            for(int i=0;i<10;i++)
                for(int j=0;j<10;j++)
                    temp[i][j] = board[i][j];
            boardHistory.push_back(temp);
            playerHistory.push_back(turn);

            win();

            nextTurn();

            update();
        }
    }
}

//移动合法判断
bool ChessBoard::moveAvailable(int x_now, int y_now, int x_to, int y_to) const {
    int flag = 0;//合法？
    if(board[x_to][y_to] != 0) return 0;

    //路径合理
    if(x_now == x_to || y_now == y_to)
        flag = 1;
    if(abs(x_now - x_to) == abs(y_now - y_to))
        flag = 2;

    if(flag == 0)
        return 0;

    //障碍与否
    int dx = (x_now == x_to ? 0 : (x_to - x_now) / abs(x_to - x_now));
    int dy = (y_now == y_to ? 0 : (y_to - y_now) / abs(y_to - y_now));
    int pos_x = x_now;
    int pos_y = y_now;
    flag = 1;

    while(pos_x != x_to || pos_y != y_to) {
        pos_x += dx;
        pos_y += dy;
        if(board[pos_x][pos_y] != 0) {
            flag = 0;
            break;
        }
    }

    if(flag)
        return 1;
    else
        return 0;
}

//新游戏
void ChessBoard::reset() {
    for(int i = 0;i < size_of_board; ++i)
        for(int j = 0;j < size_of_board; ++j)
            board[i][j] = 0;

    turn = 1;

    board[0][3] = 2;  board[0][6] = 2;
    board[3][0] = 2;  board[3][9] = 2;
    board[6][0] = 1;  board[6][9] = 1;
    board[9][3] = 1;  board[9][6] = 1;

    selectedRow = -1;
    selectedCol = -1;

    boardHistory.clear();
    playerHistory.clear();

    toShoot = 0;

    emit turnChanged(turn);
    update();
}


//判断输赢
void ChessBoard::win() {
    //位移
    int dx[] = {0, 0, 1, 1, 1, -1, -1, -1};
    int dy[] = {1, -1, 0, -1, 1, 0, -1, 1};

    //看看黑方
    int flag = 0; //动？
    for(int i = 0;i < size_of_board; ++i) {
        for(int j = 0;j < size_of_board; ++j) {
            if(flag) break;
            if(board[i][j] == 2) {
                for(int k = 0;k < 8; ++k) {
                    int x = i + dx[k];
                    int y = j + dy[k];

                    if(x >= 0 && y >= 0 && x < size_of_board && y < size_of_board) {
                        if(board[x][y] == 0) {
                            flag = 1;
                        }
                    }
                }
            }
        }
    }

    if(flag == 0) {
        update();
        showGameOverDialog(this, "白方获胜！", [this]{ reset(); });
        /*
        // 创建弹窗
        QMessageBox msgBox;
        msgBox.setWindowTitle("游戏结束");

        // 修改整体字体
        QFont font;
        font.setPointSize(16);       // 主字体大小
        msgBox.setFont(font);

        // 设置文本（多行更美观）
        msgBox.setText("<p align='center'> <br> 白方获胜！<br> <br> 下一步？</p>");

        // 取到内部的 QLabel，加大字体让弹窗变大
        QLabel *label = msgBox.findChild<QLabel *>();
        if (label) {
            QFont lf = label->font();
            lf.setPointSize(16);     // 提示语句更大
            label->setFont(lf);

            label->setAlignment(Qt::AlignCenter);
        }

        // 自定义按钮
        QPushButton *btn1 = new QPushButton("再开一把");
        QPushButton *btn2 = new QPushButton("不玩了，退出");

        // 让按钮变宽变高
        btn1->setMinimumHeight(36);
        btn2->setMinimumHeight(36);

        QFont bf;
        bf.setPointSize(13);
        btn1->setFont(bf);
        btn2->setFont(bf);

        // 放进 msgBox
        msgBox.addButton(btn1, QMessageBox::AcceptRole);
        msgBox.addButton(btn2, QMessageBox::RejectRole);

        // 显示
        msgBox.exec();

        // 判断结果
        if (msgBox.clickedButton() == btn1) {
            reset();
        } else if (msgBox.clickedButton() == btn2) {
            qApp->quit();
        }
        */

        return;

    }

    //看看白方
    flag = 0; //动？
    for(int i = 0;i < size_of_board; ++i) {
        for(int j = 0;j < size_of_board; ++j) {
            if(flag) break;
            if(board[i][j] == 1) {
                for(int k = 0;k < 8; ++k) {
                    int x = i + dx[k];
                    int y = j + dy[k];

                    if(x >= 0 && y >= 0 && x < size_of_board && y < size_of_board) {
                        if(board[x][y] == 0) {
                            flag = 1;
                        }
                    }
                }
            }
        }
    }

    if(flag == 0) {
        update();

        showGameOverDialog(this, "黑方获胜！", [this](){ reset(); }); {
            reset();  // 你的重置棋盘函数
        };

        return;
    }
}

//跳结束弹窗
void ChessBoard::showGameOverDialog(QWidget *parent, const QString &winnerText, std::function<void()> onRestart) {
    QDialog dlg(parent);
    dlg.setWindowTitle("游戏结束");
    dlg.setModal(true);
    dlg.resize(400, 250);
    dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);

    // 文本
    QLabel *label = new QLabel(winnerText + "<br>下一步？");
    label->setAlignment(Qt::AlignCenter);
    QFont f; f.setPointSize(18); f.setBold(true);
    label->setFont(f);
    label->setStyleSheet("color: white;");
    layout->addWidget(label);

    // 按钮布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnRestart = new QPushButton("再开一把");
    QPushButton *btnQuit    = new QPushButton("不玩了，退出");

    QFont bf; bf.setPointSize(14);
    btnRestart->setFont(bf);
    btnQuit->setFont(bf);
    btnRestart->setMinimumHeight(36);
    btnQuit->setMinimumHeight(36);

    // 区分颜色
    btnRestart->setStyleSheet(
        "QPushButton {background-color: #4CAF50; color: white; border-radius: 5px; padding: 10px 20px;}"
        "QPushButton:hover {background-color: #45a049;}"
        );

    btnQuit->setStyleSheet(
        "QPushButton {background-color: #f44336; color: white; border-radius: 5px; padding: 10px 20px;}"
        "QPushButton:hover {background-color: #da190b;}"
        );

    btnLayout->addWidget(btnRestart);
    btnLayout->addWidget(btnQuit);
    layout->addLayout(btnLayout);

    // 信号
    QObject::connect(btnRestart, &QPushButton::clicked, [&dlg, onRestart]() {
        dlg.accept();
        onRestart();
    });

    QObject::connect(btnQuit, &QPushButton::clicked, [&dlg]() {
        dlg.reject();
        qApp->quit();
    });

    dlg.exec();
}

void ChessBoard::modeSet() {

}

void ChessBoard::undomove() {

    if(boardHistory.size() == 1) {
        QDialog dlg(this);
        dlg.setWindowTitle("提示");
        dlg.setModal(true);
        dlg.resize(400, 250);
        dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&dlg);

        // 文本
        QLabel *label = new QLabel("您还没下过棋呢");
        label->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(18); f.setBold(true);
        label->setFont(f);
        label->setStyleSheet("color: white;");
        layout->addWidget(label);

        dlg.exec();
        return;
    }

    boardHistory.pop_back();
    playerHistory.pop_back();
    std::vector<std::vector<int> > temp = boardHistory.back();
    int lastTurn = playerHistory.back();


    for(int i = 0;i < size_of_board; ++i) {
        for(int j = 0;j < size_of_board; ++j) {
            if(board[i][j] == 1 && temp[i][j] != 1)
                turn = 1;
            if(board[i][j] == 2 && temp[i][j] != 2)
                turn = 2;
        }
    }

    for(int i = 0;i < size_of_board; i++)
        for(int j = 0;j < size_of_board; j++)
            board[i][j] = temp[i][j];

    selectedCol = -1;
    selectedRow = -1;
    toShoot = 0;

    emit turnChanged(turn);

    update();
}
