#pragma once
#include <QThread>
#include "chessboard.h"

class AIThread : public QThread {
    Q_OBJECT
public:
    AIThread(ChessBoard* board, int aiType, QObject* parent = nullptr);

signals:
    //void moveReady(ChessBoard::AIMove bestMove);  // 计算完成后发信号

protected:
    void run() override;

private:
    ChessBoard* m_board;
    int m_aiType;
};
