#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QString>

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);
    bool moveAvailable(int x_now, int y_now, int x_to, int y_to) const;

    int turn = 2; //到谁了
    void nextTurn() {
        turn = 3 - turn;
        emit turnChanged(turn);    // 发信号告诉 MainWindow
        update();
    }
    void showGameOverDialog(QWidget *parent, const QString &winnerText, std::function<void()> onRestart);
    void saveGame(const QString &filename);
    void readGame(const QString &filename);
    void modeSet();
    void undomove();
    void reset();
    struct AIMove {
        int qx, qy;  // queen original
        int mx, my;  // queen moved
        int ax, ay;  // arrow
        int score;
    };
    AIMove findBestMove(int depth);
    int minimax(int depth, int alpha, int beta, bool maximizingPlayer);
    int evaluateBoard();
    std::vector<AIMove> generateAllMoves(int player);
    void applyMove(const AIMove &m);
    void undoMove(const AIMove &m, int prevTargetCell);
    bool isGameOver(int player);
    int localMobility(int x,int y);
    int arrowBlock(int ax,int ay,int opponent);
    int centerControl(int x, int y);
    int quickHeuristic(const AIMove &m, int player);
    int alphaBeta(int depth, int alpha, int beta, int player);
    int quickEvaluate(int player);
    int getDynamicDepth();
    int calculateTerritory(int x,int y,int player);
    int positionValue(int x, int y);
    int distanceToNearestAlly(int x, int y, int player);
    int advancedEvaluate(int player);
    int getAggressiveness(int x, int y, int opponent);
    std::pair<int,int> calculateTerritoriesGlobal();
    /*
    // Zobrist & TT
    void initZobrist();
    uint64_t computeZobristHash() const;
    struct TTEntry {
        int value;
        int depth;
        uint8_t flag; // 0 = exact, 1 = lowerbound, 2 = upperbound
        int sx, sy, mx, my, ax, ay;
    };
    std::unordered_map<uint64_t, TTEntry> tt;

    // AI 搜索/工具
    int mobility(int player);
    int territory(int player); // flood-fill based territory estimate
    int evaluate(int player);

    struct AIMove {
        int sx, sy;
        int mx, my;
        int ax, ay;
        int score;
    };

    void generateMoves(int player, std::vector<AIMove>& moves);
    void makeAIMove(const AIMove& m);
    void undoAIMove(const AIMove& m);
    int probeTT(uint64_t hash, int depth, int alpha, int beta, AIMove* best);
    void storeTT(uint64_t key, int value, int depth, uint8_t flag, const AIMove& best);
    AIMove computeBestMove(int player);  // 只计算，不修改棋盘
    void applyMove(const AIMove& move);  // 执行走法

    // 搜索
    int negamax(int depth, int alpha, int beta, int player, int ply);
    void initAI(); // 初始化随机种子、Zobrist、TT、killer/history
    void aiMove(); // 主入口（带迭代加深）
    */

    void aiMove();
    int getAIType() const { return aiType; }
    void drawArrow(QPainter &painter, int row, int col, int cellWidth, int cellHeight);
    void exit();
    void win();
    int gameMode = 0;
    int lastMoveX = -1;    // 记录最新移动的棋子
    int lastMoveY = -1;
    int lastArrowX = -1;    // 记录最新移动的箭
    int lastArrowY = -1;
    int aiType = 0;

private:
    int board[10][10];
    int selectedRow = -1; //鼠标控制中选中的行
    int selectedCol = -1; //列
    int toShoot = 0; //射箭吗



signals:
    void turnChanged(int newTurn);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

};

#endif // CHESSBOARD_H
