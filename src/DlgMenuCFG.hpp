#ifndef _DLGMENUCFG_HPP_
#define _DLGMENUCFG_HPP_

#include <QDialog>
#include <QApplication>
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
#include <QToolTip>
#include <QSpinBox>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include "TaskWZ.hpp"

namespace MenuSocketCFG{
    enum AddrType{
        LOCAL_IP,
        LOCAL_PORT,
        DEVICE_IP,
        DEVICE_PORT,
    };

    class ModelItem{
    public:
        AddrType Type;
        QString Addr;

        ModelItem();
        ModelItem(AddrType type, QString addr);
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
            SECOND_COL,
        };

        Model(QObject* parent = nullptr);

        bool AddItem(ModelItem);
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
        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        void setEditorData(QWidget* editor, const QModelIndex& index) const override;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    };

    class ReturnEventFilter: public QObject{
        Q_OBJECT
    public:
        using QObject::QObject;
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override;
    };

    class ListView: public QListView{
        Q_OBJECT
    public:
        ListView(QWidget* parent = nullptr);
        ~ListView();

        virtual bool eventFilter(QObject* obj, QEvent* event) override;

    signals:
        void ItemChanged(int row, QString text);
    public:
        QModelIndex HoveredIndex;//鼠标悬停的索引
        QPlainTextEdit* TextEdit{ nullptr };//动态创建的文本编辑框
        QLineEdit* LineEditIP{ nullptr };
        QRegularExpressionValidator* ValidatorIP;
        ReturnEventFilter* Filter;
    };

    enum SockOPT{
        CONNECT,
        DISCONNECT,
    };

    class CfgInfo{
    public:
        QString LocalIP;
        QString LocalPort;
        QString DeviceIP;
        QString DevicePort;
        SockOPT Option;

        CfgInfo();
        CfgInfo(QString localIP, QString localPort, QString deviceIP, QString devicePort, SockOPT option);
        CfgInfo(const CfgInfo& cp);
        
        void operator=(const CfgInfo& as);
        bool operator==(const CfgInfo& cmp);
        void SetOption(SockOPT opt);
    };

    class DialogSockCFG:public QDialog{
        Q_OBJECT
    private:
        Model* model;
        Delegate* delegate;
        ListView* view;
        QPushButton* BtnConnect;
        QVBoxLayout* VlayoutView;
        QHBoxLayout* HLayoutView;
        QHBoxLayout* HlayoutConnect;
        QGroupBox* GboxView;
        QPushButton* BtnOK;
        QPushButton* BtnCancel;
        QHBoxLayout* HlayoutOK;

        QHBoxLayout* HlayoutFst;
        QVBoxLayout* VlayoutAll;

        SockOPT Option;//表示当前按钮要执行的操作
        CfgInfo Record;//记录上一次配置的状态

        void InitUI();
        void Connect();

        void OKSlot();
        void CancelSlot();
        void ListItemChanged(int, QString);

    signals:
        void SockConfigComplete();
        void ConnectStatusChanged(bool);
    public slots:
        void NetLoss();
    public:
        DialogSockCFG(QWidget* parent = nullptr);
        ~DialogSockCFG();
    };
};

namespace MenuSINADCFG{
    class TabFocusEventFilter: public QObject{
        Q_OBJECT
    public:
        using QObject::QObject;
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override;
    private:
        template<typename QWidgetDerive>
        QWidget* FindNextFocusableWidget(QWidget* current);
    };

    #define SPINBOX_PROPERTY_KEY "spin_box_key"
    #define TIP_CODEC_SINAD   "输入数字表示Sinad判断范围, 最大值128"
    #define TIP_CODEC_VPP   "输入数字表示峰峰值数字量，16bit"
    #define TIP_CODEC_RMS   "输入数字表示均方根数字量，16bit"
    #define TIP_ADPOW_SINAD "输入数字表示Sinad判断范围， 最大值128"
    #define TIP_ADPOW_VPP   "输入数字表示峰峰值数字量，16bit"
    #define TIP_ADPOW_RMS   "输入数字表示均方根数字量，16bit"

    enum SpinBoxType{
        CODEC_SINAD_L,
        CODEC_SINAD_R,
        CODEC_VPP_L,
        CODEC_VPP_R,
        CODEC_RMS_L,
        CODEC_RMS_R,
        ADPOW_SINAD_L,
        ADPOW_SINAD_R,
        ADPOW_VPP_L,
        ADPOW_VPP_R,
        ADPOW_RMS_L,
        ADPOW_RMS_R,
    };

    class DialogSinadCFG:public QDialog{
        Q_OBJECT

        QLabel* LabelSinadCodec;
        QSpinBox* SpinSinadCodecLeft;
        QSpinBox* SpinSinadCodecRight;
        QPushButton* BtnCFGSinadCodec;
        QLabel* LabelVppPTPCodec;
        QSpinBox* SpinVppPTPCodecLeft;
        QSpinBox* SpinVppPTPCodecRight;
        QPushButton* BtnCFGVppPTPCodec;
        QLabel* LabelVppRMSCodec;
        QSpinBox* SpinVppRMSCodecLeft;
        QSpinBox* SpinVppRMSCodecRight;
        QPushButton* BtnCFGVppRMSCodec;
        QLabel* LabelSinadADPOW;
        QSpinBox* SpinSinadADPOWLeft;
        QSpinBox* SpinSinadADPOWRight;
        QPushButton* BtnCFGSinadADPOW;
        QLabel* LabelVppPTPADPOW;
        QSpinBox* SpinVppPTPADPOWLeft;
        QSpinBox* SpinVppPTPADPOWRight;
        QPushButton* BtnCFGVppPTPADPOW;
        QLabel* LabelVppRMSADPOW;
        QSpinBox* SpinVppRMSADPOWLeft;
        QSpinBox* SpinVppRMSADPOWRight;
        QPushButton* BtnCFGVppRMSADPOW;
        QPushButton* BtnOK;
        QPushButton* BtnCancel;
        QHBoxLayout* HlayoutSinadCodec;
        QHBoxLayout* HlayoutVppPTPCodec;
        QHBoxLayout* HlayoutVppRMSCodec;
        QVBoxLayout* VlayoutCodec;
        QGroupBox* GboxCodec;
        QHBoxLayout* HlayoutSinadADPOW;
        QHBoxLayout* HlayoutVppPTPADPOW;
        QHBoxLayout* HlayoutVppRMSADPOW;
        QVBoxLayout* VlayoutADPOW;
        QGroupBox* GboxADPOW;

        QHBoxLayout* HlayoutOK;

        QVBoxLayout* VlayoutAll;

        TabFocusEventFilter* Filter;

        std::unique_ptr<TCOND::TestCondition[]> Cond;

        void InitUI();
        void SetFocus();//设置Tab键转移光标
        void Connect();
        void CFGSinadCodecSlot();
        void CFGVppPTPCodecSlot();
        void CFGVppRMSCodecSlot();
        void CFGSinadADPOWSlot();
        void CFGVppPTPADPOWSlot();
        void CFGVppRMSADPOWSlot();
        void OKSlot();
        void CancelSlot();
    public:
        DialogSinadCFG(QWidget* parent = nullptr);
        ~DialogSinadCFG();
    }; 
};

#endif // _DLGMENUCFG_HPP_