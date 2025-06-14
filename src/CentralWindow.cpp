#include "CentralWindow.hpp"

namespace CWD{

    Range::Range()
        :Left(0), Right(0)
    {}

    Range::Range(long long left, long long right)
        :Left(left), Right(right)
    {}

    Range::Range(const Range& range)
        :Left(range.Left), Right(range.Right)
    {}

    Range::Range(const TCOND::Range& range)
        :Left(range.GetLeft()), Right(range.GetRight())
    {}

    Range::~Range()
    {}

    void Range::operator=(const Range& range)
    {
        Left = range.Left;
        Right = range.Right;
    }

    void Range::SetRange(long long left, long long right)
    {
        Left = left;
        Right = right;
    }

    long long Range::GetLeft() const
    {
        return Left;
    }

    long long Range::GetRight() const
    {
        return Right;
    }

    //

    Item::Item():IfValid(false), OnLineStat(INITIAL_OFF_LINE), TestRes(), SinadRang(){}

    Item::Item(bool b, Range tr, Range sr):IfValid(b), OnLineStat(INITIAL_OFF_LINE), TestRes(tr), SinadRang(sr){}

    Item::Item(const Item& cp):IfValid(cp.IfValid), OnLineStat(cp.OnLineStat), TestRes(cp.TestRes), SinadRang(cp.SinadRang){}

    Item::~Item(){}

    void Item::operator=(const Item& as)
    {
        IfValid = as.IfValid;
        OnLineStat = as.OnLineStat;
        TestRes = as.TestRes;
        SinadRang = as.SinadRang;
    }

    void Item::RestOnLineStat()
    {
        OnLineStat = INITIAL_OFF_LINE;
    }

    void Item::Paint(QPainter* painter, const QStyleOptionViewItem& option, int row)
    {
        // WRITE_CENTRAL_WIDGET_DBG("Item::Paint(), Enter\n");
        if(row > 0)
        {

            painter->save();

            int Left = option.rect.left();
            int Top = option.rect.top();
            int Width = option.rect.width();
            int Height = option.rect.height();
            int CellWidth = Width / 8;

            // QStyleOptionProgressBar ProgressBarOption;
            // QPalette palette = ProgressBarOption.palette;

            QRect Rc[3] = {
                QRect(Left, Top, CellWidth - 3, Height - 2),
                QRect(Left + CellWidth, Top, CellWidth * 5 - 3, Height - 2),
                QRect(Left + CellWidth * 6, Top, CellWidth * 2 - 3, Height - 2)
            };
            // painter->setPen(QPen(Qt::black));
            QFont font("Microsoft YaHei");
            painter->setFont(font);
            if(IfValid)
            {
                // WRITE_CENTRAL_WIDGET_DBG("Chip:%d, Online, Satisfied cnt = %d, Completed cnt = %d\n",
                //     row - 1, TestRes.GetLeft(), TestRes.GetRight());
                switch (OnLineStat)
                {
                case INITIAL_OFF_LINE:
                {
                    OnLineStat = NORMAL_ON_LINE;
                    painter->fillRect(Rc[0], QColor(Qt::green));
                    if(TestRes.GetLeft() == TestRes.GetRight() && TestRes.GetRight() != 0)
                    {
                        // painter->fillRect(Rc[0], QColor(Qt::green));
                        // palette.setColor(QPalette::ColorRole::Highlight, QColor(Qt::red));
                        //绘制背景边框
                        painter->setPen(QPen(QColor(Qt::green), 2));
                        painter->setBrush(QColor(Qt::white));
                        painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                        //绘制进度条
                        QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                        painter->fillRect(progressRect, QColor(Qt::green));
                    }
                    else if(TestRes.GetRight() == 0)
                    {
                       //绘制背景边框
                       painter->setPen(QPen(QColor(Qt::lightGray), 2));
                       painter->setBrush(QColor(Qt::lightGray));
                       painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
   
                       //绘制进度条
                       QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                    //    progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
                       painter->fillRect(progressRect, QColor(Qt::red));
                    }
                    else
                    {
                        //绘制背景边框
                        painter->setPen(QPen(QColor(Qt::red), 2));
                        painter->setBrush(QColor(Qt::white));
                        painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                        //绘制进度条
                        QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                        // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
                        painter->fillRect(progressRect, QColor(Qt::red));
                    }
                    painter->setPen(Qt::black);
                    painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").
                        arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
                        arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
                }
                    break;
                case NORMAL_OFF_LINE:
                {
                    OnLineStat = ABNORMAL_ON_LINE;
                    painter->fillRect(Rc[0], QColor(Qt::yellow));
                        // painter->fillRect(Rc[0], QColor(Qt::green));
                        // palette.setColor(QPalette::ColorRole::Highlight, QColor(Qt::red));
                    //绘制背景边框
                    painter->setPen(QPen(QColor(Qt::red), 2));
                    painter->setBrush(QColor(Qt::white));
                    painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                    //绘制进度条
                    QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                    // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
                    painter->fillRect(progressRect, QColor(Qt::red));
                    painter->setPen(Qt::black);
                    painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").
                        arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
                        arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
                }
                case NORMAL_ON_LINE:
                {
                    painter->fillRect(Rc[0], QColor(Qt::green));
                    if(TestRes.GetLeft() == TestRes.GetRight())
                    {
                        // painter->fillRect(Rc[0], QColor(Qt::green));
                        // palette.setColor(QPalette::ColorRole::Highlight, QColor(Qt::red));
                        //绘制背景边框
                        painter->setPen(QPen(QColor(Qt::green), 2));
                        painter->setBrush(QColor(Qt::white));
                        painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                        //绘制进度条
                        QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                        painter->fillRect(progressRect, QColor(Qt::green));
                    }
                    else
                    {
                        // painter->fillRect(Rc[0], QColor(Qt::red));
                        // palette.setColor(QPalette::ColorRole::Highlight, QColor(Qt::red));
                        //绘制背景边框
                        painter->setPen(QPen(QColor(Qt::red), 2));
                        painter->setBrush(QColor(Qt::white));
                        painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                        //绘制进度条
                        QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                        // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
                        painter->fillRect(progressRect, QColor(Qt::red));
                    }
                    painter->setPen(Qt::black);
                    painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").
                        arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
                        arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
                }
                    break;
                case ABNORMAL_OFF_LINE:
                    OnLineStat = ABNORMAL_ON_LINE;
                case ABNORMAL_ON_LINE:
                {
                    painter->fillRect(Rc[0], QColor(Qt::yellow));
                    //绘制背景边框
                    painter->setPen(QPen(QColor(Qt::red), 2));
                    painter->setBrush(QColor(Qt::white));
                    painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                    //绘制进度条
                    QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                    // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
                    painter->fillRect(progressRect, QColor(Qt::red));
                    painter->setPen(Qt::black);
                    painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").
                        arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
                        arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
                }
                default:
                    break;
                }
                
            }
            else
            {
                switch (OnLineStat)
                {
                case INITIAL_OFF_LINE:
                    OnLineStat = NORMAL_OFF_LINE;
                case NORMAL_OFF_LINE:
                {
                    painter->fillRect(Rc[0], QColor(Qt::gray));
                    //绘制背景边框
                    painter->setPen(QPen(QColor(Qt::gray), 2));
                    painter->setBrush(QColor(Qt::gray));
                    painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                    painter->setPen(Qt::black);
                    painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").
                        arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
                        arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
                }
                    break;
                case NORMAL_ON_LINE:
                    OnLineStat = ABNORMAL_OFF_LINE;
                case ABNORMAL_OFF_LINE:
                {
                    painter->fillRect(Rc[0], QColor(Qt::red));
                    //绘制背景边框
                    painter->setPen(QPen(QColor(Qt::red), 2));
                    painter->setBrush(QColor(Qt::white));
                    painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                    //绘制进度条
                    QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                    // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
                    painter->fillRect(progressRect, QColor(Qt::red));
                    painter->setPen(Qt::black);
                    painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").
                        arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
                        arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
                }
                    break;
                case ABNORMAL_ON_LINE:
                {
                    OnLineStat = ABNORMAL_OFF_LINE;
                    painter->fillRect(Rc[0], QColor(Qt::red));
                    //绘制背景边框
                    painter->setPen(QPen(QColor(Qt::red), 2));
                    painter->setBrush(QColor(Qt::white));
                    painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);
    
                    //绘制进度条
                    QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                    // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
                    painter->fillRect(progressRect, QColor(Qt::red));
                    painter->setPen(Qt::black);
                    painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").
                        arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
                        arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
                }
                    break;
                default:
                    break;
                }
            }
            // palette.setColor(QPalette::Text, Qt::black);

            painter->drawText(Rc[0], QString("S%1").arg(row));

            // ProgressBarOption.rect = Rc[1];
            // ProgressBarOption.minimum = 0;
            // ProgressBarOption.maximum = (TestRes.GetRight() == 0) ? 1 : TestRes.GetRight();
            // ProgressBarOption.progress = TestRes.GetLeft();// QRandomGenerator::global()->bounded(0, 100);
            // WRITE_CENTRAL_WIDGET_DBG("Item::Paint(), TestRes.GetLeft() = %d, TestRes.GetRight() = %lld\n", TestRes.GetLeft(), TestRes.GetRight());
            // ProgressBarOption.text = QString("%1\%(%2/%3)").arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10);
            // ProgressBarOption.textAlignment = Qt::AlignCenter;
            // ProgressBarOption.textVisible = true;
            
            // ProgressBarOption.palette = palette;

            // QApplication::style()->drawControl(QStyle::CE_ProgressBar, &ProgressBarOption, painter);

            painter->fillRect(Rc[2], Qt::gray);
            painter->drawText(Rc[2], Qt::AlignCenter, QString("[%1, %2]").arg(SinadRang.GetLeft(), 2, 10).arg(SinadRang.GetRight(), 2, 10));

            painter->restore();

            // WRITE_CENTRAL_WIDGET_DBG("Item::paint(), End\n");
        }
    }

    //

    Model::Model(QObject* parent)
        :QAbstractListModel(parent)
    {
    }

    void Model::AddItem(Item i)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        Items.push_back(i);
        endInsertRows();
    }

    void Model::UpdateItem(int row, Item i)
    {
        Items[row - 1] = i;
        QModelIndex index = createIndex(row, 0);
        dataChanged(index, index);
    }

    void Model::SetBoardNum(int n)
    {
        BoardNum = n;
    }

    int Model::rowCount(const QModelIndex& parent) const
    {
        return static_cast<int>(Items.size()) + 1;
    }

    QVariant Model::data(const QModelIndex& index, int role) const
    {
        if(index.row() > 0 && index.row() < rowCount())
        {
            switch (role)
            {
            case ITEM:              return QVariant::fromValue(Items[index.row() - 1]);
            case IF_VALID:          return Items[index.row() - 1].IfValid;
            case TEST_RES_RANG:     return QVariant::fromValue(Items[index.row() - 1].TestRes);
            case SINAD_RANGE:       return QVariant::fromValue(Items[index.row() - 1].SinadRang);
            default:
                break;
            }
        }
        else
        {
            return QString("J%1").arg(BoardNum);
        }

        return QVariant();
    }

    bool Model::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        if(index.row() > 0 && index.row() < rowCount())
        {
            switch (role)
            {
            case ITEM:          Items[index.row() - 1] = value.value<Item>();                return true;
            case IF_VALID:      Items[index.row() - 1].IfValid = value.value<bool>();        return true;
            case TEST_RES_RANG: Items[index.row() - 1].TestRes = value.value<Range>();       return true;
            case SINAD_RANGE:   Items[index.row() - 1].SinadRang = value.value<Range>();     return true;
            default:
                break;
            }
        }

        return false;
    }

    Qt::ItemFlags Model::flags(const QModelIndex& index) const
    {
        if(index.row() > 0 && index.row() < rowCount())
        {
            return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
        return Qt::ItemIsEnabled;
    }

    void Delegate::SetBoardNum(int n)
    {
        BoardNum = n;
    }

    void Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            Item item = index.data(Model::ITEM).value<Item>();
            item.Paint(painter, option, index.row());
            QStyledItemDelegate::paint(painter, option, index);
        }
        else if(index.row() == 0)
        {
            painter->fillRect(option.rect, QBrush(QColor(255, 255, 255)));
            QFont font("Microsoft YaHei");
            font.setWeight(QFont::Black);
            painter->setFont(font);
            painter->drawText(option.rect, Qt::AlignLeft, QString("J%1").arg(BoardNum));
        }
    }

    QSize Delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            return QSize(option.rect.width(), 24);
        }
        else if(index.row() == 0)
        {
            return QSize(option.rect.width(), 24);
        }
        return QSize();
    }

    //

    bool RetEventFilter::eventFilter(QObject* obj, QEvent* e)
    {
        switch (e->type())
        {
        case QEvent::KeyPress:
        {
            QKeyEvent* KeyEvent = static_cast<QKeyEvent*>(e);
            switch (KeyEvent->key())
            {
            case Qt::Key_Return:
            case Qt::Key_Enter:
            {
                QPlainTextEdit* TextEdit = qobject_cast<QPlainTextEdit*>(obj);
                if(TextEdit)
                {
                    TextEdit->clearFocus();
                    return true;
                }
            }
                break;
            
            default:
                break;
            }
        }
            break;
        
        default:
            break;
        }

        return QObject::eventFilter(obj, e);
    }

    //

    CentralWidget::CentralWidget(QWidget* parent)
        :QWidget(parent)
    {
        OPEN_CENTRAL_WIDGWT_DBG(LogCenTralWidget.txt);

        InitUI();
        InitThread();
        Connect();
        CreateThread();
        // TimingDetection->start(1000);
    }

    CentralWidget::~CentralWidget()
    {
        if(TaskStatistics) TaskStatistics = nullptr;
        if(TaskSend) TaskSend = nullptr;
        TimingDetection->stop();
        CLOSE_CENTRAL_WIDGET_DBG();
    }

    void CentralWidget::InitUI()
    {
        LocalIP             .setText("本机IPPORT:00.00.00.00:0000");
        DeviceIP            .setText("设备IPPORT:00.00.00.00:0000");
        VersionNum          .setText("版本号:--");
        ChipType            .setText("芯片型号:AIC9628BM");
        TemperatureInner    .setText("FPGA温度:--℃");
        TemperatureEnv      .setText("环境温度:--℃");

        for(int i = 0; i < 8; i++)
        {
            Models[i].SetBoardNum(i + 1);
            Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));
            Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));
            Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));
            Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));
            
            Delegates[i].SetBoardNum(i + 1);
            Lists[i].setModel(&Models[i]);
            Lists[i].setItemDelegate(&Delegates[i]);
            Lists[i].setMinimumSize(270, 160);
        }

        Filter = new RetEventFilter(this);

        LocalIP         .setFixedWidth(200);
        DeviceIP        .setFixedWidth(200);
        VersionNum      .setFixedWidth(200);
        ChipType        .setFixedWidth(200);
        TemperatureInner.setFixedWidth(200);
        TemperatureEnv  .setFixedWidth(200);
        LTestNum.setText("总测试次数");
        LTestedNum.setText("已测试次数");
        LTestTime.setText("总测试时间");
        LOnlineChipNum.setText("芯片在线数量");
        LSatisfiedChipNum.setText("测试通过数量");
        LRejectionChipNum.setText("测试失败数量");
        BtnStartTest.setText("开始测试");
        BtnOpenLog.setText("打开日志");
        PTestNum.installEventFilter(Filter);
        PTestNum.setMaximumHeight(20);
        PTestedNum.setMaximumHeight(20);
        PTestTime.setMaximumHeight(20);
        POnlineChipNum.setMaximumHeight(20);
        PSatisfiedChipNum.setMaximumHeight(20);
        PRejectionChipNum.setMaximumHeight(20);
        GbInstrumentBoard.setMinimumSize(270, 160);
        PTestNum.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        PTestNum.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        PTestNum.setMaximumWidth(60);
        PTestedNum.setMaximumWidth(60);
        PTestTime.setMaximumWidth(60);
        POnlineChipNum.setMaximumWidth(60);
        PSatisfiedChipNum.setMaximumWidth(60);
        PRejectionChipNum.setMaximumWidth(60);

        HlIP.addWidget(&LocalIP);
        // HlIP.addStretch();
        HlIP.addSpacing(30);
        HlIP.addWidget(&DeviceIP);
        HlIP.addStretch();
        HlNum.addWidget(&VersionNum);
        // HlNum.addStretch();
        HlNum.addSpacing(30);
        HlNum.addWidget(&ChipType);
        HlNum.addStretch();
        HlTemp.addWidget(&TemperatureInner);
        // HlTemp.addStretch();
        HlTemp.addSpacing(30);
        HlTemp.addWidget(&TemperatureEnv);
        HlTemp.addStretch();
        VlStat.addLayout(&HlIP);
        VlStat.addLayout(&HlNum);
        VlStat.addLayout(&HlTemp);
        GbStat.setLayout(&VlStat);

        HlTestNum.addWidget(&LTestNum);
        HlTestNum.addStretch();
        HlTestNum.addWidget(&PTestNum);
        // HlTestNum.addStretch();
        HlTestedNum.addWidget(&LTestedNum);
        HlTestedNum.addStretch();
        HlTestedNum.addWidget(&PTestedNum);
        // HlTestedNum.addStretch();
        HlTestTime.addWidget(&LTestTime);
        HlTestTime.addStretch();
        HlTestTime.addWidget(&PTestTime);
        // HlTestTime.addStretch();
        HlOnlineChipNum.addWidget(&LOnlineChipNum);
        HlOnlineChipNum.addStretch();
        HlOnlineChipNum.addWidget(&POnlineChipNum);
        // HlOnlineChipNum.addStretch();
        HlSatisfiedNum.addWidget(&LSatisfiedChipNum);
        HlSatisfiedNum.addStretch();
        HlSatisfiedNum.addWidget(&PSatisfiedChipNum);
        // HlSatisfiedNum.addStretch();
        HlRejectedNum.addWidget(&LRejectionChipNum);
        HlRejectedNum.addStretch();
        HlRejectedNum.addWidget(&PRejectionChipNum);
        // HlRejectedNum.addStretch();
        HlBtn.addStretch();
        HlBtn.addWidget(&BtnStartTest);
        HlBtn.addStretch();
        HlBtn.addWidget(&BtnOpenLog);
        HlBtn.addStretch();
        VlInstrumentBoard.addLayout(&HlTestNum);
        VlInstrumentBoard.addLayout(&HlTestedNum);
        VlInstrumentBoard.addLayout(&HlTestTime);
        VlInstrumentBoard.addLayout(&HlOnlineChipNum);
        VlInstrumentBoard.addLayout(&HlSatisfiedNum);
        VlInstrumentBoard.addLayout(&HlRejectedNum);
        VlInstrumentBoard.addLayout(&HlBtn);
        VlInstrumentBoard.addStretch();
        GbInstrumentBoard.setLayout(&VlInstrumentBoard);        

        HlLineFst.addWidget(&Lists[0]);
        HlLineFst.addWidget(&Lists[1]);
        HlLineFst.addWidget(&Lists[2]);
        HlLineSec.addWidget(&Lists[7]);
        HlLineSec.addWidget(&GbInstrumentBoard);
        HlLineSec.addWidget(&Lists[3]);
        HlLineThd.addWidget(&Lists[6]);
        HlLineThd.addWidget(&Lists[5]);
        HlLineThd.addWidget(&Lists[4]);
        VlDevStatistics.addLayout(&HlLineFst);
        VlDevStatistics.addLayout(&HlLineSec);
        VlDevStatistics.addLayout(&HlLineThd);
        GbDevStatistics.setLayout(&VlDevStatistics);

        VlAll.addWidget(&GbStat);
        VlAll.addWidget(&GbDevStatistics);

        setLayout(&VlAll);
        
        Clock = new QTimer(this);
        TimingDetection = new QTimer(this);
    }

    void CentralWidget::InitThread()
    {
        WRITE_CENTRAL_WIDGET_DBG("InitThread()\n");
        TaskStatistics = new TASKWZ::TaskTestStatistics;
        TaskSend = nullptr;
        TaskVersion = new TASKWZ::TaskVersionParsing;
    }

    void CentralWidget::Connect()
    {
        connect(&BtnStartTest, &QPushButton::clicked, this, &CentralWidget::StartTest);
        connect(&BtnOpenLog, &QPushButton::clicked, this, &CentralWidget::OpenLog);
        connect(TaskStatistics, &TASKWZ::TaskTestStatistics::MsgOfStatistics, this, &CentralWidget::UpdateCheckResult, Qt::QueuedConnection);
        connect(TaskVersion, &TASKWZ::TaskVersionParsing::UpdateBoardVersion, this, &CentralWidget::UpdateVersion, Qt::QueuedConnection);
        connect(Clock, &QTimer::timeout, this, &CentralWidget::ClockEvent);
        connect(TimingDetection, &QTimer::timeout, this, &CentralWidget::TimingDetectionEvent);
    }

    void CentralWidget::CreateThread()
    {
        TASKWZ::worker_manager::create(TaskStatistics, TASKWZ::worker_type::EXECUTE_THREAD);
        CREATE_TASK_MNIC_PARSING;
        CREATE_TASK_DISPATCH;
        CREATE_TASK_RCV;
        TASKWZ::worker_manager::create(TaskVersion, TASKWZ::worker_type::EXECUTE_THREAD);
    }

    void CentralWidget::UpdateSockInfo()
    {
        LocalIP.setText("本机IPPORT: " + SOCKWZ::SockGlob::Sock->GetAddr()[0] + ":" + SOCKWZ::SockGlob::Sock->GetAddr()[1]);
        DeviceIP.setText("设备IPPORT: " + SOCKWZ::SockGlob::Sock->GetAddr()[2] + ":" + SOCKWZ::SockGlob::Sock->GetAddr()[3]);
    }

    void CentralWidget::StartTest()
    {
        WRITE_CENTRAL_WIDGET_DBG("StartTest(), Enter\n");        
        if(StatOfBtnStart)
        {
            PTestedNum.setText("0");
            TOOLWZ::stack_wz<TASKWZ::task_type> stack_join;
            stack_join.push(TASKWZ::task_type::TASK_END);
            stack_join.push(TASKWZ::task_type::TASK_JOIN);
            stack_join.push(TASKWZ::TASK_DATA_CONSTRUCT);
            // if(TaskSend)
            // {
            //     WRITE_CENTRAL_WIDGET_DBG("TaskSend != nullptr\n");
            //     stack_join.push(TASKWZ::TASK_DATA_SEND);
            //     disconnect(TaskSend, &TASKWZ::TaskDataSend::MsgOfStartEnd, this, &CentralWidget::UpdateCheckResult);
            //     TaskSend = nullptr;
            // }
            stack_join.push(TASKWZ::TASK_DATA_SEND);
            CREATE_TASK_JOIN(stack_join);
            TaskSend = new TASKWZ::TaskDataSend(PTestNum.toPlainText().toInt());
            connect(TaskSend, &TASKWZ::TaskDataSend::MsgOfStartEnd, this, &CentralWidget::UpdateCheckResult, Qt::QueuedConnection);
            TASKWZ::worker_manager::create(TaskSend, TASKWZ::worker_type::EXECUTE_THREAD);
            StatOfBtnStart = false;
            BtnStartTest.setText("结束测试");
        }
        else
        {    
            std::unique_lock lock(mtx);
            Clock->stop();        
            if(TaskSend)
            {
                disconnect(TaskSend, &TASKWZ::TaskDataSend::MsgOfStartEnd, this, &CentralWidget::UpdateCheckResult);
                TaskSend->close();
                TaskSend = nullptr;
            }
            TOOLWZ::stack_wz<TASKWZ::task_type> stack_end;
            stack_end.push(TASKWZ::TASK_DATA_SEND);
            CREATE_TASK_END(stack_end);
            StatOfBtnStart = true;
            BtnStartTest.setText("开始测试");
            LPWZ::LogProcessor().processLog("./LOG/LogUpRecord.log", "./LOG/LogUpRecordProcessed.log");
            lock.unlock();
        }
        WRITE_CENTRAL_WIDGET_DBG("StartTest(), End\n");
    }

    void CentralWidget::OpenLog()
    {}

    void CentralWidget::UpdateCheckResult(TASKWZ::MsgToCentralWt msg)
    {
        WRITE_CENTRAL_WIDGET_DBG("CentralWidget::UpdateCheckResult(), Enter\n");
        switch (msg.GetStat())
        {
        case TASKWZ::TestStat::TEST_START:
            ClockVal = 0;
            Clock->start(1000);
            DCR::DeviceCheckResultGlobal->Init();
            DCR::DeviceCheckResultGlobal->SetCheckCount(TaskSend->TestCount);
            TASKWZ::TaskDataSend::SemaWaitForUI.release(1);
            break;
        case TASKWZ::TestStat::TEST_RUNNING:
            WRITE_CENTRAL_WIDGET_DBG("case TESTRUNNING\n");
            switch (msg.GetMode())
            {
            case TASKWZ::STATISTICS_A_MIF:
                WRITE_CENTRAL_WIDGET_DBG("case: STATISTICS_A_MIF Enter\n");
                for(int i = 0; i < 8; i++)
                {
                    for(int j = 0; j < 4; j++)
                    {
                        Models[i].UpdateItem(j + 1, CWD::Item(
                            DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline(),
                            Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckSatisfiedCount(), 
                                DCR::DeviceCheckResultGlobal->GetCheckCompletedCount()), 
                            Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD())
                        ));
                        DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetCheckPacksOfMif(0);
                        DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetChipTestStat(DCR::WAITING_FOR_TESTING);
                    }
                }       
                WRITE_CENTRAL_WIDGET_DBG("for(for()) after\n");
                POnlineChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipOnLineNum()));                
                TASKWZ::TaskDataSend::SemaWaitForUI.release(1);
                WRITE_CENTRAL_WIDGET_DBG("case: STATISTICS_A_MIF Leave\n");
                break;
            case TASKWZ::STATISTICS_A_TIME:
                WRITE_CENTRAL_WIDGET_DBG("case: STATISTICS_A_TIME\n");
                for(int i = 0; i < 8; i++)
                {
                    for(int j = 0; j < 4; j++)
                    {
                        Models[i].UpdateItem(j + 1, CWD::Item(
                            DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline(),
                            Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckSatisfiedCount(), 
                                DCR::DeviceCheckResultGlobal->GetCheckCompletedCount() + 1), 
                            Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD())
                        ));
                    }
                }
                PTestedNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetCheckCompletedCount() + 1));

                POnlineChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipOnLineNum()));
                PSatisfiedChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipSatisfiedNum()));
                PRejectionChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipUnSatisfiedNum()));
                DCR::DeviceCheckResultGlobal->Reset();
                DCR::DeviceCheckResultGlobal->CheckCompletedCountInc();
                TASKWZ::TaskDataSend::SemaWaitForUI.release(1);
                break;
            default:
                break;
            }        
            break;
        case TASKWZ::TestStat::TEST_OVER:
        {
            WRITE_CENTRAL_WIDGET_DBG("TEST OVER\n");
            std::unique_lock lock(mtx);
            Clock->stop();
            if(TaskSend)
            {
                disconnect(TaskSend, &TASKWZ::TaskDataSend::MsgOfStartEnd, this, &CentralWidget::UpdateCheckResult);
                TaskSend->close();
                TaskSend = nullptr;
            }
            TOOLWZ::stack_wz<TASKWZ::task_type> stack_end;
            stack_end.push(TASKWZ::TASK_DATA_SEND);
            CREATE_TASK_END(stack_end);
            StatOfBtnStart = true;
            BtnStartTest.setText("开始测试");
            LPWZ::LogProcessor().processLog("./LOG/LogUpRecord.log", "./LOG/LogUpRecordProcessed.log");
            lock.unlock();
        }
            break;
        default:
            break;
        }
        WRITE_CENTRAL_WIDGET_DBG("CentralWidget::UpdateCheckResult(), End\n");
    }

    void CentralWidget::UpdateVersion()
    {
        WRITE_CENTRAL_WIDGET_DBG("CentralWidget::UpdateVersion(), Enter\n");
        // VersionNum.setText(QString("版本号:%1\%2%3-%4\%5%6:R%7").arg(TASKWZ::TaskVersionParsing::Version.YEAR, 2, 16, QChar(u'0')).
        // arg(TASKWZ::TaskVersionParsing::Version.MONTH, 2, 16, QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.DAY, 2, 16, QChar(u'0')).
        // arg(TASKWZ::TaskVersionParsing::Version.HOUR, 2, 16 , QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.MIN, 2, 16, QChar(u'0')).
        // arg(TASKWZ::TaskVersionParsing::Version.SEC, 2, 16  , QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.VER, 4, 16, QChar(u'0')));

        VersionNum.setText(QString("版本号:20%1-%2-%3-%4-%5:R%6").arg(TASKWZ::TaskVersionParsing::Version.YEAR, 2, 16, QChar(u'0')).
        arg(TASKWZ::TaskVersionParsing::Version.MONTH, 2, 16, QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.DAY, 2, 16, QChar(u'0')).
        arg(TASKWZ::TaskVersionParsing::Version.HOUR, 2, 16, QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.MIN, 2, 16, QChar(u'0')).
        arg(TASKWZ::TaskVersionParsing::Version.VER, 5, 10, QChar(u'0')));
    }

    void CentralWidget::ClockEvent()
    {
        PTestTime.setText(QString("%1:%2:%3").arg(ClockVal / 3600, 2, 10).arg(ClockVal / 60, 2, 10).arg(ClockVal, 2, 10));
        ClockVal++;
    }

    void CentralWidget::NetConnected()
    {
        TimingDetection->start(1000);
        IfNoticeMNICWhenDisconnect = true;
    }

    void CentralWidget::NetDisconnected()
    {
        TimingDetection->stop();
        ResetModelItemStat();
    }

    void CentralWidget::TimingDetectionEvent()
    {
        WRITE_CENTRAL_WIDGET_DBG("CentralWidget::TimingDetectionEvent(), Enter\n");
        TemperatureInner.setText(QString("FPGA温度:%1℃").arg(DCR::DeviceCheckResultGlobal->GetTemperatureInner(), 4, 'f', 1, '0'));
        TemperatureEnv.setText(QString("环境温度:%1℃").arg(DCR::DeviceCheckResultGlobal->GetTempeartureEnv(), 4, 'f', 1, '0'));
        if(DCR::DeviceCheckResultGlobal->GetUpPackCount() == UpPackCount)
        {
            if(IfNoticeMNICWhenDisconnect)
            {
                IfNoticeMNICWhenDisconnect = false;
                ResetModelItemStat();
                emit NetLoss();
            }
        }
        else
        {
            UpPackCount = DCR::DeviceCheckResultGlobal->GetUpPackCount();
            if(!IfNoticeMNICWhenDisconnect)
            {
                IfNoticeMNICWhenDisconnect = true;
                emit NetRecovery();
            }
        }
    }

    void CentralWidget::ResetModelItemStat()
    {
        for(int i = 0; i < 8; i++)
        {
            for(auto it : Models[i].Items)
            {
                it.RestOnLineStat();
            }
        }
    }
};