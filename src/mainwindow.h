#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QByteArray>
#include <QCryptographicHash>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    QByteArray processDES(const QByteArray &data, QByteArray key, bool encrypt);
    QByteArray initialPermutation(const QByteArray &block);
    QByteArray finalPermutation(const QByteArray &block);
    QByteArray feistelFunction(const QByteArray &halfBlock, const QByteArray &key);
};
#endif // MAINWINDOW_H
