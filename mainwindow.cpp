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

    isWaitingForInput = false;
    waitingString = false;
    debugMode = false;
    runningDebugger = false;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete state;
    delete myParser;
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
    if(debugMode) {
        QString error("Debugger Running!\nIllegal Input.");
        throw error;
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
            //prog.addNewStatement(lineNum, myParser->parse(line));
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
    cmdSet<<"RUN"<<"LOAD"<<"CLEAR"<<"HELP"<<"QUIT"<<"LIST";
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
    if(line=="LIST") {
        return;
    }
}

void MainWindow::immExcute(statement* stmt)
{
    if(!stmt) {
        QString error("Illegal Statement!\n");
        throw (error);
    }

    QString tmpString;
    tmpString=stmt->toSyntaxTree();
    ui->treeDisplay->setText(tmpString);

    int result = stmt->excute();
    if(result==0){

    }
    else if(result==-1) {  //-1表示是print语句，需要更新显示
            if(stmt->getType()==PRINT)
                resultDisplayUpdate(QString::number(static_cast<printStmt*>(stmt)->getValue()));
            else {
                resultDisplayUpdate(static_cast<printfStmt*>(stmt)->getText());
            }
    }
    else if(result==-2) {  //-2表示是input语句
            waitingString = (stmt->getType()==INPUTS);
            isWaitingForInput=true;
            inputVar = static_cast<inputStmt*>(stmt)->getVarName();
            ui->textEdit->setText(" ? ");
            ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            ui->btnDebug->setEnabled(false);
    }
    varDisplayUpdate();
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
        prog.clear();
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

    //ui->resultDisplay->clear();
    //printSyntaxTree();

    QMap<int, statement*>::iterator it;
    if(!debugMode) {
        beforeRun();
        it = prog.stmts.begin();
        printSyntaxTree();
        privateRun(it);
    }
    else {
        runningDebugger = true;
        while (stepDebug()) {}
        //exitDebug("Program Ran Succesfullly!\nNormal Exit.");
    }
}

void MainWindow::clear()
{
    isWaitingForInput=false;
    prog.clear();
    state->clear();
    codeDisplayUpdate();
    varDisplayUpdate();
    ui->textEdit->clear();
    ui->resultDisplay->clear();
    ui->treeDisplay->clear();
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

    QString tmpS;
    for (int i=1;i<tokens.size();++i) {
        tmpS+=tokens[i];
    }
    if(!waitingString) {
        bool ok;
        int value = tmpS.toInt(&ok);
        if(ok) {
            state->setValue(inputVar, value);
            isWaitingForInput=false;
            ui->btnDebug->setEnabled(true);
            varDisplayUpdate();
            if(runningDebugger)
                run();
            else
                if(itToINPUT!=prog.stmts.end()&&!debugMode)
                    privateRun(++itToINPUT);
        }
        else {
            ui->textEdit->setText(" ? ");
            ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            QString error("Illegal Input!\nYou Should Input A Number After '?'.");
            throw error;
        }
    }
    else {
        if(tmpS.contains('\'')||tmpS.contains('\"')) {
            ui->textEdit->setText(" ? ");
            ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            QString error("Illegal Input!\nString should not contains ' or \"");
            throw error;
        }
        state->setValue(inputVar, tokens[1]);
        isWaitingForInput=false;
        ui->btnDebug->setEnabled(true);
        varDisplayUpdate();
        if(runningDebugger)
            run();
        else
            if(itToINPUT!=prog.stmts.end()&&!debugMode)
                privateRun(++itToINPUT);
    }
}

void MainWindow::privateRun(QMap<int, statement*>::iterator it)
{
    while (it!=prog.stmts.end()) {
        if(!it.value()) { //不合法的statement会被存为空指针，
            QMessageBox::information(this, "Error", "Illegal Statement!");
            break;
        }
        int result = it.value()->excute();
        if(result==0) {   //0表示应该执行下一句指令
            it++;
            continue;
        }
        else if(result==-1) {  //-1表示是print或printf语句，需要更新显示
            if(it.value()->getType()==PRINT)
                resultDisplayUpdate(QString::number(static_cast<printStmt*>(it.value())->getValue()));
            else {
                resultDisplayUpdate(static_cast<printfStmt*>(it.value())->getText());
            }
            it++;
            continue;
        }
        else if(result==-2) {
            waitingString = (it.value()->getType()==INPUTS);
            isWaitingForInput=true;
            inputVar = static_cast<inputStmt*>(it.value())->getVarName();
            itToINPUT = it;
            ui->textEdit->setText(" ? ");
            ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            ui->btnDebug->setEnabled(false);
            varDisplayUpdate();
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
    varDisplayUpdate();
    //PrintSyntaxTree();

    itToINPUT=prog.stmts.end();
}

void MainWindow::printSyntaxTree()
{
    ui->treeDisplay->clear();
    QString tmpString;
    QMap<int, QString>::iterator it = prog.stmtStrings.begin();
    while (it!=prog.stmtStrings.end()) {
        if(prog.stmts.value(it.key()))
            tmpString=prog.stmts.value(it.key())->toSyntaxTree();
        else tmpString = "Error\n";
        tmpString.prepend(QString::number(it.key())+QString(" "));
        ui->treeDisplay->append(tmpString);
        tmpString.clear();
        it++;
    }
}

void MainWindow::beforeRun()
{
    // 获取需要高亮的 TextEdit 对象（QTextBrowser 是 QTextEdit 的只读子类）
    QTextBrowser *code = ui->CodeDisplay;
    QTextCursor cursor(code->document());

    // 用于维护的所有高亮的链表
    QList<QTextEdit::ExtraSelection> extras;
    QList<QPair<int, QColor>> highlights;

    prog.clearStmts();
    ui->resultDisplay->clear();
    //ui->treeDisplay->clear();
    //QString tmpString;
    statement *stmtP = nullptr;
    cursor.setPosition(QTextCursor::Start);
    for (QMap<int, QString>::iterator it=prog.stmtStrings.begin();it!=prog.stmtStrings.end();++it) {
        stmtP = myParser->parse(it.value());
//        if(stmtP) {
//            //prog.addNewStatement(it.key(), stmtP);
//            tmpString = stmtP->toSyntaxTree();
//        }
//        else {
//            tmpString = "Error\n";
//            highlights.push_back(QPair<int, QColor>(cursor.position(), QColor(255, 100, 100)));
//        }
        if(!stmtP) {
            highlights.push_back(QPair<int, QColor>(cursor.position(), QColor(255, 100, 100)));
        }
        prog.addNewStatement(it.key(), stmtP);
        //tmpString.prepend(QString::number(it.key())+QString(" "));
        //ui->treeDisplay->append(tmpString);
        //tmpString.clear();
        cursor.movePosition(QTextCursor::Down);
    }

    // 配置高亮，并加入到 extras 中
    for (auto &line : highlights) {
        QTextEdit::ExtraSelection h;
        h.cursor = cursor;
        // 下面这些的功能，请大家自行查看文档
        h.cursor.setPosition(line.first);
        h.cursor.movePosition(QTextCursor::StartOfLine);
        h.cursor.movePosition(QTextCursor::EndOfLine);
        h.format.setProperty(QTextFormat::FullWidthSelection, true);
        h.format.setBackground(line.second);
        extras.append(h);
    }
    // 应用高亮效果
    code->setExtraSelections(extras);

}

void MainWindow::beforeDebug()
{
    ui->treeDisplay->clear();

    // 获取需要高亮的 TextEdit 对象（QTextBrowser 是 QTextEdit 的只读子类）
    QTextBrowser *code = ui->CodeDisplay;
    QTextCursor cursor(code->document());

    // 用于维护的所有高亮的链表
    QList<QTextEdit::ExtraSelection> extras;
    QList<QPair<int, QColor>> highlights;
    cursor.setPosition(QTextCursor::Start);

    prog.clearStmts();
    ui->resultDisplay->clear();
    statement *stmtP = nullptr;
    cursor.setPosition(QTextCursor::Start);
    for (QMap<int, QString>::iterator it=prog.stmtStrings.begin();it!=prog.stmtStrings.end();++it) {
        stmtP = myParser->parse(it.value());
        if(!stmtP) {
            highlights.push_back(QPair<int, QColor>(cursor.position(), QColor(255, 100, 100)));
        }
        prog.addNewStatement(it.key(), stmtP);
        cursor.movePosition(QTextCursor::Down);
    }
    highlights.push_back(QPair<int, QColor>(1, QColor(100, 255, 100))); //绿色高亮第一行代码

    // 配置高亮，并加入到 extras 中
    for (auto &line : highlights) {
        QTextEdit::ExtraSelection h;
        h.cursor = cursor;
        // 下面这些的功能，请大家自行查看文档
        h.cursor.setPosition(line.first);
        h.cursor.movePosition(QTextCursor::StartOfLine);
        h.cursor.movePosition(QTextCursor::EndOfLine);
        h.format.setProperty(QTextFormat::FullWidthSelection, true);
        h.format.setBackground(line.second);
        extras.append(h);
    }
    // 应用高亮效果
    code->setExtraSelections(extras);
    stepIt = prog.stmts.begin();
    if(stepIt == prog.stmts.end())
        return;
    QString tmpString;
    if(prog.stmts.value(stepIt.key()))
        tmpString=prog.stmts.value(stepIt.key())->toSyntaxTree();
    else tmpString = "Error\n";
    tmpString.prepend(QString::number(stepIt.key())+QString(" "));
    ui->treeDisplay->setText(tmpString);
}

bool MainWindow::stepDebug()
{
    // 获取需要高亮的 TextEdit 对象（QTextBrowser 是 QTextEdit 的只读子类）
    QTextBrowser *code = ui->CodeDisplay;
    QTextCursor cursor(code->document());

    // 用于维护的所有高亮的链表
    QList<QTextEdit::ExtraSelection> extras;
    QList<QPair<int, QColor>> highlights;

    cursor.setPosition(QTextCursor::Start);

    if(stepIt==prog.stmts.end()) {
        exitDebug("Program Ran Succesfullly!\nNormal Exit.");
        return false;
    }
    if(!stepIt.value()) { //不合法的statement会被存为空指针，
        exitDebug("Illegal Statement!");
        return false;
    }
    int result = stepIt.value()->excute();
    if(result==0) {   //0表示应该执行下一句指令
        stepIt++;
    }
    else if(result==-1) {  //-1表示是print或printf语句，需要更新显示
        if(stepIt.value()->getType()==PRINT)
            resultDisplayUpdate(QString::number(static_cast<printStmt*>(stepIt.value())->getValue()));
        else {
            resultDisplayUpdate(static_cast<printfStmt*>(stepIt.value())->getText());
        }
        stepIt++;
    }
    else if(result==-2) {
        waitingString = (stepIt.value()->getType()==INPUTS);
        isWaitingForInput=true;
        inputVar = static_cast<inputStmt*>(stepIt.value())->getVarName();
        itToINPUT = stepIt;
        ui->textEdit->setText(" ? ");
        ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->btnDebug->setEnabled(false);
        stepIt++;
    }
    else if(result>0) {    //大于0的返回值是下一句要执行的statement的行号
        if((stepIt = prog.stmts.find(stepIt.value()->excute()))==prog.stmts.end()) {
            exitDebug("Illegal GOTO Statement! The Line Number Doesn't Exist.");
            return false;
        }
    }
    else if (result==-3) {
        exitDebug("Program Ran Succesfullly!\nNormal Exit.");
        return false;
    }

    for (QMap<int ,statement*>::iterator tmpIt=prog.stmts.begin();tmpIt!=prog.stmts.end();++tmpIt) {
        if(!(tmpIt.value())) {
            highlights.push_back(QPair<int, QColor>(cursor.position(), QColor(255, 100, 100)));
        }
        if(tmpIt.key()==stepIt.key()) {
            highlights.push_back(QPair<int, QColor>(cursor.position(), QColor(100, 255, 100)));
        }
        cursor.movePosition(QTextCursor::Down);
    }

    // 配置高亮，并加入到 extras 中
    for (auto &line : highlights) {
        QTextEdit::ExtraSelection h;
        h.cursor = cursor;
        // 下面这些的功能，请大家自行查看文档
        h.cursor.setPosition(line.first);
        h.cursor.movePosition(QTextCursor::StartOfLine);
        h.cursor.movePosition(QTextCursor::EndOfLine);
        h.format.setProperty(QTextFormat::FullWidthSelection, true);
        h.format.setBackground(line.second);
        extras.append(h);
    }
    // 应用高亮效果
    code->setExtraSelections(extras);

    if(stepIt==prog.stmts.end())
        return true;
    QString tmpString;
    if(prog.stmts.value(stepIt.key()))
        tmpString=prog.stmts.value(stepIt.key())->toSyntaxTree();
    else tmpString = "Error\n";
    tmpString.prepend(QString::number(stepIt.key())+QString(" "));
    ui->treeDisplay->setText(tmpString);
    varDisplayUpdate();
    if(result==-2)
        return false;
    else return true;
}

void MainWindow::exitDebug(QString imformation)
{
    QMessageBox::information(this, "Debug Information", imformation);
    ui->btnLoadCode->setEnabled(true);
    ui->btnClearCode->setEnabled(true);
    debugMode = false;
    runningDebugger = false;
}

void MainWindow::varDisplayUpdate()
{
    ui->varibleDisplay->clear();
    for (QMap<QString, QString>::iterator it=state->stringSymTable.begin();it!=state->stringSymTable.end();++it) {
        ui->varibleDisplay->append(it.key()+": STR = \""+it.value()+'\"');
    }
    for (QMap<QString, int>::iterator it=state->symbolTable.begin();it!=state->symbolTable.end();++it) {
        ui->varibleDisplay->append(it.key()+": INT = "+QString::number(it.value()));
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

void MainWindow::on_btnDebug_clicked()
{
    try {
        if(!debugMode) {
            debugMode = true;
            beforeDebug();
            ui->btnLoadCode->setEnabled(false);
            ui->btnClearCode->setEnabled(false);
        }
        else {
            stepDebug();
        }
    } catch (QString error) {
        exitDebug(error);
        return;
    }
}
