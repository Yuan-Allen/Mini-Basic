#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->installEventFilter(this);

    state = new evalstate;
    myParser = new parser(state);

    isWaitingForInput=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->textEdit) {
        if(event->type()==QEvent::KeyPress) {
            QKeyEvent *e = static_cast<QKeyEvent *>(event);
            if(e->key()==Qt::Key_Return||e->key()==Qt::Key_Enter) {
                QString text=ui->textEdit->toPlainText();
                ui->textEdit->clear();
                if(text=="") return true;   //跳过空行
                try {
                    handleLine(text);
                } catch (QString error) {
                    ui->resultDisplay->setText(error);
                }
                return true;
            }

        }
        return false;
    }
    return QWidget::eventFilter(watched, event);
}

void MainWindow::handleLine(QString line)
{
    line = line.simplified();
    QList<QString> list = line.split(' ',QString::SkipEmptyParts);
    if(isWaitingForInput) {
        runAfterInput(list);
        return;
    }

    if(isCmd(line)) {
        runCmd(line);
        return;
    }

    bool ok;
    int lineNum = 0;
    lineNum = list[0].toInt(&ok);
    if(ok) {
        line.remove(0, line.indexOf(' ')+1);
        if(lineNum<=0) {
            QString error("Illegal Line Number! A Line Number should be above 0.");
            throw error;
        }
        if(list.length()==1) {      //line里面只有一个数字，是删除命令。
            prog.removeStatement(lineNum);
        }
        else {
            if(lineNum>1000000) {
                QString error("The Line Number Can't Be Above 1000000!");
                throw error;
            }
            prog.addNewStmtString(lineNum, line);
            prog.addNewStatement(lineNum, myParser->parse(line));
        }
        codeDisplayUpdate();
    }
    else {
        statement *stmt = myParser->parse(line);
        immExcute(stmt);
    }
    itToINPUT=prog.stmts.end();
}

void MainWindow::codeDisplayUpdate()
{
    ui->CodeDisplay->clear();
    QMap<int, QString>::iterator it = prog.stmtStrings.begin();
    for(; it!=prog.stmtStrings.end(); ++it)
        ui->CodeDisplay->append(QString::number(it.key())+' '+it.value());
}

void MainWindow::resultDisplayUpdate(QString text)
{
    ui->resultDisplay->append(text);
}

bool MainWindow::isCmd(QString line)
{
    QSet<QString> cmdSet;
    cmdSet<<"RUN"<<"LOAD"<<"CLEAR"<<"HELP"<<"QUIT";
    if(cmdSet.contains(line))
        return true;
    return false;
}

void MainWindow::runCmd(QString line)
{
    itToINPUT=prog.stmts.end();
    if(line=="LOAD") {
        load();
        return;
    }
    if(line=="RUN") {
        run();
        return;
    }
    if(line=="CLEAR") {
        clear();
        return;
    }
    if(line=="HELP") {
        help();
        return;
    }
    if(line=="QUIT") {
        quit();
        return;
    }
}

void MainWindow::immExcute(statement* stmt)
{
    QString tmpString;
    tmpString=stmt->toSyntaxTree();
    ui->treeDisplay->setText(tmpString);

    int result = stmt->excute();
    if(result==0)   return;
    if(result==-1) {  //-1表示是print语句，需要更新显示
        resultDisplayUpdate(QString::number(static_cast<printStmt*>(stmt)->getValue()));
        return;
    }
    if(result==-2) {  //-2表示是input语句
        isWaitingForInput=true;
        inputVar = static_cast<inputStmt*>(stmt)->getVarName();
        ui->textEdit->setText(" ? ");
        ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        return;
    }
}

void MainWindow::load()
{
    if(isWaitingForInput) {
        QString error("You Should Input A Value!");
        throw error;
    }

    QString fileName = QFileDialog::getOpenFileName(this);
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        state->clear();
        QTextStream in(&file);
        while (!in.atEnd()) {
            handleLine(in.readLine());
        }
    }
    else qDebug()<<"Open file failed."<<endl;
}

void MainWindow::run()
{
    if(isWaitingForInput) {
        QString error("You Should Input A Value!");
        throw error;
    }

    ui->resultDisplay->clear();
    QMap<int, statement*>::iterator it = prog.stmts.begin();
    privateRun(it);
}

void MainWindow::clear()
{
    isWaitingForInput=false;
    prog.clear();
    state->clear();
    codeDisplayUpdate();
    ui->textEdit->clear();
}

void MainWindow::help()
{
    ui->resultDisplay->setText(QString("RUN    Run the program.\n"
                                       "LOAD   Load a flie containing statements and commands.\n"
                                       "CLEAR  Delete the program.\n"
                                       "HELP   The command you just entered :)\n"
                                       "QUIT   Exit the BASIC interpreter."));
}

void MainWindow::quit()
{
    this->close();
}

void MainWindow::runAfterInput(QList<QString> tokens)
{
    if(tokens.length()!=2||tokens[0]!="?") {
        ui->textEdit->setText(" ? ");
        ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        QString error("Illegal Input!\nYou Should Input A Number After ' ? '.");
        throw error;
    }

    bool ok;
    int value = tokens[1].toInt(&ok);
    if(ok) {
        state->setValue(inputVar, value);
        isWaitingForInput=false;
        if(itToINPUT!=prog.stmts.end())
            privateRun(++itToINPUT);
    }
    else {
        ui->textEdit->setText(" ? ");
        ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        QString error("Illegal Input!\nYou Should Input A Number After '?'.");
        throw error;
    }
}

void MainWindow::privateRun(QMap<int, statement*>::iterator it)
{
    while (it!=prog.stmts.end()) {
        int result = it.value()->excute();
        if(result==0) {   //0表示应该执行下一句指令
            it++;
            continue;
        }
        else if(result==-1) {  //-1表示是print语句，需要更新显示
            resultDisplayUpdate(QString::number(static_cast<printStmt*>(it.value())->getValue()));
            it++;
            continue;
        }
        else if(result==-2) {
            isWaitingForInput=true;
            inputVar = static_cast<inputStmt*>(it.value())->getVarName();
            itToINPUT = it;
            ui->textEdit->setText(" ? ");
            ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            return;
        }
        else if(result>0) {    //大于0的返回值是下一句要执行的statement的行号
            if((it = prog.stmts.find(it.value()->excute()))==prog.stmts.end()) {
                QString error("Illegal GOTO Statement! The Line Number Doesn't Exist.");
                throw error;
            }
            else continue;
        }
        else if (result==-3) {
            break;
        }
    }

    PrintSyntaxTree();

    itToINPUT=prog.stmts.end();
}

void MainWindow::PrintSyntaxTree()
{
    ui->treeDisplay->clear();
    QString tmpString;
    QMap<int, statement*>::iterator it = prog.stmts.begin();
    while (it!=prog.stmts.end()) {
        tmpString=it.value()->toSyntaxTree();
        tmpString.prepend(QString::number(it.key())+QString(" "));
        ui->treeDisplay->append(tmpString);
        tmpString.clear();
        it++;
    }
}

void MainWindow::on_btnLoadCode_clicked()
{
    try {
        load();
    } catch (QString error) {
        ui->resultDisplay->setText(error);
    }
}

void MainWindow::on_btnRunCode_clicked()
{
    try {
        run();
    } catch (QString error) {
        ui->resultDisplay->setText(error);
    }
}

void MainWindow::on_btnClearCode_clicked()
{
    clear();
}
