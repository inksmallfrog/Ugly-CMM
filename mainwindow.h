#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    bool save();
    bool saveAs();
    bool open(QString _filePath);

private slots:

    void on_action_Save_triggered();

    void on_action_New_triggered();

    void on_action_Open_2_triggered();

    void on_action_Save_As_triggered();

    void on_action_Exit_triggered();

    void on_action_Token_triggered();

    void on_action_Grammar_triggered();

    void on_action_Quaternary_triggered();

    void on_action_Run_triggered();

private:
    Ui::MainWindow *ui;
    QString filePath;

    static const QString filter;
};

#endif // MAINWINDOW_H
