#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFileInfo>
#include <QFileDialog>

#include "TokenParser.h"
#include "GrammarParser.h"
#include "CMMCore.h"

const QString MainWindow::filter(".cmm");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    filePath = "./untitled.cmm";
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::save(){
    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
           QMessageBox::warning(this, tr("CMMIde Error"),
                       tr("无法写入文件 %1：/n %2")
                      .arg(filePath).arg(file.errorString()));
           return false;
       }

    QTextStream out(&file);
    // 鼠标指针变为等待状态
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->sourceEdit->toPlainText();
    QApplication::restoreOverrideCursor();
    filePath = QFileInfo(filePath).canonicalFilePath();
    setWindowTitle(filePath);

    return true;
}

bool MainWindow::saveAs(){
    filePath = QFileDialog::getSaveFileName(this, tr("另存为"),filePath, QString("CMM Files(*.cmm)")).append(filter);
    if (filePath.isEmpty()) return false;
    return save();
}

bool MainWindow::open(QString _filePath){
    QFile file(_filePath); // 新建QFile对象
       if (!file.open(QFile::ReadOnly | QFile::Text)) {
           QMessageBox::warning(this, tr("CMMIde Error"),
                                 tr("无法读取文件 %1:\n%2.")
                                 .arg(_filePath).arg(file.errorString()));
           return false; // 只读方式打开文件，出错则提示，并返回false
       }
       QTextStream in(&file); // 新建文本流对象
       QApplication::setOverrideCursor(Qt::WaitCursor);
       // 读取文件的全部文本内容，并添加到编辑器中
       ui->sourceEdit->setPlainText(in.readAll());
       QApplication::restoreOverrideCursor();

       // 设置当前文件
       filePath = _filePath;
       setWindowTitle(filePath);
       return true;
}

void MainWindow::on_action_Save_triggered()
{
    if(filePath == ""){
        saveAs();
    }
    else{
        save();
    }
}

void MainWindow::on_action_New_triggered()
{
    ui->sourceEdit->document()->clear();
}


void MainWindow::on_action_Open_2_triggered()
{
    QString _filePath = QFileDialog::getOpenFileName(this, tr("打开"),filePath, QString("CMM Files(*.cmm)"));
    if (_filePath.isEmpty()) return;
    open(_filePath);
}

void MainWindow::on_action_Save_As_triggered()
{
    saveAs();
}

void MainWindow::on_action_Exit_triggered()
{
    this->close();
}

void MainWindow::on_action_Token_triggered()
{
    save();
    TokenParser parser(filePath.toLatin1().data());
    QString result;
    while(parser.ReadNext()){
        result.append(parser.GetLine())
                .append(":")
                .append(parser.GetColumn())
                .append(" ")
                .append(TokenParser::TranslateType(parser.GetToken().type).c_str())
                .append("\n");
    }
    ui->resultText->setPlainText(result);
}

void MainWindow::on_action_Grammar_triggered()
{
    save();
    GrammarParser parser(filePath.toLatin1().data());
    parser.Start();
    ui->resultText->setPlainText(parser.GetResult().c_str());
}

void MainWindow::on_action_Quaternary_triggered()
{
    save();
    CMMCore cmm;
    std::string file = filePath.toLatin1().data();
    cmm.CompileFile(file);
    if(cmm.HasError()){
        ui->resultText->setPlainText(cmm.GetErrors().c_str());
    }
    else{
        ui->resultText->setPlainText(cmm.GetQuaternaries().c_str());
    }
}

void MainWindow::on_action_Run_triggered()
{
    save();
    CMMCore cmm;
    std::string file = filePath.toLatin1().data();
    cmm.CompileFile(file);
    cmm.RunResult();
}
