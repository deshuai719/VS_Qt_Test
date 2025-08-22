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
#include <QCheckBox>
#include <QFileDialog>
#include <QScrollBar>
#include <QTextCursor>
#include <QDateTime>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <memory>
#include "TestCondition.hpp"
#include "TaskWZ.hpp"
#include "SocketWZ.hpp"
#include "ConfigInit.hpp"

#define VERSION_NUM "圆芯Codec测试软件2508071718--V0"

namespace CWD{

    // 新增：全局信息管理器类（单例模式）
    class InfoMessageManager : public QObject
    {
        Q_OBJECT
    private:
        static InfoMessageManager* instance;
        InfoMessageManager(QObject* parent = nullptr) : QObject(parent) {}
        
    public:
        static InfoMessageManager* getInstance();
        
        // 静态方法：其他文档可以直接调用
        static void postInfo(const QString& message, const QString& level = "INFO");
        static void postInfoWithTime(const QString& message, const QString& level = "INFO");
        
    signals:
        void infoMessageReady(const QString& message, const QString& level);
    };

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

    Q_DECLARE_METATYPE(CWD::Item)

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
        void UpdateItemOnlineStatus(int row, bool online); // 新增：仅更新状态声明
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

    class CentralWidget: 
        public QWidget
    {
        Q_OBJECT

        QLabel VersionNum;
        QLabel ChipType;
        QLabel TemperatureInner;
        QLabel TemperatureEnv;
        QString currentLogSePath; 
        QString testSessionTime; // 新增：测试会话时间戳
        
        // 网络配置相关控件
        QLineEdit LeLocalIP;          // 本地IP输入框
        QLineEdit LeLocalPort;        // 本地端口输入框
        QLineEdit LeDeviceIP;         // 设备IP输入框  
        QLineEdit LeDevicePort;       // 设备端口输入框
        QPushButton BtnConnect;       // 连接按钮
        QRegularExpressionValidator* ValidatorIP;  // IP地址验证器
        QRegularExpressionValidator* ValidatorPort; // 端口验证器
        bool isConnected;             // 连接状态标志
    
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

        // 新增：底部信息显示区域
        QPlainTextEdit InfoTextArea;           // 信息显示文本区域
        QPushButton BtnClearInfo;              // 清空信息按钮
        QPushButton BtnSaveInfo;               // 保存信息按钮
        QCheckBox ChkAutoScroll;               // 自动滚动复选框
        QHBoxLayout HlInfoControls;            // 信息区域控制按钮布局
        QVBoxLayout VlInfoArea;                // 信息区域整体布局
        QGroupBox GbInfoArea;                  // 信息区域分组框

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
        int TotalGroupCount = 0;
        RetEventFilter* Filter;

        bool StatOfBtnStart{ true };

        std::mutex mtx;

        bool IfNoticeMNICWhenDisconnect{ true };//网络断开时是否通知MNIC窗口

        void InitUI();
        void InitThread();
        void Connect();
        void CreateThread();
        void ResetModelItemStat();
        
        // 新增：重置在线芯片状态为蓝色显示的方法
        void ResetOnlineChipsToBlue();
        
        // 新增：信息显示区域相关方法
        void InitInfoArea();                   // 初始化信息显示区域
        void AppendInfo(const QString& message, const QString& level = "INFO"); // 添加信息
        void AppendInfoWithTime(const QString& message, const QString& level = "INFO"); // 添加带时间戳的信息
        
        // 网络配置相关方法
        void InitNetworkConfig();              // 初始化网络配置控件
        void LoadNetworkConfig();              // 从配置文件加载网络设置
        void SaveNetworkConfig();              // 保存网络设置到配置文件
        bool ValidateNetworkInputs();          // 验证网络输入
        void OnNetworkConnect();               // 处理连接/断开操作
        void UpdateConnectionStatus(bool connected); // 更新连接状态显示
    
    public slots:
        void StartTest();
        void OpenLog();
        void UpdateCheckResult(TASKWZ::MsgToCentralWt msg);
        void UpdateVersion();
        void ClockEvent();
        void NetConnected();//msg from MNIC
        void NetDisconnected();//msg from MNIC
        void TimingDetectionEvent();
        void UpdateChipOnlineStatus();//新增：在线状态刷新
        
        // 新增：信息显示区域槽函数
        void OnClearInfo();                    // 清空信息槽函数
        void OnSaveInfo();                     // 保存信息槽函数
        void OnAutoScrollToggled(bool enabled); // 自动滚动切换槽函数
    
        // 新增：接收全局信息的槽函数
        void OnGlobalInfoMessage(const QString& message, const QString& level);
        
        // 新增：网络配置槽函数
        void OnConnectBtnClicked();            // 连接按钮点击槽函数
        void OnNetworkInputChanged();          // 网络输入改变槽函数

    signals:
        void NetRecovery();//msg to MNIC
        void NetLoss();//msg to MNIC
    public:
        CentralWidget(QWidget* parent = nullptr);
        ~CentralWidget();
    };
};

// 全局快捷宏定义：其他文档可以直接使用
#define POST_INFO(msg) CWD::InfoMessageManager::postInfo(msg, "INFO")//表现为正常黑色
#define POST_INFO_WITH_TIME(msg) CWD::InfoMessageManager::postInfoWithTime(msg, "INFO")//携带时间的信息
#define POST_SUCCESS(msg) CWD::InfoMessageManager::postInfo(msg, "SUCCESS")    //表现为绿色
#define POST_SUCCESS_WITH_TIME(msg) CWD::InfoMessageManager::postInfoWithTime(msg, "SUCCESS")
#define POST_WARNING(msg) CWD::InfoMessageManager::postInfo(msg, "WARNING")    //表现为橙色
#define POST_WARNING_WITH_TIME(msg) CWD::InfoMessageManager::postInfoWithTime(msg, "WARNING")
#define POST_ERROR(msg) CWD::InfoMessageManager::postInfo(msg, "ERROR")        //表现为红色
#define POST_ERROR_WITH_TIME(msg) CWD::InfoMessageManager::postInfoWithTime(msg, "ERROR")

#endif#endif