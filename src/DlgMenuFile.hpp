#ifndef _DLGMENUFILE_HPP_
#define _DLGMENUFILE_HPP_

#include <QDialog>
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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QOverload>
#include <QCloseEvent>
#include <QFileDialog>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <vector>
#include "TaskWz.hpp"

namespace DlgMenu{

    class ModelItem{
    public:
        QString FilePath;

        ModelItem();
        ModelItem(QString);
        ModelItem(const ModelItem& cp);
        ~ModelItem();

        void operator=(const ModelItem& as);
        void Paint(QPainter* painter, const QStyleOptionViewItem& option, int Row);
    };

    Q_DECLARE_METATYPE(ModelItem)

    class Model:public QAbstractListModel{
        Q_OBJECT
    public:
        enum UsrRole{
            ITEM = Qt::UserRole + 1,
            FIRST_COL,
        };

        Model(QObject* parent = nullptr);

        bool AddItem(ModelItem);
        bool DelItem(QString Path);
        void UpdateItem(int row, ModelItem);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& Index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
        Qt::ItemFlags flags(const QModelIndex& Index) const override;

        std::vector<ModelItem> Items;
    };

    class Delegate:public QStyledItemDelegate{
        Q_OBJECT
    public:
        using QStyledItemDelegate::QStyledItemDelegate;

        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    };

    class ListView:public QListView{
        Q_OBJECT
    public:
        ListView(QWidget* parent = nullptr);
        ~ListView();

        virtual bool eventFilter(QObject* obj, QEvent* event) override;

    signals:
        void DeleteItem(QString Path);
    public:
        QModelIndex HoveredIndex;//鼠标悬停的索引
        QPushButton* Btn{ nullptr };//动态创建的按钮
    };

    //文件选择对话框

    class DlgMu:public QDialog{
        Q_OBJECT
    public:
        DlgMu(QWidget* parent = nullptr);
        ~DlgMu();

        void InitUI();
        void Connect();

        void FileSelect();
        void DataConstruct();
        void Recover();//

        void AddFile(QString file);

        FLST::FileList<FPWZ::ArgDM> FileRecord;

        QPlainTextEdit* TextEditFileSelect;
        QPushButton* BtnFileSelect;
        QPushButton* BtnOK;
        QPushButton* BtnCancel;
        Model* model;
        Delegate* delegate;
        ListView* view;
        QVBoxLayout* VlayoutFileSelect;
        QHBoxLayout* HlayoutFlieSelect;
        QGroupBox* GboxFileSelect;
        QVBoxLayout* VlayoutView;
        QHBoxLayout* HLayoutView;
        QGroupBox* GboxView;
        QHBoxLayout* HlayoutOK;

        QHBoxLayout* HLayoutFst;
        QHBoxLayout* HlayoutSec;
        QVBoxLayout* VlayoutAll;
    };
};

namespace DlgMenuARG{

    class Item{
        DCWZ::ARG_RTC_GENERATE Arg;
        TCOND::TestCondition JudgeCondCodec;
        TCOND::TestCondition JudgeCondAdpow; // 新增：每条Item独立的判定条件

    public:
        Item();
        Item(DCWZ::ARG_RTC_GENERATE);
        Item(const Item& cp);
        ~Item();

        DCWZ::ARG_RTC_GENERATE GetArg() const;
        void operator=(const Item& as);
        bool operator==(const Item& cmp) const;
        bool operator!=(const Item& cmp) const;
        /***********新增：分别存储两组条件**************/
        void SetJudgeCondCodec(const TCOND::TestCondition& cond) { JudgeCondCodec = cond; }
        void SetJudgeCondAdpow(const TCOND::TestCondition& cond) { JudgeCondAdpow = cond; }
        TCOND::TestCondition GetJudgeCondCodec() const { return JudgeCondCodec; }
        TCOND::TestCondition GetJudgeCondAdpow() const { return JudgeCondAdpow; }
        void Paint(QPainter* painter, const QStyleOptionViewItem& option, int row);
    };

    Q_DECLARE_METATYPE(Item)

    class Model: public QAbstractListModel{
        Q_OBJECT
    public:
        enum UsrRole{
            ITEM = Qt::UserRole + 1,
            ITEM_ARG,
        };

        Model(QObject* parent = nullptr);

        bool AddItem(Item);
        bool DelItem(Item);
        void UpdateItem(int, Item);
        bool ShiftUp(int);
        bool ShiftDown(int);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        std::vector<Item> Items;
    };

    class Delegate: public QStyledItemDelegate{
        Q_OBJECT
    public:
        using QStyledItemDelegate::QStyledItemDelegate;

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    };

    class ListView: public QListView{
        Q_OBJECT
    public:
        ListView(QWidget* parent = nullptr);

        virtual bool eventFilter(QObject* obj, QEvent* event) override;
    signals:
        void UpdateItemVolume(int);//更新音量
    signals:
        void DeleteItem(Item);
    signals:
        void ItemShiftUp(int);
    signals:
        void ItemShiftDN(int);
    public:
        QModelIndex HoveredIndex;
        QPushButton* BtnUpdate{ nullptr };
        QPushButton* BtnShiftUp{ nullptr };
        QPushButton* BtnShiftDown{ nullptr };
        QPushButton* BtnDelete{ nullptr };
    };

    class DlgARG: public QDialog{
        Q_OBJECT

        QLabel* LArgDB;//dB
        QLabel* LArgFreq;//KHz
        QLabel* LArgDuration;//s
        QLabel* LArgDL;//DL
        QLabel* LArgDR;//DR
        QLabel* LArgAL;//AL
        QLabel* LArgAR;//AR

        QSpinBox* SArgDB;
        QSpinBox* SArgFreq;
        QSpinBox* SArgDuration;
        QDoubleSpinBox* SArgDL;
        QDoubleSpinBox* SArgDR;
        QDoubleSpinBox* SArgAL;
        QDoubleSpinBox* SArgAR;

        QPushButton* BtnAdd;
        //QPushButton* BtnLoadTxt;//新增：从文件配置参数

        Model* model;
        Delegate* delegate;
        ListView* listView;

        QGroupBox* GBArg;
        QGroupBox* GBArgList;

        QPushButton* BtnOK;
        QPushButton* BtnCancel;

        QHBoxLayout* HLArgUp;
        QHBoxLayout* HLArgDown;
        QVBoxLayout* VLArg;

        QHBoxLayout* HLArgList;
        QVBoxLayout* VLArgList;

        QHBoxLayout* HLFst;
        QHBoxLayout* HLSec;
        QHBoxLayout* HLOK;
        
        QVBoxLayout* VLAll;

        FLST::FileList<DCWZ::ARG_RTC_GENERATE> ArgRecord;
        int DefaultArgListCnt{ 0 };
    public:
        DlgARG(QWidget* parent = nullptr);
        ~DlgARG();

        void InitUI();
        void Connect();
        void InitARG();

        void AddARG(double, unsigned long long, unsigned int, unsigned char, unsigned char, unsigned char, unsigned char); 
        //新增：重载AddARG
        void AddARG(double db, unsigned long long freq, unsigned int dur,
            unsigned char dl, unsigned char dr, unsigned char al, unsigned char ar,
            const TCOND::TestCondition& codecCond,
            const TCOND::TestCondition& adpowCond);
		//void LoadArgsFromTxt(const QString& filePath);//新增：从文件加载配置参数
		//void AddArgsFromArray(const ArgParam* arr, int count);//新增：声明一个批量添加配置参数的函数


    public slots: void AddArg();
    //public slots: void LoadArgsFromTxtSlot();//新增：声明槽函数
    public slots: void UpdateItemVolume(int);
    public slots: void DelArg(Item);
    public slots: void ShiftUp(int);
    public slots: void ShiftDown(int);
    public slots: void Recived();
    public slots: void Rejected();
    };
};

#define KEYITEM(i, key) QString("ITEM%1/").arg(i) + #key

#endif
