// GameDialog.h
#ifndef GAMEDIALOG_H
#define GAMEDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <functional>

class GameDialog : public QDialog
{
    Q_OBJECT
public:
    // parent: 父窗口，text: 显示内容，onYes/onNo: 点击按钮回调
    GameDialog(QWidget *parent, const QString &text,
               const QString &yesText, const QString &noText,
               std::function<void()> onYes,
               std::function<void()> onNo)
        : QDialog(parent)
    {
        setWindowTitle("提示");
        setModal(true);                // 模态窗口
        setFixedSize(300,150);         // 固定大小
        setStyleSheet("background-color: #f0f0f0;"); // 背景色

        QLabel *label = new QLabel(text, this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 16px; font-weight: bold;");

        QPushButton *btnYes = new QPushButton(yesText, this);
        btnYes->setStyleSheet("background-color: #4caf50; color: white; padding: 6px;");

        QPushButton *btnNo = new QPushButton(noText, this);
        btnNo->setStyleSheet("background-color: #f44336; color: white; padding: 6px;");

        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addWidget(btnYes);
        btnLayout->addWidget(btnNo);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(label);
        mainLayout->addLayout(btnLayout);

        // 点击按钮
        connect(btnYes, &QPushButton::clicked, [=]() {
            if(onYes) onYes();
            accept();  // 关闭对话框
        });
        connect(btnNo, &QPushButton::clicked, [=]() {
            if(onNo) onNo();
            reject();  // 关闭对话框
        });
    }
};

#endif // GAMEDIALOG_H
