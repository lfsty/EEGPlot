#ifndef EEGPLOT_H
#define EEGPLOT_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QPainter>

#include "stdafx.h"

enum DispMode
{
    Cycle = 0,
    Append
};

class EEGPlot : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    class _EEGPlotData
    {
    public:
        _EEGPlotData();
        ~_EEGPlotData();
    public:
        const double getChYData(const int& chIndex, const int& dataIndex) const;
        const double getChXData(const int& dataIndex) const;
        void AddData(const double* ydata, const double& xdata, const int& length);
    public:
        void setEEGDataShape(const int& ch_capacity, const int& ch_num, const DispMode& save_mode);
        const int getChCapacity() const;
        const int getChNum() const;
        const int getSize() const;
    private:
        double** m_ydata = nullptr;
        double* m_xdata = nullptr;
        //头位置
        int m_earliestDataIndex = 0;
        int m_currentIndex = 0;
        //导联最大存储数据量
        int m_chCapacity = 0;
        //导联数量
        int m_chNum = 0;
        //已保存的数据量
        int m_chDataSize = 0;
        DispMode m_saveMode = DispMode::Cycle;
    private:
        void f_clearData();
    };

public:
    explicit EEGPlot(QWidget* parent = nullptr);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
public:
    void initPaintData(const DispMode& save_mode, const double& dispXRangeS, const int& ch_num, const int& freq, const double& y_range = 20);
    void AddData(const QVector<double>& yData, const double& xData);
private:
    void f_calFPS();
    void f_calAddDataFPS();
public:
    void setYRange(const double& y_range);
    void setChTickets(const QStringList& ch_tickets);
private:
    _EEGPlotData m_data;
    DispMode m_dispMode = DispMode::Cycle;
    double m_dispXRangeS = 5;
    int m_chNum = 0;
    int m_freq = 1000;
    double m_yRange = 20;
    QStringList m_chTickets;
private:
    //绘图相关
    double m_phy_singleChHeight;
    double m_phy_chDataBias;
    double m_phy_dataRatio;
    double m_phy_xRatio;


signals:
    void paintFPSUpdate(int fps);
    void addDataFPSUpdate(int fps);
};

#endif // EEGPLOT_H
