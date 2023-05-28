#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QThread>
#include <QTimer>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void sig_gen_data(const QVector<double>&, const double&);
private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
