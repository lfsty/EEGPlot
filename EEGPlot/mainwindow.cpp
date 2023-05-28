#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define FREQ 1000
#define CHNUM 8
#define XRANGES 5
#define YRANGE 20
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->m_widget_eegplot, &EEGPlot::paintFPSUpdate, this, [ = ](int fps)
    {
        QString _data = QString("fps:%1").arg(fps);
        ui->m_label_opengl_paint_fps->setText(_data);
    });
    connect(ui->m_widget_eegplot, &EEGPlot::addDataFPSUpdate, this, [ = ](int fps)
    {
        QString _data = QString("freq:%1").arg(fps);
        ui->m_label_opengl_adddata_fps->setText(_data);
    });

    ui->m_widget_eegplot->initPaintData(DispMode::Cycle, XRANGES, CHNUM, FREQ, YRANGE);


    {
        connect(this, &MainWindow::sig_gen_data, ui->m_widget_eegplot, &EEGPlot::AddData);

        QTimer* _timer_gen_data = new QTimer();
        QThread* _thread_gen_data = new QThread();

        _timer_gen_data->moveToThread(_thread_gen_data);
        _timer_gen_data->setTimerType(Qt::PreciseTimer);
        _timer_gen_data->setInterval(1000 / FREQ);
        connect(_timer_gen_data, &QTimer::timeout, this, [ = ]()
        {
            static double _range_y = 10;
            static int _x = 0;
            QVector<double> _tmp(CHNUM);

            for(int i = 0; i < CHNUM; i++)
            {
                _tmp[i] = QRandomGenerator::global()->bounded(_range_y) - _range_y / 2;
            }
            emit sig_gen_data(_tmp, double(_x++) / FREQ);
        });
        connect(this, &MainWindow::destroyed, this, [ = ]()
        {
            _thread_gen_data->quit();
            _thread_gen_data->wait();
            _thread_gen_data->deleteLater();
            _timer_gen_data->deleteLater();
        });

        connect(_thread_gen_data, &QThread::started, _timer_gen_data, static_cast<void (QTimer::*)()>(&QTimer::start));
        connect(_thread_gen_data, &QThread::finished, _timer_gen_data, &QTimer::stop);
        _thread_gen_data->start();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

