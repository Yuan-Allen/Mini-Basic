#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "program.h"
#include <QFileDialog>
#include <QDebug>
#include <QSet>
#include "parser.h"
#include <QTextCursor>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class printStmt;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void handleLine(QString line);
private slots:
    void on_btnLoadCode_clicked();

    void on_btnRunCode_clicked();

    void on_btnClearCode_clicked();

    void on_btnDebug_clicked();

private:
    Ui::MainWindow *ui;

    program prog;
    parser *myParser;
    evalstate *state;

    bool isWaitingForInput;      //表示现在是否在因为INPUT语句而等待用户输入的状态
    bool waitingString;     //true表示等待输入的是字符串，false表示是数字
    QString inputVar;       //INPUT等待用户输入的变量名
    QMap<int, statement*>::iterator itToINPUT;       //指向当前等待的INPUT语句的迭代器
    bool debugMode; //debug模式中？
    QMap<int, statement*>::iterator stepIt; //debug模式下指向下一步要执行的statement的迭代器
    bool runningDebugger;   //在debug模式中直接运行而不单步调试？（若如此，INPUT和INPUTS结束回到程序后也会直接继续运行下去而不是停住）

    bool isCmd(QString line);
    void codeDisplayUpdate();
    void resultDisplayUpdate(QString text);
    void runCmd(QString line);  //执行RUN、LOAD等只能在命令行输入的立即执行语句
    void immExcute(statement* stmt);    //执行不带行号立即执行的LET、PRINT、INPUT指令
    void load();
    void run();         //用户直接调用的RUN是这个，从头开始调用privateRun
    void clear();
    void help();
    void quit();
    void runAfterInput(QList<QString> tokens);   //input输入后程序继续执行，从INPUT后面开始调用privateRun
    void privateRun(QMap<int, statement*>::iterator it);    //RUN的核心逻辑，run和runAfterInput都会调用这个函数
    void printSyntaxTree();
    void beforeRun();   //parse所有代码，生成statement并高亮错误代码
    void beforeDebug();
    bool stepDebug();   //单步调试
    void exitDebug(QString imformation);    //设置debugMode为false, 弹框输出imformation，退出debug模式
    void varDisplayUpdate();    //更新显示变量
};

#endif // MAINWINDOW_H
