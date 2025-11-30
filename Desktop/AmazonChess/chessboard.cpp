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
#include <QFile>
#include <QTimer>
#include <math.h>
#include <queue>

    const int size_of_board = 10;
std::vector<std::vector<std::vector<int> > > boardHistory{};
std::vector<int > playerHistory;
int aiType = 0;

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

void ChessBoard::drawArrow(QPainter &painter, int row, int col, int cellWidth, int cellHeight)
{
    // 箭颜色：渐变粉色
    QLinearGradient gradient(col * cellWidth, row * cellHeight,
                             col * cellWidth + cellWidth, row * cellHeight + cellHeight);
    gradient.setColorAt(0, QColor(255, 182, 193));  // 浅粉
    gradient.setColorAt(1, QColor(255, 105, 180));  // 深粉

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);  // 不需要边框

    // 箭的矩形部分（稍小于格子）
    int margin = cellWidth / 4;
    QRect rect(col * cellWidth + margin/2, row * cellHeight + margin/2,
               cellWidth - margin, cellHeight - margin);

    // 先画箭体
    painter.drawRect(rect);

    // 再画箭头
    QPolygon arrowHead;
    arrowHead << QPoint(col * cellWidth + cellWidth/2, row * cellHeight + margin/2)            // 顶部中心
              << QPoint(col * cellWidth + 3*cellWidth/4, row * cellHeight + cellHeight/2)      // 右下
              << QPoint(col * cellWidth + cellWidth/2, row * cellHeight + 3*cellHeight/4)      // 底部中心
              << QPoint(col * cellWidth + cellWidth/4, row * cellHeight + cellHeight/2);       // 左下
    painter.drawPolygon(arrowHead);
}


//画画这一块
void ChessBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    int rows = 10;
    int cols = 10;

    int cellWidth = width() / cols;
    int cellHeight = height() / rows;

    QColor C(105, 105, 105);

    //棋盘
    for(int i = 0; i < rows; ++i) {
        for(int j = 0;j < cols; ++j) {

            QColor A(240, 217, 181);
            QColor B(181, 136, 99);

            if((i + j) % 2 == 0)
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

            if(board[i][j] == 0) continue;

            //设置颜色
            if(board[i][j] == 3) {
                drawArrow(painter, i, j, cellWidth, cellHeight);
                continue;
            }
            else if(board[i][j] == 1) {
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
            QPixmap whitePiece(":/images/white.png");  // 棋子图片
            QPixmap blackPiece(":/images/black.png");

            QPixmap piece = (board[i][j] == 1) ? whitePiece : blackPiece;
            painter.drawPixmap(x, y, w, h, piece);
            */

        }
    }

    //给选中棋子高亮
    if(selectedRow != -1 && toShoot == 0) {
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
    // 高亮移动的棋子所在格子
    if(lastMoveX != -1 && lastMoveY != -1) {
        painter.setPen(QPen(Qt::blue, 3));   // 蓝色边框
        painter.setBrush(Qt::NoBrush);       // 不填充
        int x = lastMoveY * cellWidth;
        int y = lastMoveX * cellHeight;
        painter.drawRect(x + 2, y + 2, cellWidth - 4, cellHeight - 4); // 留一点间距
    }

    // 高亮射出的箭所在格子
    if(lastArrowX != -1 && lastArrowY != -1) {
        painter.setPen(QPen(Qt::blue, 3));
        painter.setBrush(Qt::NoBrush);
        int x = lastArrowY * cellWidth;
        int y = lastArrowX * cellHeight;
        painter.drawRect(x + 2, y + 2, cellWidth - 4, cellHeight - 4);
        update();
    }

}
int undo_ok = 1;
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
        if((gameMode == 1 && aiType == board[selectedRow][selectedCol])) {
            QDialog dlg(this);
            dlg.setWindowTitle("提示");
            dlg.setModal(true);
            dlg.resize(400, 250);
            dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

            QVBoxLayout *layout = new QVBoxLayout(&dlg);

            // 文本
            QLabel *label = new QLabel("这是人机用的棋子");
            label->setAlignment(Qt::AlignCenter);
            QFont f; f.setPointSize(18); f.setBold(true);
            label->setFont(f);
            label->setStyleSheet("color: white;");
            layout->addWidget(label);

            dlg.exec();
        }
        //可以移动
        if(moveAvailable(selectedRow, selectedCol,i, j)) {
            board[i][j] = board[selectedRow][selectedCol];
            board[selectedRow][selectedCol] = 0;

            selectedRow = i;
            selectedCol = j;
            lastMoveX = i;    // 记录最新移动的棋子
            lastMoveY = j;

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
            lastArrowX = i;    // 记录最新移动的棋子
            lastArrowY = j;

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

            if(gameMode == 1 && turn == aiType) {
                undo_ok = 0;
                QTimer::singleShot(1500, this, &ChessBoard::aiMove);
                win();  // 检查输赢
            }
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
    // 清空棋盘
    for(int i = 0;i < size_of_board; ++i)
        for(int j = 0;j < size_of_board; ++j)
            board[i][j] = 0;

    turn = 2;

    // 初始棋子
    board[0][3] = 2;  board[0][6] = 2;
    board[3][0] = 2;  board[3][9] = 2;
    board[6][0] = 1;  board[6][9] = 1;
    board[9][3] = 1;  board[9][6] = 1;

    selectedRow = -1;
    selectedCol = -1;
    lastArrowX = -1;
    lastArrowY = -1;
    lastMoveX = -1;
    lastMoveY = -1;

    boardHistory.clear();
    playerHistory.clear();

    // ✅ 保存初始状态
    std::vector<std::vector<int> > temp(size_of_board, std::vector<int>(size_of_board));
    for(int i = 0;i < size_of_board; ++i)
        for(int j = 0;j < size_of_board; ++j)
            temp[i][j] = board[i][j];
    boardHistory.push_back(temp);
    playerHistory.push_back(turn);

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
    GameDialog *dlg = new GameDialog(
        this,
        "请选择游戏模式",
        "PVE",
        "PVP",
        [=]() {  // 点击 PVE
            gameMode = 1;
            GameDialog *dlgg = new GameDialog(
                this,
                "您将与人机对弈\n请选择您的先后手",
                "先手",
                "后手",
                [=]() {
                    reset();
                    aiType = 1;
                    emit turnChanged(turn);
                },
                [=]() {
                    reset();
                    QTimer::singleShot(1500, this, &ChessBoard::aiMove);
                    aiType = 2;
                    emit turnChanged(turn);
                }
                );

            dlgg->exec();
        },
        [=]() {  // 点击 PVP
            gameMode = 0;
            reset();
        }
        );

    dlg->exec();
}

void ChessBoard::undomove() {
    if(boardHistory.size() == 2 && gameMode == 1) {
        QDialog dlg(this);
        dlg.setWindowTitle("提示");
        dlg.setModal(true);
        dlg.resize(400, 250);
        dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&dlg);

        // 文本
        QLabel *label = new QLabel("您还没移动过");
        label->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(18); f.setBold(true);
        label->setFont(f);
        label->setStyleSheet("color: white;");
        layout->addWidget(label);

        dlg.exec();
        return;
    }

    if(gameMode == 1) {
        QDialog dlg(this);
        dlg.setWindowTitle("提示");
        dlg.setModal(true);
        dlg.resize(400, 250);
        dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&dlg);

        // 文本
        QLabel *label = new QLabel("人机模式不可悔棋");
        label->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(18); f.setBold(true);
        label->setFont(f);
        label->setStyleSheet("color: white;");
        layout->addWidget(label);

        dlg.exec();
        return;
    }

    if(boardHistory.size() == 1) {
        QDialog dlg(this);
        dlg.setWindowTitle("提示");
        dlg.setModal(true);
        dlg.resize(400, 250);
        dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&dlg);

        // 文本
        QLabel *label = new QLabel("您的对局记录为空哦");
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
    if(boardHistory.size() == 1) {
        reset();
        return;
    }

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
    lastArrowX = -1;
    lastArrowY = -1;
    lastMoveX = -1;
    lastMoveY = -1;

    /*if(gameMode == 1 && turn == aiType) {
        boardHistory.pop_back();
        playerHistory.pop_back();
        std::vector<std::vector<int> > temp = boardHistory.back();
        if(boardHistory.size() == 1) {
            reset();
            return;
        }

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

    }*/

    emit turnChanged(turn);

    update();
}

void ChessBoard::saveGame(const QString &filename) {
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QDialog dlg(this);
        dlg.setWindowTitle("提示");
        dlg.setModal(true);
        dlg.resize(400, 250);
        dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&dlg);

        // 文本
        QLabel *label = new QLabel("存档保存失败-_-");
        label->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(18); f.setBold(true);
        label->setFont(f);
        label->setStyleSheet("color: white;");
        layout->addWidget(label);

        dlg.exec();
        return;
    }

    QTextStream out(&file);

    int stepCount = boardHistory.size();
    out << stepCount << "\n";

    for(int k = 0; k < stepCount; ++k) {
        out << playerHistory[k] << "\n";

        for(int i = 0;i < size_of_board; ++i) {
            for(int j = 0; j < size_of_board; ++j) {
                out << boardHistory[k][i][j] << " ";
            }
            out << "\n";
        }
    }

    file.close();

    QDialog dlg(this);
    dlg.setWindowTitle("提示");
    dlg.setModal(true);
    dlg.resize(400, 250);
    dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);

    // 文本
    QLabel *label = new QLabel("存档成功！");
    label->setAlignment(Qt::AlignCenter);
    QFont f; f.setPointSize(18); f.setBold(true);
    label->setFont(f);
    label->setStyleSheet("color: white;");
    layout->addWidget(label);

    dlg.exec();
}

void ChessBoard::readGame(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QDialog dlg(this);
        dlg.setWindowTitle("提示");
        dlg.setModal(true);
        dlg.resize(400, 250);
        dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&dlg);

        // 文本
        QLabel *label = new QLabel("存档打开失败-_-");
        label->setAlignment(Qt::AlignCenter);
        QFont f; f.setPointSize(18); f.setBold(true);
        label->setFont(f);
        label->setStyleSheet("color: white;");
        layout->addWidget(label);

        dlg.exec();
        return;
    }

    QTextStream in(&file);

    reset();

    int stepCount;
    in >> stepCount;                                      // 读取文件中第一行的步数（整数）
    in.readLine();

    for (int k = 0; k < stepCount; k++) {
        int turnValue;
        in >> turnValue;                                  // 读取该步的玩家值
        playerHistory.push_back(turnValue);
        in.readLine();                                    // 读取并丢弃到行尾，确保下一行是棋盘的第一行

        // 准备一个临时棋盘
        std::vector<std::vector<int>> tempBoard(size_of_board,
                                                std::vector<int>(size_of_board));

        // 逐行逐列读取棋盘数据
        for (int i = 0; i < size_of_board; i++) {
            for (int j = 0; j < size_of_board; j++) {
                in >> tempBoard[i][j];                    // 读取一个整数并放到临时棋盘对应位置
            }
            in.readLine();                                // 读取并丢弃到行尾（如果存在额外空格或换行）
        }

        boardHistory.push_back(tempBoard);                // 把这一步的棋盘推入 boardHistory
    }

    // 恢复到最后一步（如果有历史）
    if (!boardHistory.empty()) {
        for(int i = 0;i < size_of_board; ++i) {
            for(int j = 0;j < size_of_board; ++j) {
                board[i][j] = boardHistory.back()[i][j];
            }
        }

        boardHistory.pop_back();
        std::vector<std::vector<int> > temp = boardHistory.back();

        for(int i = 0;i < size_of_board; ++i) {
            for(int j = 0;j < size_of_board; ++j) {
                if(board[i][j] == 1 && temp[i][j] != 1) {
                    turn = 2;
                }
                if(board[i][j] == 2 && temp[i][j] != 2) {
                    turn = 1;

                }
            }
        }                     // 恢复最后一步对应的玩家到 turn（当前回合）

        for(int i = 0;i < size_of_board; ++i) {
            for(int j = 0;j < size_of_board; ++j) {
                temp[i][j] = board[i][j];
            }
        }
        emit turnChanged(turn);
        boardHistory.push_back(temp);
    }

    file.close();
    update();

    QDialog dlg(this);
    dlg.setWindowTitle("提示");
    dlg.setModal(true);
    dlg.resize(400, 250);
    dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);

    // 文本
    QLabel *label = new QLabel("存档加载成功！");
    label->setAlignment(Qt::AlignCenter);
    QFont f; f.setPointSize(18); f.setBold(true);
    label->setFont(f);
    label->setStyleSheet("color: white;");
    layout->addWidget(label);

    dlg.exec();
}

void ChessBoard::aiMove() {
    int searchDepth = ChessBoard::getDynamicDepth();

    AIMove aiMove = findBestMove(searchDepth);

    if(turn != aiType) {
        qDebug() << "错误: 现在不是AI的回合!";
        return;
    }

    // 执行落子
    board[aiMove.qx][aiMove.qy] = 0;
    board[aiMove.mx][aiMove.my] = aiType;

    lastMoveX = aiMove.mx;
    lastMoveY = aiMove.my;
    update();

    QTimer::singleShot(500, this, [=](){
        board[aiMove.ax][aiMove.ay] = 3;
        lastArrowX = aiMove.ax;
        lastArrowY = aiMove.ay;

        // 存盘
        std::vector<std::vector<int>> temp(size_of_board, std::vector<int>(size_of_board,0));
        for(int i = 0;i < size_of_board;i++)
            for(int j = 0;j < size_of_board;j++)
                temp[i][j] = board[i][j];
        boardHistory.push_back(temp);
        playerHistory.push_back(turn);

        win();
        undo_ok = 1;
        // 更新界面 & 轮到玩家
        nextTurn();
        update();
    });
}


void ChessBoard::exit() {
    QDialog dlg;
    dlg.setWindowTitle("游戏结束");
    dlg.setModal(true);
    dlg.resize(400, 250);
    dlg.setStyleSheet("background-color: #0b3d0b; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);

    // 文本
    QLabel *label = new QLabel("您确认要退出吗");
    label->setAlignment(Qt::AlignCenter);
    QFont f; f.setPointSize(18); f.setBold(true);
    label->setFont(f);
    label->setStyleSheet("color: white;");
    layout->addWidget(label);

    // 按钮布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnRestart = new QPushButton("再玩一会");
    QPushButton *btnQuit    = new QPushButton("退出");

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
    QObject::connect(btnRestart, &QPushButton::clicked, [&dlg]() {
        dlg.accept();
    });

    QObject::connect(btnQuit, &QPushButton::clicked, [&dlg]() {
        dlg.reject();
        qApp->quit();
    });

    dlg.exec();
}

// 历史表
int history_table[10][10][10][10]{0};

// 可调权重
int wLocalMobility = 10;
int wArrowBlock = 3;
double wCenterControl = 1.0/3.0;

std::vector<ChessBoard::AIMove> ChessBoard::generateAllMoves(int player) {
    std::vector<ChessBoard::AIMove> moves;
    moves.reserve(256);

    const int dx[8] = {-1,-1,-1,0,0,1,1,1};
    const int dy[8] = {-1,0,1,-1,1,-1,0,1};

    int opponent = (player == 1 ? 2 : 1);

    for(int i = 0;i < size_of_board; ++i) {
        for(int j = 0;j < size_of_board; ++j) {

            if(board[i][j] != player) continue;

            //枚举棋子移动
            for(int d = 0;d < 8; ++d) {
                int x = i, y = j;

                while(1) {
                    x += dx[d];
                    y += dy[d];

                    if(x < 0 || x >= size_of_board || y < 0 || y >= size_of_board) break;
                    if(board[x][y] != 0) break;

                    //暂时移动
                    board[i][j] = 0;
                    board[x][y] = player;

                    //箭 - 从移动后的位置发射
                    for(int ad = 0; ad < 8; ++ad) {
                        int ax = x, ay = y;

                        while(1) {
                            ax += dx[ad];
                            ay += dy[ad];

                            if(ax < 0 || ax >= size_of_board || ay < 0 || ay >= size_of_board) break;
                            if(board[ax][ay] != 0) break;

                            AIMove m = {i,j,x,y,ax,ay,0};
                            m.score = quickHeuristic(m, player);
                            moves.push_back(m);
                        }
                    }

                    //恢复棋盘
                    board[i][j] = player;
                    board[x][y] = 0;
                }
            }
        }
    }

    // 降序排序
    std::sort(moves.begin(), moves.end(),
              [](const AIMove &a, const AIMove &b) {
                  return a.score > b.score;
              });

    // 只保留前N个最佳走法（减少搜索量）
    if(moves.size() > 150) {
        moves.resize(150);
    }

    return moves;
}

int ChessBoard::getAggressiveness(int x, int y, int opponent) {
    int minDist = 999;

    // 找到最近的对手棋子
    for(int i = 0; i < size_of_board; i++) {
        for(int j = 0; j < size_of_board; j++) {
            if(board[i][j] == opponent) {
                int dist = std::max(abs(i - x), abs(j - y)); // 切比雪夫距离
                minDist = std::min(minDist, dist);
            }
        }
    }

    // 距离越近，分数越高
    return (size_of_board - minDist);
}

int ChessBoard::localMobility(int x, int y) {
    const int dx[8] = {-1,-1,-1,0,0,1,1,1};
    const int dy[8] = {-1,0,1,-1,1,-1,0,1};
    int count = 0;

    for(int d = 0; d < 8; ++d) {
        int nx = x, ny = y;
        while (true) {
            nx += dx[d];
            ny += dy[d];
            if(nx < 0 || nx >= size_of_board || ny < 0 || ny >= size_of_board) break;
            if(board[nx][ny] != 0) break;
            ++count;
        }
    }
    return count;
}


int ChessBoard::arrowBlock(int ax, int ay, int opponent) {
    const int dx[8] = {-1,-1,-1,0,0,1,1,1};
    const int dy[8] = {-1,0,1,-1,1,-1,0,1};

    int blockScore = 0;

    for(int d = 0; d < 8; ++d) {
        int nx = ax + dx[d];
        int ny = ay + dy[d];

        if(nx >= 0 && nx < size_of_board &&
            ny >= 0 && ny < size_of_board &&
            board[nx][ny] == opponent)
        {
            // 检查这个对手棋子是否被封锁（所有移动方向都被阻塞）
            if(localMobility(nx, ny) == 0) {
                blockScore += 3; // 完全封锁
            } else {
                blockScore += 1; // 部分封锁
            }
        }
    }
    return blockScore;
}

int ChessBoard::centerControl(int x, int y) {
    int cx = size_of_board / 2;
    int cy = size_of_board / 2;

    // 越靠近中心分越高（小负数）
    return -(abs(x - cx) + abs(y - cy));
}

int ChessBoard::quickHeuristic(const AIMove &m, int player) {
    int score = 0;
    score += wLocalMobility * localMobility(m.mx, m.my);
    score += wArrowBlock * arrowBlock(m.ax, m.ay, (player==1?2:1));
    score += wCenterControl * centerControl(m.mx, m.my);
    return score;
}

int ChessBoard::quickEvaluate(int player) {
    int score = 0;
    int opponent = 3 - player;

    for(int i = 0; i < size_of_board; i++) {
        for(int j = 0; j < size_of_board; j++) {
            if(board[i][j] == player) {
                score += localMobility(i, j);
                score += (int)(wCenterControl * centerControl(i, j));
            } else if(board[i][j] == opponent) {
                score -= localMobility(i, j);
                score -= (int)(wCenterControl * centerControl(i, j));
            }
        }
    }

    return score;
}

ChessBoard::AIMove ChessBoard::findBestMove(int depth) {
    int bestScore = std::numeric_limits<int>::min();
    AIMove bestMove = {-1, -1, -1, -1, -1, -1, 0};

    auto moves = generateAllMoves(aiType);
    if(moves.empty()) {
        return bestMove;
    }

    for(auto &m : moves) {
        // 保存原始状态
        int oldQx = board[m.qx][m.qy];
        int oldMx = board[m.mx][m.my];
        int oldAx = board[m.ax][m.ay];

        // 执行移动
        board[m.qx][m.qy] = 0;
        board[m.mx][m.my] = aiType;
        board[m.ax][m.ay] = 3;

        int score = alphaBeta(depth - 1,
                              std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max(),
                              3 - aiType);

        //恢复
        board[m.qx][m.qy] = oldQx;
        board[m.mx][m.my] = oldMx;
        board[m.ax][m.ay] = oldAx;

        if(score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
    }

    return bestMove;
}

int nodeCount = 0;
const int MAX_NODES = 10000000;

int ChessBoard::alphaBeta(int depth, int alpha, int beta, int player) {
    if(++nodeCount > MAX_NODES) {
        qDebug() << "警告：搜索节点超限！";
        //return quickEvaluate(player);
    }

    if(depth == 0 || isGameOver(player)) {
        return advancedEvaluate(player);
    }

    auto moves = generateAllMoves(player);
    if(moves.empty()) {
        return advancedEvaluate(player);
    }

    int opponent = 3 - player;

    if(player == aiType) {
        int maxEval = std::numeric_limits<int>::min();
        for(auto &m : moves) {
            // 保存原始状态
            int oldQx = board[m.qx][m.qy];
            int oldMx = board[m.mx][m.my];
            int oldAx = board[m.ax][m.ay];

            // 执行移动
            board[m.qx][m.qy] = 0;
            board[m.mx][m.my] = player;
            board[m.ax][m.ay] = 3;

            int eval = alphaBeta(depth - 1, alpha, beta, opponent);

            // 恢复
            board[m.qx][m.qy] = oldQx;
            board[m.mx][m.my] = oldMx;
            board[m.ax][m.ay] = oldAx;

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            // 产生了剪枝，记录下来
            if(beta <= alpha) {
                history_table[m.qx][m.qy][m.mx][m.my] += depth * depth;
                break;
            }
        }
        return maxEval;
    }
    else {
        int minEval = std::numeric_limits<int>::max();
        for(auto &m : moves) {
            // 保存原始状态
            int oldQx = board[m.qx][m.qy];
            int oldMx = board[m.mx][m.my];
            int oldAx = board[m.ax][m.ay];

            // 执行移动
            board[m.qx][m.qy] = 0;
            board[m.mx][m.my] = player;
            board[m.ax][m.ay] = 3;

            int eval = alphaBeta(depth - 1, alpha, beta, opponent);

            // 恢复
            board[m.qx][m.qy] = oldQx;
            board[m.mx][m.my] = oldMx;
            board[m.ax][m.ay] = oldAx;

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if(beta <= alpha) break;
        }
        return minEval;
    }
}

bool ChessBoard::isGameOver(int player) {
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
        return 1;
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
        return 1;
    }
    return 0;
}

// 动态深度：根据局面复杂度调整
int ChessBoard::getDynamicDepth() {
    int totalPieces = 0;
    int emptySpaces = 0;

    for(int i = 0; i < size_of_board; i++) {
        for(int j = 0; j < size_of_board; j++) {
            if(board[i][j] == 1 || board[i][j] == 2) totalPieces++;
            if(board[i][j] == 0) emptySpaces++;
        }
    }

    // 残局搜索更深
    if(emptySpaces < 30) return 4;
    if(emptySpaces < 50) return 3;
    return 2;
}

int ChessBoard::calculateTerritory(int x, int y, int player) {
    // 计算从该棋子出发，3步内能到达的空格数
    bool visited[10][10] = {false};
    std::queue<std::tuple<int, int, int>> q; // x, y, depth
    q.push({x, y, 0});
    visited[x][y] = true;

    int territory = 0;
    const int dx[] = {-1,-1,-1,0,0,1,1,1};
    const int dy[] = {-1,0,1,-1,1,-1,0,1};
    const int MAX_DEPTH = 3;

    while(!q.empty()) {
        auto [cx, cy, depth] = q.front();
        q.pop();

        if(depth >= MAX_DEPTH) continue;

        for(int d = 0; d < 8; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if(nx >= 0 && nx < size_of_board &&
                ny >= 0 && ny < size_of_board &&
                !visited[nx][ny] && board[nx][ny] == 0) {
                visited[nx][ny] = true;
                territory++;
                q.push({nx, ny, depth + 1});
            }
        }
    }

    return territory;
}

int ChessBoard::positionValue(int x, int y) {
    // 中心位置更有价值
    int cx = size_of_board / 2;
    int cy = size_of_board / 2;
    int distToCenter = abs(x - cx) + abs(y - cy);

    // 距离中心越近，分数越高
    return (size_of_board - distToCenter) * 2;
}

int ChessBoard::distanceToNearestAlly(int x, int y, int player) {
    int minDist = 999;

    for(int i = 0; i < size_of_board; i++) {
        for(int j = 0; j < size_of_board; j++) {
            if(board[i][j] == player && !(i == x && j == y)) {
                int dist = abs(i - x) + abs(j - y);
                minDist = std::min(minDist, dist);
            }
        }
    }

    return minDist;
}

std::pair<int,int> ChessBoard::calculateTerritoriesGlobal() {
    // visited 标记空格是否已被处理
    std::vector<std::vector<char>> visited(size_of_board, std::vector<char>(size_of_board, 0));
    const int dx8[8] = {-1,-1,-1,0,0,1,1,1};
    const int dy8[8] = {-1,0,1,-1,1,-1,0,1};

    int terr1 = 0;
    int terr2 = 0;

    for(int i = 0; i < size_of_board; ++i) {
        for(int j = 0; j < size_of_board; ++j) {
            // 只从未访问过的空格开始一次 BFS（8 邻接）
            if(board[i][j] != 0) continue;
            if(visited[i][j]) continue;

            // BFS 队列
            std::queue<std::pair<int,int>> q;
            q.push({i,j});
            visited[i][j] = 1;

            int compSize = 0;
            bool touch1 = false;
            bool touch2 = false;

            while(!q.empty()) {
                auto [cx, cy] = q.front(); q.pop();
                ++compSize;

                // 对该空格的 8 邻邻居做检查：
                // - 若邻居是空且未访问，则加入组件
                // - 若邻居是玩家棋子，则标记该组件能被该玩家接触到
                for(int d = 0; d < 8; ++d) {
                    int nx = cx + dx8[d];
                    int ny = cy + dy8[d];
                    if(nx < 0 || nx >= size_of_board || ny < 0 || ny >= size_of_board) continue;

                    if(board[nx][ny] == 0) {
                        if(!visited[nx][ny]) {
                            visited[nx][ny] = 1;
                            q.push({nx, ny});
                        }
                    } else if(board[nx][ny] == 1) {
                        touch1 = true;
                    } else if(board[nx][ny] == 2) {
                        touch2 = true;
                    }
                }
            }

            // 归属判定：只接触到1 or 2 则计入对应玩家领地；两者都能接触则为中立不计
            if(touch1 && !touch2) terr1 += compSize;
            else if(touch2 && !touch1) terr2 += compSize;
            else {
                // touch1 && touch2 -> 中立区域：不计入任何一方
                // 既不属于双方，什么也不做
            }
        }
    }

    return {terr1, terr2};
}

int ChessBoard::advancedEvaluate(int player) {
    int score = 0;
    int opponent = 3 - player;

    // 1. 移动性评估（加权）
    int myMobility = 0, oppMobility = 0;

    // 2. 领地控制（可达区域）
    int myTerritory = 0, oppTerritory = 0;

    // 3. 棋子间距离（分散 vs 集中）
    int myCompactness = 0, oppCompactness = 0;

    for(int i = 0; i < size_of_board; i++) {
        for(int j = 0; j < size_of_board; j++) {
            if(board[i][j] == player) {
                // 移动性
                int mobility = localMobility(i, j);
                myMobility += mobility;

                // 领地（BFS/DFS可达区域）
                myTerritory += calculateTerritory(i, j, player);

                // 位置价值（中心 > 边缘）
                score += positionValue(i, j);

                // 与队友距离（不要太分散）
                myCompactness += distanceToNearestAlly(i, j, player);

            } else if(board[i][j] == opponent) {
                oppMobility += localMobility(i, j);
                //oppTerritory += calculateTerritory(i, j, opponent);
                score -= positionValue(i, j);
                oppCompactness += distanceToNearestAlly(i, j, opponent);
            }
        }
    }
    auto territories = calculateTerritoriesGlobal();
    int terr1 = territories.first;
    int terr2 = territories.second;

    myTerritory = (player == 1 ? terr1 : terr2);
    oppTerritory = (player == 1 ? terr2 : terr1);
    // 加权组合
    score += 50 * (myMobility - oppMobility);        // 移动性权重最高
    score += 400 * (myTerritory - oppTerritory);      // 领地控制
    score += 1 * (oppCompactness - myCompactness);  // 对手分散更好

    // 如果对手无路可走，给极高分
    if(oppMobility == 0) {
        score += 100000;
    }

    // 如果己方无路可走，给极低分
    if(myMobility == 0) {
        score -= 100000;
    }

    return score;
}
