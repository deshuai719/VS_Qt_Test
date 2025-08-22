//UI界面文字添加
//

#include "MNIC.h"
#include "LogWZ.hpp"

MNIC::MNIC(QWidget* parent)
    : QMainWindow(parent)
{
    OPEN_MNIC_DBG(LogMNIC.txt);
    TASKWZ::worker_manager::init(100);
    TASKWZ::worker_manager::begin_task_system();
    FCT::FluidCtrlGlob->Init();
    DCR::DeviceCheckResultGlobal->Init(8, 4);
    SDG::SamplingDataGenerate::cosBufferInit();
    DCWZ::DataConstruct::InitCmdRegCFG();
    resize(1000, 800);
    InitUI();
}

MNIC::~MNIC()
{
}

void MNIC::InitMenuBar()
{
    MenuBar = new QMenuBar(this);
    setMenuBar(MenuBar);
}


void MNIC::InitFileMenu()
{
    FileMenu = new QMenu("参数", this);
    MenuBar->addMenu(FileMenu);

    //AddFileAction = new QAction("添加文件", this);
    //FileMenu->addAction(AddFileAction);
    //connect(AddFileAction, &QAction::triggered, this, &MNIC::AddFile);

    //DlgMenuFileSelect = new DlgMenu::DlgMu(this);

    //QDir Dir(DEFAULT_FILE_PATH);

    //if(Dir.exists())
    //{
    //    //设置过滤器
    //    QStringList filters;
    //    filters.append("*.mif*");//匹配.mif文件
    //    Dir.setNameFilters(filters);
    //    Dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);//只获取文件，排除.和..

    //    //获取文件列表
    //    QStringList fileList = Dir.entryList();

    //    //遍历文件列表
    //    for(const QString &file : fileList)
    //    {
    //        DlgMenuFileSelect->AddFile(Dir.filePath(file));
    //    }
    //    DlgMenuFileSelect->DataConstruct();
    //}

    AddArgAction = new QAction("添加参数", this);
    FileMenu->addAction(AddArgAction);
    connect(AddArgAction, &QAction::triggered, this, &MNIC::AddArg);

    DlgMenuArgAdd = new DlgMenuARG::DlgARG(this);
}

void MNIC::InitLogMenu()
{
    LogMenu = new QMenu("Log", this);
    MenuBar->addMenu(LogMenu);

    LogONAction = new QAction("Log ON", this);
    LogMenu->addAction(LogONAction);
    connect(LogONAction, &QAction::triggered, this, &MNIC::LogON);

    LogOFFAction = new QAction("Log OFF", this);
    LogMenu->addAction(LogOFFAction);
    connect(LogOFFAction, &QAction::triggered, this, &MNIC::LogOFF);
}

//void MNIC::InitHelpMenu()
//{
//    HelpMenu = new QMenu("Help", this);
//    MenuBar->addMenu(HelpMenu);
//}

void MNIC::InitCentralWt()
{
    CentralWt = new CWD::CentralWidget(this);
    setCentralWidget(CentralWt);
}

void MNIC::InitStatusBar()
{
    StatusBar = new QStatusBar(this);
    setStatusBar(StatusBar);

    StatusBar->showMessage("Ready", 3000);
    LabelStat = new QLabel("网口未连接", this);
    StatusBar->addPermanentWidget(LabelStat);
}

void MNIC::InitUI()
{
    setWindowTitle(VERSION_NUM);
    InitMenuBar();
    InitFileMenu();
    InitLogMenu();
    //InitHelpMenu();
    InitStatusBar();
    InitCentralWt();

    connect(CentralWt, &CWD::CentralWidget::NetLoss, this, &MNIC::ResetNetStatus);
    connect(CentralWt, &CWD::CentralWidget::NetRecovery, this, &MNIC::NetRecovery);
    connect(this, &MNIC::NetConnected, CentralWt, &CWD::CentralWidget::NetConnected);
    connect(this, &MNIC::NetDisconnected, CentralWt, &CWD::CentralWidget::NetDisconnected);
}

//void MNIC::AddFile()
//{
//    DlgMenuFileSelect->exec();//模态对话框
//}

void MNIC::AddArg()
{
    DlgMenuArgAdd->exec();
}

//void MNIC::SinadConfig()
//{
//    DlgMenuSinadCFG->exec();
//}

void MNIC::LogON()
{
    g_logOn = true; // 新增：打开日志
    qDebug() << "Log ON";
}

void MNIC::LogOFF()
{
    g_logOn = false; // 新增：关闭日志
    qDebug() << "Log OFF";
}

void MNIC::ModifyStatus(bool b)
{
    if(b)
    {
        emit NetConnected();//msg to Central Widget
        LabelStat->setText("网口已连接");
    }
    else
    {
        emit NetDisconnected();//msg to central widget
        LabelStat->setText("网口未连接");
    }
}

void MNIC::closeEvent(QCloseEvent* event)
{
    // TASKWZ::worker_manager::end_task_system();
    // SOCKWZ::SockGlob::Clear();
    DCWZ::DataMana::DataListGlobal.Clear();
    DCR::DeviceCheckResultGlobal->Clear();
    DCR::DeviceCheckResultGlobal.reset();
    SDG::SamplingDataGenerate::ReleaseCosBuffer();
    WRITE_MNIC_DBG("End Task SYS Befor\n");
    TASKWZ::worker_manager::end_task_system();
    WRITE_MNIC_DBG("End Task SYS After\n");
    SOCKWZ::SockGlob::Clear();
    WRITE_MNIC_DBG("After Sock::reset()\n");
    CLOSE_MNIC_DBG();
    event->accept();
}

void MNIC::NetRecovery()
{
    LabelStat->setText("网口已连接");
    QMessageBox::information(this, "网口状态", "连接成功", QMessageBox::Ok);
}

void MNIC::ResetNetStatus()
{
    LabelStat->setText("网口未连接");
    QMessageBox::critical(this, "网口状态", "连接断开", QMessageBox::Ok);
}