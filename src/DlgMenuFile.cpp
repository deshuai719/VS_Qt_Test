﻿#include "DlgMenuFile.hpp"
#include "DlgMenuCFG.hpp"

#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QMessageBox>
#include <QRegularExpression>
#include "GlobalConditionList.hpp"

namespace DlgMenu{
    ModelItem::ModelItem(){}

    ModelItem::ModelItem(QString path)
        :FilePath(path)
    {}

    ModelItem::ModelItem(const ModelItem& cp)
        :FilePath(cp.FilePath)
    {}

    ModelItem::~ModelItem()
    {}

    void ModelItem::operator=(const ModelItem& as)
    {
        FilePath = as.FilePath;
    }

    void ModelItem::Paint(QPainter* painter, const QStyleOptionViewItem& option, int row)
    {
        painter->save();

        int Left = option.rect.left();
        int Top = option.rect.top();
        int Width = option.rect.width();
        int Height = option.rect.height();
        int CellWidth = Width / 10;
        QBrush Brush[3] = { QBrush(QColor(233, 233, 233)), QBrush(QColor(222, 222, 222)), QBrush(QColor(216, 216, 216)) };

        if(row > 0)
        {
            QRect Rc[3] = {
                QRect(Left, Top, CellWidth - 2, Height - 2),
                QRect(Left + CellWidth, Top, CellWidth * 8 - 2, Height - 2),
                QRect(Left + CellWidth * 9, Top, CellWidth - 2, Height - 2)
            };
            painter->setPen(QPen(Qt::black));
            QFont font("Microsoft YaHei");
            painter->setFont(font);

            painter->fillRect(Rc[0], Brush[0]);
            painter->drawText(Rc[0], QString("%1").arg(row));
            painter->fillRect(Rc[1], Brush[1]);
            painter->drawText(Rc[1], FilePath);
            painter->fillRect(Rc[2], Brush[2]);
            painter->drawText(Rc[2], Qt::AlignHCenter, "删除");
        }
        painter->restore();
    }

    Model::Model(QObject* parent)
        :QAbstractListModel(parent)
    {}

    bool Model::AddItem(ModelItem item)
    {
        
        // WRITE_DLG_FILE_MENU("Model::AddItem() Enter\n");
        for(auto it = Items.begin(); it != Items.end(); ++it)
        {
            if(it->FilePath == item.FilePath)
            {
                return false;
            }
        }
        // WRITE_DLG_FILE_MENU("beginInsertRows()\n");
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        Items.push_back(item);
        endInsertRows();
        // WRITE_DLG_FILE_MENU("Model::AddItem() End\n");
        return true;
    }

    bool Model::DelItem(QString Path)
    {
        for(auto it = Items.begin(); it != Items.end(); ++it)
        {
            if(it->FilePath == Path)
            {
                beginRemoveRows(QModelIndex(), it - Items.begin() + 1, it - Items.begin() + 1);
                Items.erase(it);
                endRemoveRows();
                return true;
            }
        }
        return false;
    }

    void Model::UpdateItem(int row, ModelItem item)
    {
        Items[row - 1] = item;
        QModelIndex index = createIndex(row, 0);
        dataChanged(index, index);
    }

    int Model::rowCount(const QModelIndex& parent) const
    {
        return static_cast<int>(Items.size()) + 1;
    }

    QVariant Model::data(const QModelIndex& index, int role) const
    {
        // WRITE_DLG_FILE_MENU( "DlgMenu::Model::data() Enter\n");
        if(index.row() > 0 && index.row() < rowCount())
        {
            switch (role)
            {
            case ITEM:         
                // WRITE_DLG_FILE_MENU("DlgMenu::Model::data() ITEM\n");
                return QVariant::fromValue(Items[index.row() - 1]);
            case FIRST_COL:     
                // WRITE_DLG_FILE_MENU("DlgMenu::Model::data() FIRST_COL\n");
                return Items[index.row() - 1].FilePath;
            default:
                break;
            }
        }
        else
        {
            return QString("已添加文件列表");
        }

        return QVariant();
    }

    bool Model::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        if(index.row() > 0 && index.row() < rowCount())
        {
            switch (role)
            {
            case ITEM:
                Items[index.row() - 1] = value.value<ModelItem>();
                return true;
            case FIRST_COL:
                Items[index.row() - 1].FilePath = value.toString();
                return true;
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

    void Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            ModelItem item = index.data(Model::ITEM).value<ModelItem>();
            item.Paint(painter, option, index.row());
            QStyledItemDelegate::paint(painter, option, index);
        }
        else if(index.row() == 0)
        {
            painter->fillRect(option.rect, QBrush(QColor(255, 255, 255)));
            QFont font("Microsoft YaHei");
            font.setWeight(QFont::Black);
            painter->setFont(font);
            painter->drawText(option.rect, Qt::AlignCenter, "已添加文件列表");
        }
    }

    QSize Delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            return QSize(option.rect.width(), 40);
        }
        else if(index.row() == 0)
        {
            return QSize(option.rect.width(), 40);
        }
        return QSize();
    }

    ListView::ListView(QWidget* parent)
        :QListView(parent)
    {
        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setEditTriggers(QAbstractItemView::DoubleClicked);

        viewport()->installEventFilter(this);
    }

    ListView::~ListView()
    {
        if(Btn)
        {
            Btn->deleteLater();
            Btn = nullptr;
        }
    }

    bool ListView::eventFilter(QObject* obj, QEvent* event)
    {
        if(obj == viewport())
        {
            if(event->type() == QEvent::HoverMove)
            {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                QModelIndex index = indexAt(mouseEvent->pos());
                // WRITE_DLG_FILE_MENU("ListView::eventFilter() HoverMove index.row() = %d\n", index.row());
                if(index.row() == 0)
                {//第一行不允许选中
                    clearSelection();
                }
                else
                {
                    if(index != HoveredIndex)
                    {   //鼠标悬停的行发生变化
                        HoveredIndex = index;
                        update(index);//更新鼠标悬停的行
                        if(Btn)
                        {
                            Btn->deleteLater();
                            Btn = nullptr;
                        }

                        if(index.isValid())//如鼠标落在无效行，则不会创建按钮
                        {
                            QRect rect = visualRect(index);
                            Btn = new QPushButton("删除", this);
                            Btn->setGeometry(rect.right() - rect.width() / 10, rect.top() + 2, rect.width() / 10, rect.height() - 3);
                            connect(Btn, &QPushButton::clicked, [this](){
                                ModelItem item = HoveredIndex.data(Model::ITEM).value<ModelItem>();
                                emit DeleteItem(item.FilePath);
                            });
                            Btn->show();                        
                        }
                    }
                }
            }
        }
        return QListView::eventFilter(obj, event);
    }

    //

    DlgMu::DlgMu(QWidget* parent)
        :QDialog(parent)
    {
        OPEN_DLG_FILE_MENU(LogFileMenu.txt);
        setWindowTitle("文件选择");
        resize(800, 600);
        InitUI();    
        Connect();
    }

    DlgMu::~DlgMu()
    {
        if(TextEditFileSelect)
        {
            TextEditFileSelect->deleteLater();
            TextEditFileSelect = nullptr;
        }

        if(BtnFileSelect)
        {
            BtnFileSelect->deleteLater();
            BtnFileSelect = nullptr;
        }

        if(BtnOK)
        {
            BtnOK->deleteLater();
            BtnOK = nullptr;
        }

        if(BtnCancel)
        {
            BtnCancel->deleteLater();
            BtnCancel = nullptr;
        }

        if(model)
        {
            model->deleteLater();
            model = nullptr;
        }

        if(delegate)
        {
            delegate->deleteLater();
            delegate = nullptr;
        }

        if(view)
        {
            view->deleteLater();
            view = nullptr;
        }

        if(VlayoutFileSelect)
        {
            VlayoutFileSelect->deleteLater();
            VlayoutFileSelect = nullptr;
        }

        if(HlayoutFlieSelect)
        {
            HlayoutFlieSelect->deleteLater();
            HlayoutFlieSelect = nullptr;
        }

        if(GboxFileSelect)
        {
            GboxFileSelect->deleteLater();
            GboxFileSelect = nullptr;
        }

        if(VlayoutView)
        {
            VlayoutView->deleteLater();
            VlayoutView = nullptr;
        }

        if(HLayoutView)
        {
            HLayoutView->deleteLater();
            HLayoutView = nullptr;
        }

        if(GboxView)
        {
            GboxView->deleteLater();
            GboxView = nullptr;
        }

        if(HlayoutOK)
        {
            HlayoutOK->deleteLater();
            HlayoutOK = nullptr;
        }

        if(HLayoutFst)
        {
            HLayoutFst->deleteLater();
            HLayoutFst = nullptr;
        }

        if(HlayoutSec)
        {
            HlayoutSec->deleteLater();
            HlayoutSec = nullptr;
        }

        if(VlayoutAll)
        {
            VlayoutAll->deleteLater();
            VlayoutAll = nullptr;
        }

        FileRecord.Clear();
        CLOSE_DLG_FILE_MENU();
    }
    
    void DlgMu::InitUI()
    {
        TextEditFileSelect = new QPlainTextEdit;
        TextEditFileSelect->setMaximumSize(800, 24);
        BtnFileSelect = new QPushButton;
        BtnFileSelect->setText("选择文件");
        BtnFileSelect->setMaximumSize(60, 24);
        BtnOK = new QPushButton;
        BtnOK->setText("确定");
        BtnOK->setMaximumSize(60, 24);
        BtnCancel = new QPushButton;
        BtnCancel->setText("取消");
        BtnCancel->setMaximumSize(60, 24);
        model = new Model;
        delegate = new Delegate;
        view = new ListView;
        view->setModel(model);
        view->setItemDelegate(delegate);
        view->setMinimumSize(600, 500);
        VlayoutFileSelect = new QVBoxLayout;
        HlayoutFlieSelect = new QHBoxLayout;
        GboxFileSelect = new QGroupBox;
        GboxFileSelect->setMaximumSize(800, 50);
        VlayoutView = new QVBoxLayout;
        HLayoutView = new QHBoxLayout;
        GboxView = new QGroupBox;
        HlayoutOK = new QHBoxLayout; 
        HLayoutFst = new QHBoxLayout;
        HlayoutSec = new QHBoxLayout;
        VlayoutAll = new QVBoxLayout;
        
        HlayoutFlieSelect->addWidget(TextEditFileSelect);
        HlayoutFlieSelect->addWidget(BtnFileSelect);
        HlayoutFlieSelect->addStretch();
        VlayoutFileSelect->addLayout(HlayoutFlieSelect);
        GboxFileSelect->setLayout(VlayoutFileSelect);

        HLayoutView->addWidget(view);
        VlayoutView->addLayout(HLayoutView);
        GboxView->setLayout(VlayoutView);

        HlayoutOK->addStretch();
        HlayoutOK->addWidget(BtnOK);
        HlayoutOK->addWidget(BtnCancel);
        
        HLayoutFst->addWidget(GboxFileSelect);
        HlayoutSec->addWidget(GboxView);

        VlayoutAll->addLayout(HLayoutFst);
        VlayoutAll->addLayout(HlayoutSec);
        VlayoutAll->addStretch();
        VlayoutAll->addLayout(HlayoutOK);

        setLayout(VlayoutAll);
    }

    void DlgMu::Connect()
    {
        connect(BtnFileSelect, &QPushButton::clicked, this, [this](){
            this->FileSelect();
        });

        connect(BtnOK, &QPushButton::clicked, this, [this](){
            accept();
        });

        connect(BtnCancel, &QPushButton::clicked, this, [this](){
            reject();
        });

        connect(view, &ListView::DeleteItem, this, [this](QString Path){
            if(model->DelItem(Path))
            {
                FLST::FileNode<FPWZ::ArgDM>Node (FPWZ::ArgDM(Path, 1), FLST::FileOPT::DEL_FIEL);
                FileRecord.AddNode(Node);
                WRITE_DLG_FILE_MENU("model->DelItem(%s)\n", Path.toStdString().c_str());
            }
        }, Qt::QueuedConnection);

        connect(this, &QDialog::accepted, this, [this](){
            // WRITE_DLG_FILE_MENU("DlgMenu::DlgMu::Connect() accept\n");
            // for(auto it = FileRecord.GetHead(); it != nullptr; it = it->GetNext())
            // {
            //     WRITE_DLG_FILE_MENU("DlgMenu::DlgMu::Connect() FileRecord.Path, = %s, Option = %d\n", it->GetPath().toStdString().c_str(), it->GetOpt());
            // }
            this->DataConstruct();
        });

        connect(this, &QDialog::rejected, this, [this](){
            this->Recover();
        });
    }

    void DlgMu::FileSelect()
    {
        WRITE_DLG_FILE_MENU("DlgMu::FileSelect() Enter\n");
        QString Path = QFileDialog::getOpenFileName(this, "选择文件", ".", "All Files(*.*)");
        if(!Path.isEmpty())
        {
            ModelItem item(Path);
            WRITE_DLG_FILE_MENU("Before AddItem()\n");
            if(model->AddItem(item))
            {
                WRITE_DLG_FILE_MENU("AddItem Return True\n");
                FLST::FileNode<FPWZ::ArgDM> Node(FPWZ::ArgDM(Path, 1), FLST::FileOPT::ADD_FILE);
                FileRecord.AddNode(Node);
            }
            TextEditFileSelect->setPlainText(Path);
        }
        WRITE_DLG_FILE_MENU("DlgMu::FileSelect() End\n");
    }

    void DlgMu::DataConstruct()
    {
        //在这里添加启动文件解析和数据生成线程的代码
        CREATE_TASK_DATA_CONSTRUCT(FileRecord);
        FileRecord.BZero();//复位链表，使其为空，以便下次使用
    }

    void DlgMu::Recover()
    {
        for(auto it = FileRecord.GetHead(); it != nullptr; it = it->GetNext())
        {
            WRITE_DLG_FILE_MENU("Recover(), Cycle Start\n");
            switch (it->GetOpt())
            {
            case FLST::FileOPT::ADD_FILE:
            {
                model->DelItem(it->GetData().GetPath());
                WRITE_DLG_FILE_MENU("Recover(), Before DelNode\n");
                FLST::FileNode<FPWZ::ArgDM> Node(FPWZ::ArgDM(it->GetData()), FLST::FileOPT::ADD_FILE);
                FileRecord.DelNode(Node);
                WRITE_DLG_FILE_MENU("Recover(), After DelNode\n");
            }
                break;
            case FLST::FileOPT::DEL_FIEL:
            {
                model->AddItem(ModelItem(it->GetData().GetPath()));
                FLST::FileNode<FPWZ::ArgDM> Node(FPWZ::ArgDM(it->GetData()), FLST::FileOPT::DEL_FIEL);
                FileRecord.DelNode(Node);
                WRITE_DLG_FILE_MENU("Recover(), AddItem(%s)\n", it->GetData().GetPath().toStdString().c_str());
            }
                break;
            default:
                break;
            }
        }
        WRITE_DLG_FILE_MENU("Recover, Cycle Over\n");
        // FileRecord.BZero();
    }

    void DlgMu::AddFile(QString file)
    {
        if(!file.isEmpty())
        {
            ModelItem item(file);
            if(model->AddItem(item))
            {
                FLST::FileNode<FPWZ::ArgDM> Node(FPWZ::ArgDM(file, 1), FLST::FileOPT::ADD_FILE);
                FileRecord.AddNode(Node);
            }
        }
    }
};

namespace DlgMenuARG{
    Item::Item(){}

    Item::Item(DCWZ::ARG_RTC_GENERATE a)
        :Arg(a),
        JudgeCondCodec(),   // 默认构造
        JudgeCondAdpow()    // 默认构造
    {}

    Item::Item(const Item& cp)
        :Arg(cp.Arg),
		//新增：补全判定条件的拷贝构造函数
        JudgeCondCodec(cp.JudgeCondCodec),
        JudgeCondAdpow(cp.JudgeCondAdpow)
    {}

    Item::~Item(){}

    DCWZ::ARG_RTC_GENERATE Item::GetArg() const
    {
        return Arg;
    }

   

    void Item::operator=(const Item& a)
    {
        Arg = a.GetArg();
        //新增：补全判定条件的
        JudgeCondCodec = a.JudgeCondCodec;
        JudgeCondAdpow = a.JudgeCondAdpow;
    }

    bool Item::operator==(const Item& cmp) const
    {
        return Arg == cmp.GetArg();
    }

    bool Item::operator!=(const Item& cmp) const
    {
        return !(Arg == cmp.GetArg());
    }

    void Item::Paint(QPainter* painter, const QStyleOptionViewItem& option, int row)
    {
        painter->save();

        int Left = option.rect.left();
        int Top = option.rect.top();
        int Width = option.rect.width();
        int Height = option.rect.height();

        int Cell = Width / 19;

        QBrush Brush[2] = { QBrush(QColor(233, 233, 233)), QBrush(QColor(222, 222, 222)) };
        QRect Rc[12] = {
            QRect(Left,             Top, Cell - 2,     Height - 2),//序号
            QRect(Left + Cell,      Top, Cell - 2,     Height - 2),//更新音量
            QRect(Left + Cell * 2,  Top, Cell - 2,     Height - 2),//上移
            QRect(Left + Cell * 3,  Top, Cell - 2,     Height - 2),//下移
            QRect(Left + Cell * 4,  Top, Cell * 2 - 2, Height - 2),//db
            QRect(Left + Cell * 6,  Top, Cell * 2 - 2, Height - 2),//freq
            QRect(Left + Cell * 8,  Top, Cell * 2 - 2, Height - 2),//dur
            QRect(Left + Cell * 10, Top, Cell * 2 - 2, Height - 2),//DL
            QRect(Left + Cell * 12, Top, Cell * 2 - 2, Height - 2),//DR
            QRect(Left + Cell * 14, Top, Cell * 2 - 2, Height - 2),//AL
            QRect(Left + Cell * 16, Top, Cell * 2 - 2, Height - 2),//AR
            QRect(Left + Cell * 18, Top, Cell - 2, Height - 2)//delete
        };

        painter->setPen(QPen(Qt::black));
        QFont font("Microsoft YaHei");
        painter->setFont(font);

        painter->fillRect(Rc[0], Brush[0]);
        painter->fillRect(Rc[1], Brush[1]);
        painter->fillRect(Rc[2], Brush[0]);
        painter->fillRect(Rc[3], Brush[1]);
        painter->fillRect(Rc[4], Brush[0]);
        painter->fillRect(Rc[5], Brush[1]);
        painter->fillRect(Rc[6], Brush[0]);
        painter->fillRect(Rc[7], Brush[1]);
        painter->fillRect(Rc[8], Brush[0]);
        painter->fillRect(Rc[9], Brush[1]);
        painter->fillRect(Rc[10], Brush[0]);
        painter->fillRect(Rc[11], Brush[1]);

        painter->drawText(Rc[0],    QString("%1").arg(row));
        painter->drawText(Rc[1],    Qt::AlignCenter, "条件");
        painter->drawText(Rc[2],    Qt::AlignCenter, "上移");
        painter->drawText(Rc[3],    Qt::AlignCenter, "下移");
        painter->drawText(Rc[4],    Qt::AlignCenter, QString("%1").arg(Arg.GetAudioARG().GetDB(), 1, 'f', 1, '0'));
        double F = Arg.GetAudioARG().GetFreq();
        painter->drawText(Rc[5],    Qt::AlignCenter, QString("%1").arg(F / 1000, 3, 'f', 1, '0'));
        painter->drawText(Rc[6],    Qt::AlignCenter, QString("%1").arg(Arg.GetAudioARG().GetDur()));
        double DL = 30 - Arg.GetRegCfgARG().GetDL() * 1.5;
        double DR = Arg.GetRegCfgARG().GetDR() * 1.0 - 18;
        double AL = Arg.GetRegCfgARG().GetAL() * 1.5 - 126;
        double AR = Arg.GetRegCfgARG().GetAR() * 1.0 - 40;
        painter->drawText(Rc[7],    Qt::AlignCenter, QString("%1").arg(DL, 1, 'f', 1, '0'));
        painter->drawText(Rc[8],    Qt::AlignCenter, QString("%1").arg(DR, 1, 'f', 1, '0'));
        painter->drawText(Rc[9],    Qt::AlignCenter, QString("%1").arg(AL, 1, 'f', 1, '0'));
        painter->drawText(Rc[10],   Qt::AlignCenter, QString("%1").arg(AR, 1, 'f', 1, '0'));
        painter->drawText(Rc[11],   Qt::AlignCenter, "删除");

        painter->restore();
    }

    //

    Model::Model(QObject* parent)
        :QAbstractListModel(parent)
    {}

    bool Model::AddItem(Item item)
    {
   /*     qDebug() << "Model::AddItem: codecCond.SINAD_L=" << item.GetJudgeCondCodec().GetRangeSINAD().GetLeft();*/
       /* for(auto mem : Items)
        {
            if(mem == item)
            {
                qDebug() << "AddItem: 重复，未添加";
                return false;
            }
        }*/

        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        Items.push_back(item);
        endInsertRows();
        qDebug() << "AddItem: 成功添加，当前总数" << Items.size();
        return true;
    }

    bool Model::DelItem(Item item)
    {
        for(auto it = Items.begin(); it != Items.end(); it++)
        {
            if(*it == item)
            {
                beginRemoveRows(QModelIndex(), it - Items.begin() + 1, it - Items.begin() + 1);
                Items.erase(it);
                endRemoveRows();
                return true;
            }
        }
        return false;
    }

    void Model::UpdateItem(int row, Item item)
    {
        Items[row - 1] = item;
        QModelIndex index = createIndex(row, 0);
        dataChanged(index, index);
    }

    bool Model::ShiftUp(int row)
    {
        if(row > 1 && row < rowCount())
        {
            Item ItemAbove = Items[row - 2];
            UpdateItem(row - 1, Items[row - 1]);
            UpdateItem(row, ItemAbove);
            return true;
        }
        return false;
    }

    bool Model::ShiftDown(int row)
    {
        if(row < rowCount() - 1 && row > 0)
        {
            Item ItemUnderlying = Items[row];
            UpdateItem(row + 1, Items[row - 1]);
            UpdateItem(row, ItemUnderlying);
            return true;
        }
        return false;
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
            case UsrRole::ITEM:     return QVariant::fromValue(Items[index.row() - 1]);
            case UsrRole::ITEM_ARG: return QVariant::fromValue(Items[index.row() - 1].GetArg());
            // case UsrRole::AMP_DB:   return QVariant(Items[index.row() - 1].GetArg().GetAudioARG().GetDB());
            // case UsrRole::FREQ:     return QVariant(Items[index.row() - 1].GetArg().GetAudioARG().GetFreq());
            // case UsrRole::DURATION: 
            //     WRITE_DLG_FILE_MENU("SlgMenuARG::Model::data(), UsrRole::DURATION, Duration = %lu\n", Items[index.row() - 1].GetArg().GetAudioARG().GetDur());
            //     return QVariant(Items[index.row() - 1].GetArg().GetAudioARG().GetDur());
            // case UsrRole::CFG_DL:   return QVariant(Items[index.row() - 1].GetArg().GetRegCfgARG().GetDL());
            // case UsrRole::CFG_DR:   return QVariant(Items[index.row() - 1].GetArg().GetRegCfgARG().GetDR());
            // case UsrRole::CFG_AL:   return QVariant(Items[index.row() - 1].GetArg().GetRegCfgARG().GetAL());
            // case UsrRole::CFG_AR:   return QVariant(Items[index.row() - 1].GetArg().GetRegCfgARG().GetAR());
            default:
                break;
            }
        }
        return QVariant();
    }

    bool Model::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        if(index.row() > 0 && index.row() < rowCount())
        {
            switch (role)
            {
            case UsrRole::ITEM:     Items[index.row() - 1] = value.value<Item>();               return true;
            case UsrRole::ITEM_ARG: Items[index.row() - 1].GetArg() = value.value<DCWZ::ARG_RTC_GENERATE>(); return true;
            // case UsrRole::AMP_DB:   Items[index.row() - 1] = value.value<char>();               return true;
            // case UsrRole::FREQ:     Items[index.row() - 1] = value.value<unsigned long long>(); return true;
            // case UsrRole::DURATION: Items[index.row() - 1] = value.value<unsigned int>();
            //     WRITE_DLG_FILE_MENU("SlgMenuARG::Model::setData(), UsrRole::DURATION, value = %lu\n", value.value<unsigned int>());
            //     return true;
            // case UsrRole::CFG_DL:   Items[index.row() - 1] = value.value<unsigned char>();      return true;
            // case UsrRole::CFG_DR:   Items[index.row() - 1] = value.value<unsigned char>();      return true;
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

    //

    void Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            index.data(Model::ITEM).value<Item>().Paint(painter, option, index.row());
        }
        else if(index.row() == 0)
        {
            painter->save();

            int Left = option.rect.left();
            int Top = option.rect.top();
            int Width = option.rect.width();
            int Height = option.rect.height();
    
            int Cell = Width / 19;

            QBrush Brush[2] = { QBrush(QColor(233, 233, 233)), QBrush(QColor(222, 222, 222)) };
            QRect Rc[12] = {
                QRect(Left,             Top, Cell - 2,     Height - 2),//序号
                QRect(Left + Cell,      Top, Cell - 2,     Height - 2),//更新音量
                QRect(Left + Cell * 2,  Top, Cell - 2,     Height - 2),//上移
                QRect(Left + Cell * 3,  Top, Cell - 2,     Height - 2),//下移
                QRect(Left + Cell * 4,  Top, Cell * 2 - 2, Height - 2),//db
                QRect(Left + Cell * 6,  Top, Cell * 2 - 2, Height - 2),//freq
                QRect(Left + Cell * 8,  Top, Cell * 2 - 2, Height - 2),//dur
                QRect(Left + Cell * 10, Top, Cell * 2 - 2, Height - 2),//DL
                QRect(Left + Cell * 12, Top, Cell * 2 - 2, Height - 2),//DR
                QRect(Left + Cell * 14, Top, Cell * 2 - 2, Height - 2),//AL
                QRect(Left + Cell * 16, Top, Cell * 2 - 2, Height - 2),//AR
                QRect(Left + Cell * 18, Top, Cell - 2, Height - 2)//delete
            };

            painter->setPen(QPen(Qt::black));
            QFont font("Microsoft YaHei");
            painter->setFont(font);

            painter->fillRect(Rc[0], Brush[0]);
            painter->fillRect(Rc[1], Brush[1]);
            painter->fillRect(Rc[2], Brush[0]);
            painter->fillRect(Rc[3], Brush[1]);
            painter->fillRect(Rc[4], Brush[0]);
            painter->fillRect(Rc[5], Brush[1]);
            painter->fillRect(Rc[6], Brush[0]);
            painter->fillRect(Rc[7], Brush[1]);
            painter->fillRect(Rc[8], Brush[0]);
            painter->fillRect(Rc[9], Brush[1]);
            painter->fillRect(Rc[10], Brush[0]);
            painter->fillRect(Rc[11], Brush[1]);

            painter->drawText(Rc[0], "序号");
            painter->drawText(Rc[1], "条件");
            painter->drawText(Rc[2], "上移");
            painter->drawText(Rc[3], "下移");
            painter->drawText(Rc[4], Qt::AlignCenter, "幅度/dB");
            painter->drawText(Rc[5], Qt::AlignCenter, "频率/KHz");
            painter->drawText(Rc[6], Qt::AlignCenter, "持续时间/s");
            painter->drawText(Rc[7], Qt::AlignCenter, "Digital/dB");
            painter->drawText(Rc[8], Qt::AlignCenter, "PGA/dB");
            painter->drawText(Rc[9], Qt::AlignCenter, "Playback/dB");
            painter->drawText(Rc[10], Qt::AlignCenter, "Headset/dB");
            painter->drawText(Rc[11], "删除");

            painter->restore();
        }
    }

    QSize Delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        return QSize(option.rect.width(), 20);
    }

    //

    ListView::ListView(QWidget* parent)
        :QListView(parent)
    {
        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setEditTriggers(QAbstractItemView::DoubleClicked);

        viewport()->installEventFilter(this);
    }

    bool ListView::eventFilter(QObject* obj, QEvent* event)
    {
        if(obj == viewport())
        {
            if(event->type() == QEvent::HoverMove)
            {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                QModelIndex index = indexAt(mouseEvent->pos());
                // WRITE_DLG_FILE_MENU("ListView::eventFilter() HoverMove index.row() = %d\n", index.row());
                if(index.row() == 0)
                {//第一行不允许选中
                    clearSelection();
                }
                else
                {
                    if(index != HoveredIndex)
                    {   //鼠标悬停的行发生变化
                        HoveredIndex = index;
                        update(index);//更新鼠标悬停的行
                        if(BtnUpdate)
                        {
                            BtnUpdate->deleteLater();
                            BtnUpdate = nullptr;
                        }
                        if(BtnShiftUp)
                        {
                            BtnShiftUp->deleteLater();
                            BtnShiftUp = nullptr;
                        }

                        if(BtnShiftDown)
                        {
                            BtnShiftDown->deleteLater();
                            BtnShiftDown = nullptr;
                        }

                        if(BtnDelete)
                        {
                            BtnDelete->deleteLater();
                            BtnDelete = nullptr;
                        }

                        if(index.isValid())//如鼠标落在无效行，则不会创建按钮
                        {
                            QRect rect = visualRect(index);
                            BtnUpdate = new QPushButton("更新", this);
                            BtnUpdate->setGeometry(rect.left() + rect.width() / 19, rect.top() + 2, rect.width() / 19 - 2, rect.height() - 2);
                            connect(BtnUpdate, &QPushButton::clicked, [this](){
                                emit UpdateItemVolume(HoveredIndex.row());
                            });
                            BtnUpdate->show();

                            BtnShiftUp = new QPushButton("上移", this);
                            BtnShiftUp->setGeometry(rect.left() + rect.width() * 2 / 19, rect.top() + 2,
                                rect.width() / 19 - 2, rect.height() - 2);
                            connect(BtnShiftUp, &QPushButton::clicked, [this](){
                                if(HoveredIndex.row() > 1)
                                {
                                    emit ItemShiftUp(HoveredIndex.row());
                                    QModelIndex IndexAbove = model()->index(HoveredIndex.row() - 1, 0);
                                    QRect ItemAboveRc = visualRect(IndexAbove);
                                    QRect AboveBtnRc(ItemAboveRc.left() + ItemAboveRc.width() * 2 / 19,
                                        ItemAboveRc.top(), ItemAboveRc.width() / 19,  ItemAboveRc.height());
                                    QCursor::setPos(viewport()->mapToGlobal(AboveBtnRc.center()));//移动鼠标到上一行按钮的中心
                                }
                            }); 
                            BtnShiftUp->show();

                            BtnShiftDown = new QPushButton("下移", this);
                            BtnShiftDown->setGeometry(rect.left() + rect.width() * 3 / 19, rect.top() + 2,
                                rect.width() / 19 - 2, rect.height() - 2);
                            connect(BtnShiftDown, &QPushButton::clicked, [this](){
                                if(HoveredIndex.row() < model()->rowCount() - 1)
                                {
                                    emit ItemShiftDN(HoveredIndex.row());
                                    QModelIndex IndexBelow = model()->index(HoveredIndex.row() + 1, 0);
                                    QRect ItemBelowRc = visualRect(IndexBelow);
                                    QRect BelowBtnRc(ItemBelowRc.left() + ItemBelowRc.width() * 3 / 19,
                                        ItemBelowRc.top(), ItemBelowRc.width() / 19,  ItemBelowRc.height());
                                    QCursor::setPos(viewport()->mapToGlobal(BelowBtnRc.center()));
                                }
                            });
                            BtnShiftDown->show();

                            BtnDelete = new QPushButton("删除", this);
                            BtnDelete->setGeometry(rect.left() + rect.width() * 18 / 19 - 4, rect.top() + 2, rect.width() / 19 - 2, rect.height() - 2);
                            connect(BtnDelete, &QPushButton::clicked, [this](){
                                if(HoveredIndex.row() == model()->rowCount() - 1)
                                {
                                    QModelIndex indexAbove = model()->index(HoveredIndex.row() - 1, 0);
                                    QRect ItemAboveRc = visualRect(indexAbove);
                                    QRect AboveBtnRc(ItemAboveRc.left() + ItemAboveRc.width() * 18 / 19,
                                        ItemAboveRc.top(), ItemAboveRc.width() / 19, ItemAboveRc.height());
                                    QCursor::setPos(viewport()->mapToGlobal(AboveBtnRc.center()));
                                }
                                emit DeleteItem(HoveredIndex.data(Model::ITEM).value<Item>());
                            });
                            BtnDelete->show();                        
                        }
                    }
                }
            }
        }
        return QListView::eventFilter(obj, event);
    }

    //

    DlgARG::DlgARG(QWidget* parent)
        :QDialog(parent)
    {
        OPEN_DLG_MENU_AUDIO_ARG_DBG(LogMenuAudioArg.txt);
        InitUI();
        Connect();
        InitARG();
    }

    DlgARG::~DlgARG(){
        CLOSE_DLG_MENU_AUDIO_ARG_DBG();
    }

    // LArgDB
    // LArgFreq
    // LArgDuration
    // SArgDB
    // SArgFreq
    // SArgDuration
    // BtnAdd
    // GBArg
    // BtnOK
    // BtnCancel

    void DlgARG::InitUI()
    {
        resize(800, 800);
        setWindowTitle("添加参数");

        LArgDB = new QLabel("振幅");//, this);
        LArgFreq = new QLabel("频率");//, this);
        LArgDuration = new QLabel("持续时间");//, this);
        LArgDL = new QLabel("Digital");
        LArgDR = new QLabel("PGA");
        LArgAL = new QLabel("Playback");
        LArgAR = new QLabel("Headset");

        SArgDB = new QSpinBox;//(this);
        SArgFreq = new QSpinBox;//(this);
        SArgDuration = new QSpinBox;//(this);
        SArgDL = new QDoubleSpinBox;
        SArgDR = new QDoubleSpinBox;
        SArgAL = new QDoubleSpinBox;
        SArgAR = new QDoubleSpinBox;

        BtnAdd = new QPushButton("添加");//, this);
        //BtnLoadTxt = new QPushButton("文件配置");//新增：实例化文件配置按钮

        model = new Model;//(this);
        delegate = new Delegate;//(this);
        listView = new ListView;//(this);

        listView->setModel(model);
        listView->setItemDelegate(delegate);
        listView->setMinimumSize(600, 400);

        GBArg = new QGroupBox;//(this);
        GBArgList = new QGroupBox;//(this);
        BtnOK = new QPushButton("确认");//,;// this);
        BtnCancel = new QPushButton("取消");//,;// this);
        HLArgUp = new QHBoxLayout;//(this);
        HLArgDown = new QHBoxLayout;//(this);
        VLArg = new QVBoxLayout;//(this);
        HLArgList = new QHBoxLayout;//(this);
        VLArgList = new QVBoxLayout;//(this);
        HLFst = new QHBoxLayout;//(this);
        HLSec = new QHBoxLayout;//(this);
        HLOK = new QHBoxLayout;//(this);
        VLAll = new QVBoxLayout;//(this);

        LArgDB->        setFixedSize(60, 24);
        LArgFreq->      setFixedSize(60, 24);
        LArgDuration->  setFixedSize(60, 24);
        LArgDL->        setFixedSize(60, 24);
        LArgDR->        setFixedSize(60, 24);
        LArgAL->        setFixedSize(60, 24);
        LArgAR->        setFixedSize(60, 24);
        SArgDB->        setFixedSize(80, 24);
        SArgFreq->      setFixedSize(80, 24);
        SArgDuration->  setFixedSize(80, 24);
        SArgDL->        setFixedSize(80, 24);
        SArgDR->        setFixedSize(80, 24);
        SArgAL->        setFixedSize(80, 24);
        SArgAR->        setFixedSize(80, 24);
        BtnAdd->        setFixedSize(80, 24);
		//BtnLoadTxt->    setFixedSize(80, 24);//新增：实例化文件配置按钮
        BtnOK->         setFixedSize(80, 24);
        BtnCancel->     setFixedSize(80, 24);
        
        SArgDB->setRange(-100, 100);
        SArgFreq->setRange(0, 100);
        SArgDuration->setRange(1, 100);
        SArgDL->setRange(-96, 30);//
        SArgDR->setRange(-96, 30);//
        SArgAL->setRange(-18, 42);//
        SArgAR->setRange(-18, 42);//

        SArgDB->setSingleStep(1);
        SArgFreq->setSingleStep(1);
        SArgDuration->setSingleStep(1);
        SArgDL->setSingleStep(1.5);
        SArgDR->setSingleStep(1);
        SArgAL->setSingleStep(1.5);
        SArgAR->setSingleStep(1);

        SArgDB->setSuffix("dB");
        SArgFreq->setSuffix("KHz");
        SArgDuration->setSuffix("s");
        SArgDL->setSuffix("dB");
        SArgDR->setSuffix("dB");
        SArgAL->setSuffix("dB");
        SArgAR->setSuffix("dB");

        SArgDB->setValue(1);
        SArgFreq->setValue(0);
        SArgDuration->setValue(1);

        HLArgUp->addWidget(LArgDB);
        HLArgUp->addWidget(SArgDB);
        HLArgUp->addSpacing(10);
        HLArgUp->addWidget(LArgFreq);
        HLArgUp->addWidget(SArgFreq);
        HLArgUp->addSpacing(10);
        HLArgUp->addWidget(LArgDuration);
        HLArgUp->addWidget(SArgDuration);
		//HLArgDown->addWidget(BtnLoadTxt);//新增：添加文件配置按钮
        HLArgUp->addStretch();

        HLArgDown->addWidget(LArgDL);
        HLArgDown->addWidget(SArgDL);
        HLArgDown->addSpacing(10);
        HLArgDown->addWidget(LArgDR);
        HLArgDown->addWidget(SArgDR);
        HLArgDown->addSpacing(10);
        HLArgDown->addWidget(LArgAL);
        HLArgDown->addWidget(SArgAL);
        HLArgDown->addSpacing(10);
        HLArgDown->addWidget(LArgAR);
        HLArgDown->addWidget(SArgAR);
        HLArgDown->addSpacing(10);
        HLArgDown->addWidget(BtnAdd);
        HLArgDown->addStretch();
        
        VLArg->addLayout(HLArgUp);
        VLArg->addLayout(HLArgDown);

        GBArg->setLayout(VLArg);

        HLArgList->addWidget(listView);
        VLArgList->addLayout(HLArgList);

        GBArgList->setLayout(VLArgList);

        HLFst->addWidget(GBArg);
        HLSec->addWidget(GBArgList);

        HLOK->addStretch();
        HLOK->addWidget(BtnOK);
        HLOK->addWidget(BtnCancel);

        VLAll->addLayout(HLFst);
        VLAll->addLayout(HLSec);
        VLAll->addLayout(HLOK);

        setLayout(VLAll);
    }

    void DlgARG::Connect()
    {
        connect(BtnAdd, &QPushButton::clicked, this, &DlgARG::AddArg);

		//connect(BtnLoadTxt, &QPushButton::clicked, this, &DlgARG::LoadArgsFromTxtSlot); //新增：连接文件配置按钮

        connect(listView, &ListView::UpdateItemVolume, this, &DlgARG::UpdateItemVolume);

        connect(listView, &ListView::DeleteItem, this, &DlgARG::DelArg);

        connect(listView, &ListView::ItemShiftUp, this, &DlgARG::ShiftUp);

        connect(listView, &ListView::ItemShiftDN, this, &DlgARG::ShiftDown);

        connect(BtnOK, &QPushButton::clicked, this, &QDialog::accept);

        connect(BtnCancel, &QPushButton::clicked, this, &QDialog::reject);

        connect(this, &QDialog::accepted, this, &DlgARG::Recived);

        connect(this, &QDialog::rejected, this, &DlgARG::Rejected);
    }

    void DlgARG::InitARG()
    {
        // int rowCount = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_ARG_RECORD, "NUMBER/rowCount").toInt();
        int rowCount = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, "NUMBER/rowCount").toInt();
        qDebug() << "rowCount=" << rowCount;
        g_ConditionList.clear();
        g_ParamList.clear();
        for(int i = 0; i < rowCount; i++)
        {

            //qDebug() << "正在读取第" << i << "条";
            double dB = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, DB)).toDouble();
            unsigned long long Freq = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Freq)).toULongLong();
            unsigned int Dur = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Dur)).toUInt();
            double dl = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Digital)).toDouble();
            double dr = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, PGA)).toDouble();
            double al = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Playback)).toDouble();
            double ar = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Headset)).toDouble();

            DCWZ::ARG_RTC_GENERATE param(
                SDG::ARG(dB, Freq, Dur),
                DCWZ::ArgRegCFG<unsigned char>(
                    static_cast<unsigned char>(dl),
                    static_cast<unsigned char>(dr),
                    static_cast<unsigned char>(al),
                    static_cast<unsigned char>(ar)
                )
            );
            g_ParamList.append(param); // 新增：存储参数
            //qDebug() << "dB=" << dB << "Freq=" << Freq << "Dur=" << Dur << "dl=" << (int)dl << "dr=" << (int)dr << "al=" << (int)al << "ar=" << (int)ar;
            // 新增：读取判定条件
            TCOND::TestCondition codecCond, adpowCond;
            // 新增：codec
            int codecSinadL = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Codec_SINAD_L)).toInt();
            int codecSinadR = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Codec_SINAD_R)).toInt();
     /*       qDebug() << "Codec_SINAD_L:" << codecSinadL;*/
            codecCond.GetRangeSINAD().SetRange(codecSinadL, codecSinadR);

            int codecVppPTPL = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Codec_VppPTP_L)).toInt();
            int codecVppPTPR = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Codec_VppPTP_R)).toInt();
            codecCond.GetRangeVppPTP().SetRange(codecVppPTPL, codecVppPTPR);

            int codecVppRMSL = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Codec_VppRMS_L)).toInt();
            int codecVppRMSR = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Codec_VppRMS_R)).toInt();
            codecCond.GetRangeVppRMS().SetRange(codecVppRMSL, codecVppRMSR);

            // 新增： adpow
            int adpowSinadL = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Adpow_SINAD_L)).toInt();
            int adpowSinadR = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Adpow_SINAD_R)).toInt();
            adpowCond.GetRangeSINAD().SetRange(adpowSinadL, adpowSinadR);

            int adpowVppPTPL = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Adpow_VppPTP_L)).toInt();
            int adpowVppPTPR = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Adpow_VppPTP_R)).toInt();
            adpowCond.GetRangeVppPTP().SetRange(adpowVppPTPL, adpowVppPTPR);

            int adpowVppRMSL = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Adpow_VppRMS_L)).toInt();
            int adpowVppRMSR = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, KEYITEM(i, Adpow_VppRMS_R)).toInt();
            adpowCond.GetRangeVppRMS().SetRange(adpowVppRMSL, adpowVppRMSR);

            //qDebug() << "InitARG i=" << i << "Codec_SINAD_L=" << codecSinadL << "Codec_SINAD_R=" << codecSinadR;
            AddARG(dB, Freq, Dur, -(dl - 30) / 1.5, dr + 18, (al + 126) / 1.5, ar + 40, codecCond, adpowCond);
           
            g_ConditionList.append({ codecCond, adpowCond });
            DCR::DeviceCheckResultGlobal->GetCondition()[0] = codecCond;
            DCR::DeviceCheckResultGlobal->GetCondition()[1] = adpowCond;
            // 加在这里
            qDebug() << "AddARG finished for i=" << i << ", 当前model->Items.size()=" << model->Items.size();
        }

        TOOLWZ::stack_wz<TASKWZ::task_type> stack_join;
        stack_join.push(TASKWZ::task_type::TASK_END);
        stack_join.push(TASKWZ::task_type::TASK_JOIN);
        stack_join.push(TASKWZ::task_type::TASK_DATA_CONSTRUCT);
        stack_join.push(TASKWZ::task_type::TASK_DATA_SEND);
        CREATE_TASK_JOIN(stack_join);
        CREATE_TASK_DATA_CONSTRUCT_ARG(ArgRecord);
        ArgRecord.BZero();
    }

    void DlgARG::AddARG(double db, unsigned long long freq, unsigned int dur,
        unsigned char dl, unsigned char dr, unsigned char al, unsigned char ar,
        const TCOND::TestCondition& codecCond,
        const TCOND::TestCondition& adpowCond)
    {
        ////qDebug() << "AddARG: codecCond.SINAD_L=" << codecCond.GetRangeSINAD().GetLeft()
        //    << "codecCond.SINAD_R=" << codecCond.GetRangeSINAD().GetRight();
        Item item(DCWZ::ARG_RTC_GENERATE(SDG::ARG(db, freq, dur), DCWZ::ArgRegCFG(dl, dr, al, ar)));
        item.SetJudgeCondCodec(codecCond);
        item.SetJudgeCondAdpow(adpowCond);

        if (model->AddItem(item))
        {
            FLST::FileNode<DCWZ::ARG_RTC_GENERATE> NodeAdd(
                DCWZ::ARG_RTC_GENERATE(SDG::ARG(db, freq, dur), DCWZ::ArgRegCFG<unsigned char>(dl, dr, al, ar)),
                FLST::FileOPT::ADD_FILE);
            if (!ArgRecord.AddNode(NodeAdd))
            {
                FLST::FileNode<DCWZ::ARG_RTC_GENERATE> NodeTB(
                    DCWZ::ARG_RTC_GENERATE(SDG::ARG(db, freq, dur), DCWZ::ArgRegCFG<unsigned char>(dl, dr, al, ar)),
                    FLST::FileOPT::TO_BOTTOM);
                ArgRecord.AddNode(NodeTB);
            }
        }
        /*qDebug() << "AddARG: codecCond.SINAD_L=" << codecCond.GetRangeSINAD().GetLeft()
            << "codecCond.SINAD_R=" << codecCond.GetRangeSINAD().GetRight();*/
    }

    void  DlgARG::AddARG(double db, unsigned long long freq, unsigned int dur,
        unsigned char dl, unsigned char dr, unsigned char al, unsigned char ar)
    {
       
        // 新增：初始化判定条件（可根据实际情况设定默认值）
        TCOND::TestCondition defaultCodecCond;  // 你可以自定义默认值
        TCOND::TestCondition defaultAdpowCond;  // 你可以自定义默认值

        AddARG(db, freq, dur, dl, dr, al, ar, defaultCodecCond, defaultAdpowCond);

    }
    
    void DlgARG::AddArg()
    {
        char db = SArgDB->value();
        unsigned long long freq = SArgFreq->value() * 1000;
        unsigned int dur = SArgDuration->value();
        double dl = SArgDL->value();
        double dr = SArgDR->value();
        double al = SArgAL->value();
        double ar = SArgAR->value();

        if(freq == 0)//如果初始频率为零则循环添加数据
        {
            double dB[4] = { -1.0, -7.0, -13.0, -19.0 };
            unsigned long long Freq[3] = { 1000, 13000, 25000 };
            AddARG(dB[DefaultArgListCnt / 3], Freq[DefaultArgListCnt % 3], dur, -(dl - 30) / 1.5, dr + 18 , (al + 126) / 1.5, ar + 40);
            //AddARG(dB[DefaultArgListCnt / 3], Freq[DefaultArgListCnt % 3], dur, dl, dr, al, ar);
            DefaultArgListCnt = (DefaultArgListCnt + 1) % 12;
        }
        else
        {
            //AddARG(db, freq, dur, dl, dr, al, ar);
            AddARG(db, freq, dur, -(dl - 30) / 1.5, dr + 18, (al + 126) / 1.5, ar + 40);
        }
    } 

    /************新增*****************/
    //void DlgARG::LoadArgsFromTxtSlot()
    //{
    //    QString filePath = QFileDialog::getOpenFileName(this, "选择TXT文件", ".", "Text Files (*.txt);;All Files (*.*)");
    //    if (!filePath.isEmpty()) {
    //        LoadArgsFromTxt(filePath); // 你实现的批量导入函数
    //    }
    //}

    //void DlgARG::LoadArgsFromTxt(const QString& filePath)
    //{
    //    QFile file(filePath);
    //    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //        QMessageBox::warning(this, "文件打开失败", "无法打开指定的TXT文件！");
    //        return;
    //    }

    //    QVector<ArgParam> params;
    //    QTextStream in(&file);
    //    while (!in.atEnd()) {
    //        QString line = in.readLine();
    //        if (line.trimmed().isEmpty()) continue;

    //        QStringList tokens = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    //        if (tokens.size() < 7) continue; // 跳过无效行

    //        ArgParam param;
    //        param.dB = tokens[0].toDouble();
    //        param.Freq = tokens[1].toULongLong();
    //        param.Dur = tokens[2].toUInt();
    //        param.dl = static_cast<unsigned char>(tokens[3].toUInt());
    //        param.dr = static_cast<unsigned char>(tokens[4].toUInt());
    //        param.al = static_cast<unsigned char>(tokens[5].toUInt());
    //        param.ar = static_cast<unsigned char>(tokens[6].toUInt());
    //        params.append(param);
    //    }
    //    file.close();

    //    if (!params.isEmpty()) {
    //        AddArgsFromArray(params.data(), params.size());
    //    }
    //}




    //void DlgARG::AddArgsFromArray(const ArgParam* arr, int count)
    //{
    //    for (int i = 0; i < count; ++i) {
    //        AddARG(arr[i].dB, arr[i].Freq, arr[i].Dur, arr[i].dl, arr[i].dr, arr[i].al, arr[i].ar);
    //    }
    //}//新增：批量添加参数的具体实现


    void DlgARG::UpdateItemVolume(int row)
    {
        MenuSINADCFG::DialogSinadCFG dlg(this);
        // 新增：分别取出 codec/adpow 的判定条件
        const TCOND::TestCondition& codecCond = model->Items[row - 1].GetJudgeCondCodec();
        //qDebug() << "UpdateItemVolume: codecCond.SINAD_L=" << codecCond.GetRangeSINAD().GetLeft();
        const TCOND::TestCondition& adpowCond = model->Items[row - 1].GetJudgeCondAdpow();
        dlg.SetTestCondition(codecCond, adpowCond);
        if (dlg.exec() == QDialog::Accepted) {
            // 新增：分别获取并保存两组条件
            TCOND::TestCondition newCodecCond = dlg.GetCodecTestCondition();
            TCOND::TestCondition newAdpowCond = dlg.GetAdpowTestCondition();
            model->Items[row - 1].SetJudgeCondCodec(newCodecCond);
            model->Items[row - 1].SetJudgeCondAdpow(newAdpowCond);
        }
    }
       /* double dl = SArgDL->value();
        double dr = SArgDR->value();
        double al = SArgAL->value();
        double ar = SArgAR->value();

        DCWZ::ARG_RTC_GENERATE OldARG = model->Items[row - 1].GetArg();
        DCWZ::ARG_RTC_GENERATE NewARG(model->Items[row - 1].GetArg().GetAudioARG(), DCWZ::ArgRegCFG<unsigned char>(-(dl - 30) / 1.5, dr + 18, (al + 126) / 1.5, ar + 40));
        if(OldARG == NewARG)
        {
            return;
        }
        model->UpdateItem(row, Item(NewARG));
        FLST::FileNode<DCWZ::ARG_RTC_GENERATE> NodeDel(OldARG, FLST::FileOPT::DEL_FIEL);
        ArgRecord.AddNode(NodeDel);
        FLST::FileNode<DCWZ::ARG_RTC_GENERATE> NodeAdd(NewARG, FLST::FileOPT::ADD_FILE);
        ArgRecord.AddNode(NodeAdd);
        for(int i = model->rowCount() - 2; i > 0 && model->Items[i - 1] != model->Items[row - 2]; i--)
        {
            FLST::FileNodeEXCH<DCWZ::ARG_RTC_GENERATE> NodeExch(NewARG, model->Items[i - 1].GetArg());
            ArgRecord.AddNode(NodeExch);
        }*/
    

    void DlgARG::DelArg(Item im)
    {
        if(model->DelItem(im))
        {
            FLST::FileNode<DCWZ::ARG_RTC_GENERATE> Node(im.GetArg(), FLST::FileOPT::DEL_FIEL);
            ArgRecord.AddNode(Node);
        }
    }

    void DlgARG::ShiftUp(int row)
    {
        if(model->ShiftUp(row))
        {
            // WRITE_DLG_MENU_AUDIO_ARG_DBG("DlgARG::ShiftUp(), Arg: dB = %f, Freq = %llu, DL = %x, DR = %x, AL = %x, AR = %x\n", 
            //     model->Items[row - 2].GetArg().GetAudioARG().GetDB(), model->Items[row - 2].GetArg().GetAudioARG().GetFreq(),
            //     model->Items[row - 2].GetArg().GetRegCfgARG().GetDL(), model->Items[row - 2].GetArg().GetRegCfgARG().GetDR(),
            //     model->Items[row - 2].GetArg().GetRegCfgARG().GetAL(), model->Items[row - 2].GetArg().GetRegCfgARG().GetAR());
            // WRITE_DLG_MENU_AUDIO_ARG_DBG("DlgARG::ShiftUp(), ArgEXCHG: dB = %f, Freq = %llu, DL = %x, DR = %x, AL = %x, AR = %x\n", 
            //     model->Items[row - 1].GetArg().GetAudioARG().GetDB(), model->Items[row - 1].GetArg().GetAudioARG().GetFreq(),
            //     model->Items[row - 1].GetArg().GetRegCfgARG().GetDL(), model->Items[row - 1].GetArg().GetRegCfgARG().GetDR(),
            //     model->Items[row - 1].GetArg().GetRegCfgARG().GetAL(), model->Items[row - 1].GetArg().GetRegCfgARG().GetAR());
            FLST::FileNodeEXCH<DCWZ::ARG_RTC_GENERATE> Node(model->Items[row - 2].GetArg(), model->Items[row - 1].GetArg());
            if(ArgRecord.AddNode(Node))
            {
                // WRITE_DLG_MENU_AUDIO_ARG_DBG("DlgARG::ShiftUp(), row = %d\n", row);
            }
            else
            {
                // WRITE_DLG_MENU_AUDIO_ARG_DBG("DlgARG::ShiftUp(), row = %d, failed\n", row);
            }
        }
    }

    void DlgARG::ShiftDown(int row)
    {
        if(model->ShiftDown(row))
        {
            FLST::FileNodeEXCH<DCWZ::ARG_RTC_GENERATE> Node(model->Items[row].GetArg(), model->Items[row - 1].GetArg());
            ArgRecord.AddNode(Node);
        }
    }
    

    void DlgARG::Recived()
    {
        // for(int i = 0; i < model->rowCount() - 1; i++)WriteINI
        // {
        //     WRITE_DLG_MENU_AUDIO_ARG_DBG("DgMenuARG::DlgARG::Recived(), dB = %f, Freq = %llu, DL = %x, DR = %x, AL = %x, AR = %x\n", 
        //         model->Items[i].GetArg().GetAudioARG().GetDB(), model->Items[i].GetArg().GetAudioARG().GetFreq(),
        //         model->Items[i].GetArg().GetRegCfgARG().GetDL(), model->Items[i].GetArg().GetRegCfgARG().GetDR(),
        //         model->Items[i].GetArg().GetRegCfgARG().GetAL(), model->Items[i].GetArg().GetRegCfgARG().GetAR());
        // }
        TOOLWZ::stack_wz<TASKWZ::task_type> stack_join;
        stack_join.push(TASKWZ::task_type::TASK_END);
        stack_join.push(TASKWZ::task_type::TASK_JOIN);
        stack_join.push(TASKWZ::task_type::TASK_DATA_CONSTRUCT);
        stack_join.push(TASKWZ::task_type::TASK_DATA_SEND);
        CREATE_TASK_JOIN(stack_join);
        CREATE_TASK_DATA_CONSTRUCT_ARG(ArgRecord);
        ArgRecord.BZero();

        CFGI::IniFileCFGGlobal->ClearINI(CFGI::INI_CENTRALIZE);
        CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NUMBER/rowCount", QString("%1").arg(model->rowCount() - 1));
        for(int i = 0; i < model->rowCount() - 1; i++)
        {

            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/DB").arg(i), QString("%1").arg(model->Items[i].GetArg().GetAudioARG().GetDB()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Freq").arg(i), QString("%1").arg(model->Items[i].GetArg().GetAudioARG().GetFreq()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Dur").arg(i), QString("%1").arg(model->Items[i].GetArg().GetAudioARG().GetDur()));
            const auto& arg = model->Items[i].GetArg().GetRegCfgARG();
            double dl_db = 30 - arg.GetDL() * 1.5;
            double dr_db = arg.GetDR() * 1.0 - 18;
            double al_db = arg.GetAL() * 1.5 - 126;
            double ar_db = arg.GetAR() * 1.0 - 40;

            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Digital").arg(i), QString::number(dl_db));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/PGA").arg(i), QString::number(dr_db));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Playback").arg(i), QString::number(al_db));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Headset").arg(i), QString::number(ar_db));

            //新增： 保存判定条件
            const TCOND::TestCondition& codecCond = model->Items[i].GetJudgeCondCodec();
            const TCOND::TestCondition& adpowCond = model->Items[i].GetJudgeCondAdpow();

            // 新增：codec
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Codec_SINAD_L").arg(i), QString::number(codecCond.GetRangeSINAD().GetLeft()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Codec_SINAD_R").arg(i), QString::number(codecCond.GetRangeSINAD().GetRight()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Codec_VppPTP_L").arg(i), QString::number(codecCond.GetRangeVppPTP().GetLeft()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Codec_VppPTP_R").arg(i), QString::number(codecCond.GetRangeVppPTP().GetRight()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Codec_VppRMS_L").arg(i), QString::number(codecCond.GetRangeVppRMS().GetLeft()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Codec_VppRMS_R").arg(i), QString::number(codecCond.GetRangeVppRMS().GetRight()));

            // 新增：adpow
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Adpow_SINAD_L").arg(i), QString::number(adpowCond.GetRangeSINAD().GetLeft()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Adpow_SINAD_R").arg(i), QString::number(adpowCond.GetRangeSINAD().GetRight()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Adpow_VppPTP_L").arg(i), QString::number(adpowCond.GetRangeVppPTP().GetLeft()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Adpow_VppPTP_R").arg(i), QString::number(adpowCond.GetRangeVppPTP().GetRight()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Adpow_VppRMS_L").arg(i), QString::number(adpowCond.GetRangeVppRMS().GetLeft()));
            CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_TYPE::INI_CENTRALIZE, QString("ITEM%1/Adpow_VppRMS_R").arg(i), QString::number(adpowCond.GetRangeVppRMS().GetRight()));
        }
    }

    void DlgARG::Rejected()
    {
        for(auto it = ArgRecord.GetHead(); it != nullptr;)
        {
            switch (it->GetOpt())
            {
            case FLST::FileOPT::ADD_FILE:
                if(model->DelItem(Item(it->GetData())))
                {
                    FLST::FileNode<DCWZ::ARG_RTC_GENERATE> Node(DCWZ::ARG_RTC_GENERATE(it->GetData()), FLST::FileOPT::DEL_FIEL);
                    it = it->GetNext();
                    ArgRecord.AddNode(Node);
                }
                else
                {
                    it = it->GetNext();
                }
                break;
            case FLST::FileOPT::DEL_FIEL:
                if(model->AddItem(Item(it->GetData())))
                {
                    FLST::FileNode<DCWZ::ARG_RTC_GENERATE> Node(DCWZ::ARG_RTC_GENERATE(it->GetData()), FLST::FileOPT::ADD_FILE);
                    it = it->GetNext();
                    if(!ArgRecord.AddNode(Node))
                    {
                        FLST::FileNode<DCWZ::ARG_RTC_GENERATE> Node(DCWZ::ARG_RTC_GENERATE(it->GetData()), FLST::FileOPT::TO_BOTTOM);
                        ArgRecord.AddNode(Node);
                    }
                    else
                    {
                        it = it->GetNext();
                    }
                }
                else
                {
                    it = it->GetNext();
                }
                break;
            default:
                it = it->GetNext();
                break;
            }
        }
    }
};