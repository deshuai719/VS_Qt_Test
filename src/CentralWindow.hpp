#ifndef _CENTRALWINDOW_HPP_
#define _CENTRALWINDOW_HPP_

#include <QAbstractListModel>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QEvent>
#include <QListView>
#include <QHoverEvent>
#include <QPlainTextEdit>
#include <QVboxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QOverload>
#include <QLabel>
#include <QProgressBar>
#include <QApplication>
#include <QRandomGenerator>
#include <QTimer>
#include "TaskWZ.hpp"

#define VERSION_NUM "圆芯Codec测试软件2503111002--V0"

namespace CWD{

    class Range{
        long long Left;
        long long Right;
    public:
        Range();
        Range(long long left, long long right);
        Range(const Range& range);
        Range(const TCOND::Range& range);
        ~Range();

        void operator=(const Range& range);
        void SetRange(long long left, long long right);
        long long GetLeft() const;
        long long GetRight() const;
    };

    enum OnLineStat{
        INITIAL_OFF_LINE,//
        NORMAL_OFF_LINE,//hui
        NORMAL_ON_LINE,//green
        ABNORMAL_OFF_LINE,//red
        ABNORMAL_ON_LINE,//yellow
    };

    class Item{
    public:
        bool IfValid;
        OnLineStat OnLineStat;
        Range TestRes;
        Range SinadRang;

        Item();
        Item(bool, Range, Range);
        Item(const Item&);
        ~Item();

        void operator=(const Item&);
        void RestOnLineStat();
        void Paint(QPainter*, const QStyleOptionViewItem&, int);
    };

    Q_DECLARE_METATYPE(Item)

    class Model: public QAbstractListModel{
        Q_OBJECT
    public:
        enum UsrRole{
            ITEM = Qt::UserRole + 1,
            IF_VALID,
            TEST_RES_RANG,
            SINAD_RANGE,
        };

        Model(QObject* parent = nullptr);

        void AddItem(Item);
        void UpdateItem(int, Item);
        void SetBoardNum(int n);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
        virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

        std::vector<Item> Items;
        int BoardNum;
    };

    class Delegate: public QStyledItemDelegate{
        Q_OBJECT
    public:
        using QStyledItemDelegate::QStyledItemDelegate;

        void SetBoardNum(int n);

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    
        int BoardNum;
    };

    //Key Return Filter

    class RetEventFilter: public QObject{
    public:
        using QObject::QObject;
    protected:
        bool eventFilter(QObject* obj, QEvent* e) override;
    };

    //CentralWidget

    class CentralWidget: public QWidget{
        Q_OBJECT

        QLabel LocalIP;
        QLabel DeviceIP;
        QLabel VersionNum;
        QLabel ChipType;
        QLabel TemperatureInner;
        QLabel TemperatureEnv;
        
        Model Models[8];//8个
        Delegate Delegates[8];//8个
        QListView Lists[8];//8个

        //测试仪表盘
        QLabel LTestNum;//总测试次数
        QPlainTextEdit PTestNum;
        QLabel LTestedNum;//已测试次数
        QLabel PTestedNum;
        QLabel LTestTime;//总测试时间
        QLabel PTestTime;
        QLabel LOnlineChipNum;
        QLabel POnlineChipNum;
        QLabel LSatisfiedChipNum;
        QLabel PSatisfiedChipNum;
        QLabel LRejectionChipNum;
        QLabel PRejectionChipNum;
        QPushButton BtnStartTest;
        QPushButton BtnOpenLog;

        QHBoxLayout HlIP;
        QHBoxLayout HlNum;
        QHBoxLayout HlTemp;
        QVBoxLayout VlStat;
        QGroupBox GbStat;

        QHBoxLayout HlLineFst;
        QHBoxLayout HlLineSec;
        QHBoxLayout HlLineThd;
        QVBoxLayout VlDevStatistics;
        QGroupBox GbDevStatistics;

        QHBoxLayout HlTestNum;
        QHBoxLayout HlTestedNum;
        QHBoxLayout HlTestTime;
        QHBoxLayout HlOnlineChipNum;
        QHBoxLayout HlSatisfiedNum;
        QHBoxLayout HlRejectedNum;
        QHBoxLayout HlBtn;
        QVBoxLayout VlInstrumentBoard;
        QGroupBox GbInstrumentBoard;//测试仪表盘

        QVBoxLayout VlAll;

        TASKWZ::TaskTestStatistics *TaskStatistics;
        TASKWZ::TaskDataSend* TaskSend;
        TASKWZ::TaskVersionParsing* TaskVersion;
        QTimer* Clock;
        long long ClockVal;
        QTimer* TimingDetection;
        int UpPackCount{ -1 };
        RetEventFilter* Filter;

        bool StatOfBtnStart{ true };

        std::mutex mtx;

        bool IfNoticeMNICWhenDisconnect{ true };//网络断开时是否通知MNIC窗口

        void InitUI();
        void InitThread();
        void Connect();
        void CreateThread();
        void ResetModelItemStat();
    public slots:
        void UpdateSockInfo();
        void StartTest();
        void OpenLog();
        void UpdateCheckResult(TASKWZ::MsgToCentralWt msg);
        void UpdateVersion();
        void ClockEvent();
        void NetConnected();//msg from MNIC
        void NetDisconnected();//msg from MNIC
        void TimingDetectionEvent();
    signals:
        void NetRecovery();//msg to MNIC
        void NetLoss();//msg to MNIC
    public:
        CentralWidget(QWidget* parent = nullptr);
        ~CentralWidget();
    };
};

#endif