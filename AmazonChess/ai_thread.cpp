#include "ai_thread.h"

AIThread::AIThread(ChessBoard* board, int aiType, QObject* parent)
    : QThread(parent), m_board(board), m_aiType(aiType) {}

void AIThread::run()
{
    if(!m_board) return;

    // 调用 ChessBoard 的函数计算最佳走法（不要直接修改棋盘）
    //ChessBoard::AIMove bestMove = m_board->computeBestMove(m_aiType);
    //emit moveReady(bestMove);
}
