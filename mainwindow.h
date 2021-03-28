#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "program.h"
#include <QFileDialog>
#include <QDebug>
#include <QSet>
#include "parser.h"
#include <QTextCursor>

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

private:
    Ui::MainWindow *ui;

    program prog;
    parser *myParser;
    evalstate *state;

    bool isWaitingForInput;      //表示现在是否在因为INPUT语句而等待用户输入的状态
    QString inputVar;       //INPUT等待用户输入的变量名
    QMap<int, statement*>::iterator itToINPUT;       //指向当前等待的INPUT语句的迭代器

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
    void PrintSyntaxTree();
};

#endif // MAINWINDOW_H
