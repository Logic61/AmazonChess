#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);
    bool moveAvailable(int x_now, int y_now, int x_to, int y_to) const;

    int turn = 1; //到谁了
    void nextTurn() {
        turn = 3 - turn;
        emit turnChanged(turn);    // 发信号告诉 MainWindow
        update();
    }
    void showGameOverDialog(QWidget *parent, const QString &winnerText, std::function<void()> onRestart);
    void modeSet();
    void undomove();
    void reset();

private:
    int board[10][10];
    int selectedRow = -1; //鼠标控制中选中的行
    int selectedCol = -1; //列
    int toShoot = 0; //射箭吗

    void win();

signals:
    void turnChanged(int newTurn);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

};

#endif // CHESSBOARD_H
