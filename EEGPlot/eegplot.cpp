#include "eegplot.h"
EEGPlot::EEGPlot(QWidget* parent)
    : QOpenGLWidget{parent}
{

}

void EEGPlot::initializeGL()
{
    //调用内容初始化函数
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  //设置窗体背景色
}

void EEGPlot::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0, 0.0, 0.0);


    for(int i = 0; i < m_chNum; i++)
    {
        for(int j = 0; j < m_data.getSize() - 1; j++)
        {
            glLineWidth(1);
            glBegin(GL_LINES);//画线
            glVertex2d(j * m_phy_xRatio, m_data.getChYData(i, j)*m_phy_dataRatio + m_phy_chDataBias + m_phy_singleChHeight * i); //开始点(x,y)
            glVertex2d((j + 1)*m_phy_xRatio, m_data.getChYData(i, j + 1)*m_phy_dataRatio + m_phy_chDataBias + m_phy_singleChHeight * i); //结束点 (x,y)
            glEnd();
        }
    }



    //    QPainter painter(this);
    //    QRect _text_rect = painter.fontMetrics().boundingRect("hello world");
    //    auto _rect = QRect(50, 50, rect.width(), rect.height());
    //    painter.drawRect(_rect);
    //    painter.drawText(_rect, Qt::AlignRight, "hello world");

    f_calFPS();
    this->update();
}

void EEGPlot::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glLoadIdentity();//重置投影矩阵
    glOrtho(0.0, w, h, 0.0, -1.0, 1.0); //这个代表截取屏幕

    if(m_chNum != 0)
    {
        m_phy_singleChHeight = double(h) / m_chNum;
        m_phy_chDataBias = m_phy_singleChHeight / 2;
        m_phy_dataRatio = m_phy_singleChHeight / m_yRange;
        m_phy_xRatio = double(this->width()) / (m_dispXRangeS * m_freq);
    }
}

void EEGPlot::initPaintData(const DispMode& save_mode, const double& dispXRangeS, const int& ch_num, const int& freq, const double& y_range)
{
    assert(dispXRangeS > 0);
    assert(ch_num > 0);
    assert(freq > 0);
    assert(y_range > 0);

    m_dispMode = save_mode;
    m_dispXRangeS = dispXRangeS;
    m_chNum = ch_num;
    m_freq = freq;

    m_data.setEEGDataShape(int(m_dispXRangeS * m_freq), m_chNum, m_dispMode);
    m_phy_xRatio = double(this->width()) / (m_dispXRangeS * m_freq);

    setYRange(y_range);
}

void EEGPlot::AddData(const QVector<double>& yData, const double& xData)
{
    assert(yData.size() == m_chNum);
    m_data.AddData(yData.data(), xData, yData.size());
    f_calAddDataFPS();
}

void EEGPlot::f_calFPS()
{
    static int _frames  = 0;       // 用于存储渲染的帧数
    static long _lastTime   = clock();       // 前一秒的时刻
    long _currentTime =  clock();
    _frames++;
    if(_currentTime - _lastTime >= CLOCKS_PER_SEC)
    {
        emit paintFPSUpdate(_frames);
        _lastTime = _currentTime;
        _frames = 0;
    }
}

void EEGPlot::f_calAddDataFPS()
{
    static int _frames  = 0;       // 用于存储渲染的帧数
    static long _lastTime   = clock();       // 前一秒的时刻
    long _currentTime =  clock();
    _frames++;
    if(_currentTime - _lastTime >= CLOCKS_PER_SEC)
    {
        emit addDataFPSUpdate(_frames);
        _lastTime = _currentTime;
        _frames = 0;
    }
}

void EEGPlot::setYRange(const double& y_range)
{
    m_yRange = y_range;
    m_phy_dataRatio = m_phy_singleChHeight / m_yRange;
}

void EEGPlot::setChTickets(const QStringList& ch_tickets)
{
    m_chTickets = ch_tickets;
}

EEGPlot::_EEGPlotData::_EEGPlotData()
{

}

EEGPlot::_EEGPlotData::~_EEGPlotData()
{
    f_clearData();
}

const double EEGPlot::_EEGPlotData::getChYData(const int& chIndex, const int& dataIndex) const
{
    assert(m_ydata != nullptr);
    assert(chIndex < getChNum() && chIndex >= 0);
    assert(dataIndex < getChCapacity() && dataIndex >= 0);

    switch(m_saveMode)
    {
        case DispMode::Cycle:
            {
                return m_ydata[chIndex][dataIndex];
                break;
            }
        case DispMode::Append:
            {
                return m_ydata[chIndex][(m_earliestDataIndex + dataIndex) % getChCapacity()];
                break;
            }
    }
}

const double EEGPlot::_EEGPlotData::getChXData(const int& dataIndex) const
{
    assert(m_xdata != nullptr);
    assert(dataIndex < getChCapacity() && dataIndex >= 0);

    switch(m_saveMode)
    {
        case DispMode::Cycle:
            {
                return m_xdata[dataIndex];
                break;
            }
        case DispMode::Append:
            {
                return m_xdata[(m_earliestDataIndex + dataIndex) % getChCapacity()];
                break;
            }
    }
}

void EEGPlot::_EEGPlotData::AddData(const double* ydata, const double& xdata, const int& length)
{
    assert(m_ydata != nullptr);
    assert(m_xdata != nullptr);
    assert(length == getChNum());

    m_xdata[m_currentIndex] = xdata;
    for(int i = 0; i < getChNum(); i++)
    {
        m_ydata[i][m_currentIndex] = ydata[i];
    }

    m_currentIndex++;
    if(m_currentIndex >= getChCapacity())
    {
        m_currentIndex = 0;
    }
    //如果存满了，最早的数据的index开始随之移动
    if(m_chDataSize == getChCapacity())
    {
        m_earliestDataIndex = m_currentIndex + 1;
        if(m_earliestDataIndex >= getChCapacity())
        {
            m_earliestDataIndex = 0;
        }
    }

    if(m_chDataSize != getChCapacity())
        m_chDataSize++;

}

void EEGPlot::_EEGPlotData::setEEGDataShape(const int& ch_capacity, const int& ch_num, const DispMode& save_mode)
{
    f_clearData();
    m_chCapacity = ch_capacity;
    m_chNum = ch_num;
    m_saveMode = save_mode;

    m_ydata = new double*[m_chNum];
    for(int i = 0; i < m_chNum; i++)
    {
        m_ydata[i] = new double[m_chCapacity];
    }

    m_xdata = new double[m_chCapacity];
}

const int EEGPlot::_EEGPlotData::getChCapacity() const
{
    return m_chCapacity;
}

const int EEGPlot::_EEGPlotData::getChNum() const
{
    return m_chNum;
}

const int EEGPlot::_EEGPlotData::getSize() const
{
    return m_chDataSize;
}

void EEGPlot::_EEGPlotData::f_clearData()
{
    if(m_ydata != nullptr)
    {
        for(int i = 0; i < getChNum(); i++)
        {
            delete[] m_ydata[i];
        }
        delete[] m_ydata;
        m_ydata = nullptr;

        delete[] m_xdata;
        m_xdata = nullptr;

        m_chCapacity = 0;
        m_chNum = 0;
        m_currentIndex = 0;
        m_chDataSize = 0;
        m_earliestDataIndex = 0;
    }
}

