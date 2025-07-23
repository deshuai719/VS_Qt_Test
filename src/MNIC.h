#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QCloseEvent>
#include <QTextEdit>
#include <QFile>
#include "DlgMenuFile.hpp"
#include "DlgMenuCFG.hpp"
#include "CentralWindow.hpp"

#define DEFAULT_FILE_PATH "./InitFile"

class MNIC : public QMainWindow {
    Q_OBJECT
private:
    QMenuBar* MenuBar;

    QMenu* FileMenu;
    //QAction* AddFileAction;
    QAction* AddArgAction;

    QMenu* ConfigMenu;
    QAction* NetConfigAction;
    //QAction* SinadConfigAction;

    QMenu* LogMenu;
    QAction* LogONAction;
    QAction* LogOFFAction;

    //QMenu* HelpMenu;

    QStatusBar* StatusBar;
    QLabel* LabelStat;

    DlgMenu::DlgMu* DlgMenuFileSelect;
    DlgMenuARG::DlgARG* DlgMenuArgAdd;
    MenuSocketCFG::DialogSockCFG* DlgMenuSocketCFG;
    //MenuSINADCFG::DialogSinadCFG* DlgMenuSinadCFG;
    CWD::CentralWidget* CentralWt;

    void InitMenuBar();
    void InitFileMenu();
    void InitConfigMenu();
    void InitLogMenu();
    //void InitHelpMenu();
    void InitStatusBar();
    void InitCentralWt();
    void InitUI();

    //void AddFile();
    void AddArg();
    void NetConfig();
    //void SinadConfig();
    void LogON();
    void LogOFF();

    void ModifyStatus(bool);

    void closeEvent(QCloseEvent* event) override;
signals:
    void NetLoss();//to Dlg Sock CFG
signals:
    void NetConnected();//to Central Widget
    void NetDisconnected();//to central widget
public slots:
    void NetRecovery();//msg from Central Widget
    void ResetNetStatus();//msg from Central Widget
public:
    MNIC(QWidget* parent = nullptr);
    ~MNIC();
};