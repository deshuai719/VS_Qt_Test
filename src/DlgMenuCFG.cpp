#include "DlgMenuCFG.hpp"
#include <QDebug>

namespace MenuSocketCFG{
    ModelItem::ModelItem()
        :Type(AddrType::LOCAL_IP), Addr("")
    {}

    ModelItem::ModelItem(AddrType type, QString addr)
        :Type(type), Addr(addr)
    {}

    ModelItem::ModelItem(const ModelItem& cp)
        :Type(cp.Type), Addr(cp.Addr)
    {}

    ModelItem::~ModelItem()
    {}

    void ModelItem::operator=(const ModelItem& as)
    {
        Type = as.Type;
        Addr = as.Addr;
    }

    void ModelItem::Paint(QPainter* painter, const QStyleOptionViewItem& option, int row)
    {
        painter->save();

        int Left = option.rect.left();
        int Top = option.rect.top();
        int Width = option.rect.width();
        int Height = option.rect.height();
        int CellWidth = Width / 3;

        QBrush Brush[2] = { QBrush(QColor(233, 233, 233)), QBrush(QColor(222, 222, 222)) };
        QRect Rc[2] = {
            QRect(Left, Top, CellWidth - 2, Height - 2),
            QRect(Left + CellWidth, Top, CellWidth * 2 - 2, Height - 2)
        };
        QFont font("Microsoft YaHei");
        font.setWeight(QFont::Black);
        painter->setFont(font);
        if(row > 0)
        {
            switch (Type)
            {
            case LOCAL_IP:
                painter->fillRect(Rc[0], Brush[0]);
                painter->drawText(Rc[0], Qt::AlignCenter, QString("本地IP"));
                painter->fillRect(Rc[1], Brush[1]);
                painter->drawText(Rc[1], Addr);
                break;
            case LOCAL_PORT:
                painter->fillRect(Rc[0], Brush[0]);
                painter->drawText(Rc[0], Qt::AlignCenter, QString("本地端口"));
                painter->fillRect(Rc[1], Brush[1]);
                painter->drawText(Rc[1], Addr);
                break;
            case DEVICE_IP:
                painter->fillRect(Rc[0], Brush[0]);
                painter->drawText(Rc[0], Qt::AlignCenter, QString("设备IP"));
                painter->fillRect(Rc[1], Brush[1]);
                painter->drawText(Rc[1], Addr);
                break;
            case DEVICE_PORT:
                painter->fillRect(Rc[0], Brush[0]);
                painter->drawText(Rc[0], Qt::AlignCenter, QString("设备端口"));
                painter->fillRect(Rc[1], Brush[1]);
                painter->drawText(Rc[1], Addr);
                break;
            default:
                break;
            }
        }

        painter->restore();
    }

    Model::Model(QObject* parent)
        :QAbstractListModel(parent)
    {}

    bool Model::AddItem(ModelItem item)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        Items.push_back(item);
        endInsertRows();
        return true;
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

    QVariant Model::data(const QModelIndex& Index, int role) const
    {
        if(Index.row() > 0 && Index.row() < rowCount())
        {
            switch (role)
            {
            case UsrRole::ITEM:
                return QVariant::fromValue(Items[Index.row() - 1]);
            case UsrRole::FIRST_COL:
                return QVariant(Items[Index.row() - 1].Type);
            case UsrRole::SECOND_COL:
                return QVariant(Items[Index.row() - 1].Addr);
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
            case UsrRole::ITEM:
                Items[index.row() - 1] = value.value<ModelItem>();
                break;
            case UsrRole::FIRST_COL:
                Items[index.row() - 1].Type = static_cast<AddrType>(value.toInt());
                break;
            case UsrRole::SECOND_COL:
                Items[index.row() - 1].Addr = value.toString();
                break;
            default:
                break;
            }
        }
        return false;
    }

    Qt::ItemFlags Model::flags(const QModelIndex& Index) const
    {
        if(Index.row() > 0 && Index.row() < rowCount())
        {
            return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
        return Qt::ItemIsEnabled;
    }

    void Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            ModelItem item = index.data(Model::UsrRole::ITEM).value<ModelItem>();
            item.Paint(painter, option, index.row());
            QStyledItemDelegate::paint(painter, option, index);
        }
        else
        {
            painter->save();
            painter->fillRect(QRect(option.rect.left(), option.rect.top(), option.rect.width() - 2, option.rect.height() - 2), QColor(233, 233, 233));
            painter->drawText(option.rect, Qt::AlignCenter, "Socket配置");
            painter->restore();            
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

    QWidget* Delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            int Width = option.rect.width();
            int CellWidth = Width / 3;

            int cursr = option.widget->mapFromGlobal(QCursor::pos()).x();
            switch (cursr / CellWidth)
            {
            case 0:
                break;
            case 1:
            case 2:
            {
                QPlainTextEdit* editor = new QPlainTextEdit(parent);
                editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                return editor;
            }
                /* code */
                break;
            default:
                break;
            }
        }
        return nullptr;
    }

    void Delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            int Left = option.rect.left();
            int Top = option.rect.top();
            int Width = option.rect.width();
            int Height = option.rect.height();
            int CellWidth = Width / 3;

            int cursr = option.widget->mapFromGlobal(QCursor::pos()).x();
            switch (cursr / CellWidth)
            {
            case 0:
                break;
            case 1:
            case 2:
                editor->setGeometry(QRect(Left + CellWidth, Top, CellWidth * 2, Height));
                break;
            default:
                break;
            }
        }
    }

    void Delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            QPlainTextEdit* edit = qobject_cast<QPlainTextEdit*>(editor);
            if(edit)
            {
                edit->setPlainText(index.data(Model::UsrRole::SECOND_COL).toString());
            }
        }
    }

    void Delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
    {
        if(index.row() > 0)
        {
            QPlainTextEdit* edit = qobject_cast<QPlainTextEdit*>(editor);
            if(edit)
            {
                model->setData(index, edit->toPlainText(), Model::UsrRole::SECOND_COL);
            }
        }
    }

    //回车事件过滤器

    bool ReturnEventFilter::eventFilter(QObject* obj, QEvent* event)
    {
        switch (event->type())
        {
        case QEvent::KeyPress:
        {
            QKeyEvent* KeyEvent = static_cast<QKeyEvent*>(event);
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

        return QObject::eventFilter(obj, event);
    }

    //ListView

    ListView::ListView(QWidget* parent)
        :QListView(parent)
    {
        HoveredIndex = QModelIndex();

        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setEditTriggers(QAbstractItemView::DoubleClicked);

        viewport()->installEventFilter(this);
        QString RangeIP = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";//用于ipv4地址的正则表达式
        QRegularExpression RegExpIP("^" + RangeIP + "\\." + RangeIP + "\\." + RangeIP + "\\." + RangeIP + "$");
        ValidatorIP = new QRegularExpressionValidator(RegExpIP, this);
        Filter = new ReturnEventFilter(this);
    }

    ListView::~ListView()
    {
        if(TextEdit)
        {
            TextEdit->deleteLater();
            TextEdit = nullptr;
        }
    }

    bool ListView::eventFilter(QObject* obj, QEvent* event)
    {
        if(obj == viewport())//只处理视口区域的事件
        {
            switch(event->type())
            {
            case QEvent::MouseButtonPress:
            {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                QModelIndex index = indexAt(mouseEvent->pos());
                if(index != HoveredIndex)
                {
                    HoveredIndex = index;
                    update(index);
                    if(LineEditIP != nullptr)
                    {
                        LineEditIP->deleteLater();
                        LineEditIP = nullptr;
                    }
                    if(TextEdit != nullptr)
                    {
                        TextEdit->deleteLater();
                        TextEdit = nullptr;
                    }
                    switch (index.row())
                    {
                    case 0:
                        clearSelection();
                        break;
                    case 1://local ip
                        {
                            if(index.isValid())
                            {
                                QRect Rc = visualRect(index);
                                LineEditIP = new QLineEdit(this);
                                LineEditIP->setValidator(ValidatorIP);
                                LineEditIP->setFocus();
                                LineEditIP->setGeometry(Rc.left() + Rc.width() / 3, Rc.top(), Rc.width() * 2 / 3, Rc.height());
                                LineEditIP->setText(index.data(Model::UsrRole::SECOND_COL).value<QString>());
                                connect(LineEditIP, &QLineEdit::textChanged, this, [this](){
                                    emit ItemChanged(HoveredIndex.row(), LineEditIP->text());
                                });
                                LineEditIP->show();
                            }
                        }
                        break;
                    case 2://local port
                        {
                            if(index.isValid())
                            {
                                QRect Rc = visualRect(index);
                                TextEdit = new QPlainTextEdit(this);
                                TextEdit->installEventFilter(Filter);
                                TextEdit->setFocus();
                                TextEdit->setGeometry(Rc.left() + Rc.width() / 3, Rc.top(), Rc.width() * 2 / 3, Rc.height());
                                TextEdit->setPlainText(index.data(Model::UsrRole::SECOND_COL).value<QString>());
                                connect(TextEdit, &QPlainTextEdit::textChanged, this, [this](){
                                    emit ItemChanged(HoveredIndex.row(), TextEdit->toPlainText());
                                });
                                TextEdit->show();
                            }
                        }
                        break;
                    case 3://chip ip
                        {
                            if(index.isValid())
                            {
                                QRect Rc = visualRect(index);
                                LineEditIP = new QLineEdit(this);
                                LineEditIP->setValidator(ValidatorIP);
                                LineEditIP->setFocus();
                                LineEditIP->setGeometry(Rc.left() + Rc.width() / 3, Rc.top(), Rc.width() * 2 / 3, Rc.height());
                                LineEditIP->setText(index.data(Model::UsrRole::SECOND_COL).value<QString>());
                                connect(LineEditIP, &QLineEdit::textChanged, this, [this](){
                                    emit ItemChanged(HoveredIndex.row(), LineEditIP->text());
                                });
                                LineEditIP->show();
                            }
                        }
                        break;
                    case 4://chip port
                        {
                            if(index.isValid())
                            {
                                QRect Rc = visualRect(index);
                                TextEdit = new QPlainTextEdit(this);
                                TextEdit->installEventFilter(Filter);
                                TextEdit->setFocus();
                                TextEdit->setGeometry(Rc.left() + Rc.width() / 3, Rc.top(), Rc.width() * 2 / 3, Rc.height());
                                TextEdit->setPlainText(index.data(Model::UsrRole::SECOND_COL).value<QString>());
                                connect(TextEdit, &QPlainTextEdit::textChanged, this, [this](){
                                    emit ItemChanged(HoveredIndex.row(), TextEdit->toPlainText());
                                });
                                TextEdit->show();
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
                break;            
            default:
                break;
            }
        }
        return QListView::eventFilter(obj, event);
    }

    //Dialog

    CfgInfo::CfgInfo()
        :LocalIP(""), LocalPort(""), DeviceIP(""), DevicePort(""), Option(SockOPT::DISCONNECT)
    {}

    CfgInfo::CfgInfo(QString localIP, QString localPort, QString deviceIP, QString devicePort, SockOPT option)
        :LocalIP(localIP), LocalPort(localPort), DeviceIP(deviceIP), DevicePort(devicePort), Option(option)
    {}

    CfgInfo::CfgInfo(const CfgInfo& cp)
        :LocalIP(cp.LocalIP), LocalPort(cp.LocalPort), DeviceIP(cp.DeviceIP), DevicePort(cp.DevicePort), Option(cp.Option)
    {}

    void CfgInfo::operator=(const CfgInfo& as)
    {
        LocalIP = as.LocalIP;
        LocalPort = as.LocalPort;
        DeviceIP = as.DeviceIP;
        DevicePort = as.DevicePort;
        Option = as.Option;
    }

    bool CfgInfo::operator==(const CfgInfo& cmp)
    {
        return LocalIP == cmp.LocalIP && LocalPort == cmp.LocalPort && DeviceIP == cmp.DeviceIP && DevicePort == cmp.DevicePort;
    }

    void CfgInfo::SetOption(SockOPT opt)
    {
        Option = opt;
    }

    DialogSockCFG::DialogSockCFG(QWidget* parent)
        :QDialog(parent)
    {
        setWindowTitle("Socket配置");
        resize(420, 400);
        InitUI();
        Connect();
    }

    DialogSockCFG::~DialogSockCFG()
    {
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

        if(BtnConnect)
        {
            BtnConnect->deleteLater();
            BtnConnect = nullptr;
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

        if(VlayoutAll)
        {
            VlayoutAll->deleteLater();//deleteLater()会在事件循环中删除对象
            VlayoutAll = nullptr;
        }
    }

    void DialogSockCFG::InitUI()
    {
        model = new Model;
        // model->AddItem(ModelItem(AddrType::LOCAL_IP, "192.167.2.111"));
        // model->AddItem(ModelItem(AddrType::LOCAL_PORT, "8254"));
        // model->AddItem(ModelItem(AddrType::DEVICE_IP, "192.167.2.67"));
        // model->AddItem(ModelItem(AddrType::DEVICE_PORT, "8204"));
        // model->AddItem(ModelItem(AddrType::LOCAL_IP, CFGI::IniFileCFGGlobal->   ReadINI(CFGI::INI_NET_CFG, "NETADDR/LocalIP").toString()));
        // model->AddItem(ModelItem(AddrType::LOCAL_PORT, CFGI::IniFileCFGGlobal-> ReadINI(CFGI::INI_NET_CFG, "NETADDR/LocalPort").toString()));
        // model->AddItem(ModelItem(AddrType::DEVICE_IP, CFGI::IniFileCFGGlobal->  ReadINI(CFGI::INI_NET_CFG, "NETADDR/DeviceIP").toString()));
        // model->AddItem(ModelItem(AddrType::DEVICE_PORT, CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_NET_CFG, "NETADDR/DevicePort").toString()));
        
        model->AddItem(ModelItem(AddrType::LOCAL_IP, CFGI::IniFileCFGGlobal->   ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalIP").toString()));
        model->AddItem(ModelItem(AddrType::LOCAL_PORT, CFGI::IniFileCFGGlobal-> ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalPort").toString()));
        model->AddItem(ModelItem(AddrType::DEVICE_IP, CFGI::IniFileCFGGlobal->  ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/DeviceIP").toString()));
        model->AddItem(ModelItem(AddrType::DEVICE_PORT, CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/DevicePort").toString()));

        delegate = new Delegate;
        view = new ListView;
        view->setModel(model);
        view->setItemDelegate(delegate);
        view->setMaximumSize(400, 180);
        BtnConnect = new QPushButton;
        BtnConnect->setText("连接");
        VlayoutView = new QVBoxLayout;
        HLayoutView = new QHBoxLayout;
        HlayoutConnect = new QHBoxLayout;
        GboxView = new QGroupBox;
        GboxView->setTitle("Socket配置");
        GboxView->setMaximumSize(410, 300);
        BtnOK = new QPushButton;
        BtnOK->setText("确定");
        BtnOK->setMaximumSize(60, 24);
        BtnCancel = new QPushButton;
        BtnCancel->setText("取消");
        BtnCancel->setMaximumSize(60, 24);
        HlayoutOK = new QHBoxLayout;
        HlayoutFst = new QHBoxLayout;
        VlayoutAll = new QVBoxLayout;

        HLayoutView->addWidget(view);
        HlayoutConnect->addStretch();
        HlayoutConnect->addWidget(BtnConnect);
        HlayoutConnect->addSpacing(10);
        VlayoutView->addLayout(HLayoutView);
        VlayoutView->addLayout(HlayoutConnect);
        GboxView->setLayout(VlayoutView);


        HlayoutOK->addStretch();
        HlayoutOK->addWidget(BtnOK);
        HlayoutOK->addWidget(BtnCancel);

        HlayoutFst->addWidget(GboxView);

        VlayoutAll->addLayout(HlayoutFst);
        VlayoutAll->addStretch();
        VlayoutAll->addLayout(HlayoutOK);
        setLayout(VlayoutAll);

        Option = SockOPT::DISCONNECT;
        // Record = CfgInfo("192.167.2.111", "8254", "192.167.2.67", "8204", SockOPT::DISCONNECT);
        Record = CfgInfo(model->Items[0].Addr, model->Items[1].Addr, model->Items[2].Addr, model->Items[3].Addr, SockOPT::DISCONNECT);
    }

    void DialogSockCFG::Connect()
    {
        connect(BtnConnect, &QPushButton::clicked, this, [this](){
            if(Option == SockOPT::CONNECT)
            {
                Option = SockOPT::DISCONNECT;
                BtnConnect->setText("连接");
            }
            else
            {
                Option = SockOPT::CONNECT;
                BtnConnect->setText("断开");
            }
        });

        // connect(BtnOK, &QPushButton::clicked, this, &QDialog::accept);

        connect(BtnOK, &QPushButton::clicked, this, [this](){ this->OKSlot(); });

        connect(BtnCancel, &QPushButton::clicked, this, &QDialog::reject);

        // connect(this, &QDialog::accepted, this, [this](){
        //     this->OKSlot();
        // });

        connect(this, &QDialog::rejected, this, [this](){
            this->CancelSlot();
        });

        connect(view, &ListView::ItemChanged, this, [this](int row, QString text){
            this->ListItemChanged(row, text);
        });
    }

    void DialogSockCFG::OKSlot()
    {
        CfgInfo info(model->Items[0].Addr, model->Items[1].Addr, model->Items[2].Addr, model->Items[3].Addr, Option);
        // CfgInfo info("192.167.2.111", "8254", "192.167.2.67", "8204", Option);
        // CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_NET_CFG, "NETADDR/LocalIP", info.LocalIP);
        // CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_NET_CFG, "NETADDR/LocalPort", info.LocalPort);
        // CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_NET_CFG, "NETADDR/DeviceIP", info.DeviceIP);
        // CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_NET_CFG, "NETADDR/DevicePort", info.DevicePort);

        CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalIP", info.LocalIP);
        CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalPort", info.LocalPort);
        CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/DeviceIP", info.DeviceIP);
        CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/DevicePort", info.DevicePort);

        switch (Option)
        {
        case SockOPT::CONNECT:
            switch (Record.Option)
            {
            case SockOPT::CONNECT:
                if(!(Record == info))
                {
                    SOCKWZ::SockGlob::DisConnect();
                    if(SOCKWZ::SockGlob::Connect(info.LocalIP, info.LocalPort, info.DeviceIP, info.DevicePort))
                    {
                        Record = info;
                        BtnConnect->setText("断开");
                        emit SockConfigComplete();//to MNIC
                        emit ConnectStatusChanged(true);//to MNIC
                        accept();
                    }
                    else
                    {
                        Record.SetOption(SockOPT::DISCONNECT);
                        // Option = SockOPT::DISCONNECT;
                        // BtnConnect->setText("连接");
                        emit ConnectStatusChanged(false);//to MNIC
                        QMessageBox::critical(this, "网口状态", "创建socket失败", QMessageBox::Ok);
                    }
                }
                else
                {
                    accept();
                }
                break;
            case SockOPT::DISCONNECT:
                if(SOCKWZ::SockGlob::Connect(info.LocalIP, info.LocalPort, info.DeviceIP, info.DevicePort))
                {
                    Record = info;
                    BtnConnect->setText("断开");
                    emit SockConfigComplete();
                    emit ConnectStatusChanged(true);
                    accept();
                }
                else
                {
                    Record.SetOption(SockOPT::DISCONNECT);
                    // BtnConnect->setText("连接");
                    emit ConnectStatusChanged(false);
                    QMessageBox::critical(this, "网口状态", "创建socket失败", QMessageBox::Ok);
                }
                break;
            default:
                break;
            }
            break;
        case SockOPT::DISCONNECT:
            switch (Record.Option)
            {   
            case SockOPT::CONNECT:
                if(Record == info)
                {
                    SOCKWZ::SockGlob::DisConnect();
                    Record = info;
                    BtnConnect->setText("连接");
                    emit ConnectStatusChanged(false);
                }
                else
                {
                    SOCKWZ::SockGlob::DisConnect();
                    Record.SetOption(SockOPT::DISCONNECT);
                    BtnConnect->setText("连接");
                    emit ConnectStatusChanged(false);
                }
                accept();
                break;
            case SockOPT::DISCONNECT:
                BtnConnect->setText("连接");
                accept();
                break;
            default:
                break;                
            }
            break;
        default:
            break;
        }
    }

    void DialogSockCFG::CancelSlot()
    {
        model->UpdateItem(1, ModelItem(AddrType::LOCAL_IP, Record.LocalIP));
        model->UpdateItem(2, ModelItem(AddrType::LOCAL_PORT, Record.LocalPort));
        model->UpdateItem(3, ModelItem(AddrType::DEVICE_IP, Record.DeviceIP));
        model->UpdateItem(4, ModelItem(AddrType::DEVICE_PORT, Record.DevicePort));
        if(Option != SockOPT::DISCONNECT)
        {
            BtnConnect->setText("连接");
        }
        else
        {
            BtnConnect->setText("断开");
            
        }
    }

    void DialogSockCFG::ListItemChanged(int row, QString Text)
    {
        switch(row)
        {
            case 0:
            break;
            case 1: model->UpdateItem(row, ModelItem(AddrType::LOCAL_IP, Text));    break;
            case 2: model->UpdateItem(row, ModelItem(AddrType::LOCAL_PORT, Text));  break;
            case 3: model->UpdateItem(row, ModelItem(AddrType::DEVICE_IP, Text));   break;
            case 4: model->UpdateItem(row, ModelItem(AddrType::DEVICE_PORT, Text)); break;
            default: break;
        }
    }

    void DialogSockCFG::NetLoss()
    {

    }

}; // namespace MenuSocketCFG

namespace MenuSINADCFG{

    //Tab filter

    bool TabFocusEventFilter::eventFilter(QObject* obj, QEvent* event)
    {
        // WRITE_DLG_SINADCFG_MENU_DBG("TabFocusEventFilter::eventFilter() Enter\n");
        switch(event->type())
        {
        case QEvent::KeyPress:
        {
            QKeyEvent* KeyEvent = static_cast<QKeyEvent*>(event);
            switch(KeyEvent->key())
            {
            case Qt::Key_Tab:
            case Qt::Key_Return:
            {   
                WRITE_DLG_SINADCFG_MENU_DBG("Key Tab pressed\n");
                QWidget* Wt = static_cast<QWidget*>(obj);
                if(Wt)
                {
                    QWidget* NextWt = FindNextFocusableWidget<QSpinBox>(Wt);//手动找到下一个可聚焦的SpinBox控件,
                    if(NextWt)                                              
                    {
                        NextWt->setFocus();
                        WRITE_DLG_SINADCFG_MENU_DBG("After NextWt->setFocus()\n");
                    }
                }
                return true;//拦截事件, 阻止插入制表符
            }
                break;
            default:
                break;
            }
        }
            break;
        case QEvent::ToolTip:
        {
            QHelpEvent* HelpEvent = static_cast<QHelpEvent*>(event);
            QSpinBox* SpinBox = qobject_cast<QSpinBox*>(obj);
            if(SpinBox)
            {
                switch (SpinBox->property(SPINBOX_PROPERTY_KEY).toInt())
                {
                case CODEC_SINAD_L: QToolTip::showText(HelpEvent->globalPos(), TIP_CODEC_SINAD, SpinBox); break;
                case CODEC_SINAD_R: QToolTip::showText(HelpEvent->globalPos(), TIP_CODEC_SINAD, SpinBox); break;
                case CODEC_VPP_L:   QToolTip::showText(HelpEvent->globalPos(), TIP_CODEC_VPP,   SpinBox); break;
                case CODEC_VPP_R:   QToolTip::showText(HelpEvent->globalPos(), TIP_CODEC_VPP,   SpinBox); break;
                case CODEC_RMS_L:   QToolTip::showText(HelpEvent->globalPos(), TIP_CODEC_RMS,   SpinBox); break;
                case CODEC_RMS_R:   QToolTip::showText(HelpEvent->globalPos(), TIP_CODEC_RMS,   SpinBox); break;
                case ADPOW_SINAD_L: QToolTip::showText(HelpEvent->globalPos(), TIP_ADPOW_SINAD, SpinBox); break;
                case ADPOW_SINAD_R: QToolTip::showText(HelpEvent->globalPos(), TIP_ADPOW_SINAD, SpinBox); break;
                case ADPOW_VPP_L:   QToolTip::showText(HelpEvent->globalPos(), TIP_ADPOW_VPP,   SpinBox); break;
                case ADPOW_VPP_R:   QToolTip::showText(HelpEvent->globalPos(), TIP_ADPOW_VPP,   SpinBox); break;
                case ADPOW_RMS_L:   QToolTip::showText(HelpEvent->globalPos(), TIP_ADPOW_RMS,   SpinBox); break;
                case ADPOW_RMS_R:   QToolTip::showText(HelpEvent->globalPos(), TIP_ADPOW_RMS,   SpinBox); break;
                    break;
                default:
                    break;
                }
                return true;
            }
        }
        default:
            break;
        }
        // WRITE_DLG_SINADCFG_MENU_DBG("TabFocusEventFilter::eventFilter() End\n");
        return QObject::eventFilter(obj, event);
    }

    template<typename QWidgetDerive>
    QWidget* TabFocusEventFilter::FindNextFocusableWidget(QWidget* current)
    {
        WRITE_DLG_SINADCFG_MENU_DBG("TabFocusEventFilter::FindNextFocusableWidget()\n");
        QWidget* Next = current->nextInFocusChain();
        for(; Next && Next != current; Next = Next->nextInFocusChain())
        {
            WRITE_DLG_SINADCFG_MENU_DBG("Finding Next\n");
            if(qobject_cast<QWidgetDerive*>(Next) && Next->isEnabled() && Next->focusPolicy() != Qt::NoFocus)
            {//找到下一个可聚焦的控件
                WRITE_DLG_SINADCFG_MENU_DBG("Find Next\n");
                return Next;
            }
        }
        return nullptr;//未找到
    }

    //Dlg

    DialogSinadCFG::DialogSinadCFG(QWidget* parent)
        :QDialog(parent)
    {
        OPEN_DLG_SINADCFG_MENU_DBG(LogSianadCFGMenu.txt);
        Cond = std::make_unique<TCOND::TestCondition[]>(2);
        setWindowTitle("SINAD配置");
        resize(800, 600);
        InitUI();
        SetFocus();
        Connect();
        //初始化配置数据
        CFGSinadCodecSlot();
        CFGVppPTPCodecSlot();
        CFGVppRMSCodecSlot();
        CFGSinadADPOWSlot();
        CFGVppPTPADPOWSlot();
        CFGVppRMSADPOWSlot();
        OKSlot();
    }

    DialogSinadCFG::~DialogSinadCFG()
    {
        if(LabelSinadCodec)
        {
            LabelSinadCodec->deleteLater();
            LabelSinadCodec = nullptr;
        }
        if(SpinSinadCodecLeft)
        {
            SpinSinadCodecLeft->deleteLater();
            SpinSinadCodecLeft = nullptr;
        }
        if(SpinSinadCodecRight)
        {
            SpinSinadCodecRight->deleteLater();
            SpinSinadCodecRight = nullptr;
        }
        if(BtnCFGSinadCodec)
        {
            BtnCFGSinadCodec->deleteLater();
            BtnCFGSinadCodec = nullptr;
        }
        if(LabelVppPTPCodec)
        {
            LabelVppPTPCodec->deleteLater();
            LabelVppPTPCodec = nullptr;
        }
        if(SpinVppPTPCodecLeft)
        {
            SpinVppPTPCodecLeft->deleteLater();
            SpinVppPTPCodecLeft = nullptr;
        }
        if(SpinVppPTPCodecRight)
        {
            SpinVppPTPCodecRight->deleteLater();
            SpinVppPTPCodecRight = nullptr;
        }
        if(BtnCFGVppPTPCodec)
        {
            BtnCFGVppPTPCodec->deleteLater();
            BtnCFGVppPTPCodec = nullptr;
        }
        if(LabelVppRMSCodec)
        {
            LabelVppRMSCodec->deleteLater();
            LabelVppRMSCodec = nullptr;
        }
        if(SpinVppRMSCodecLeft)
        {
            SpinVppRMSCodecLeft->deleteLater();
            SpinVppRMSCodecLeft = nullptr;
        }
        if(SpinVppRMSCodecRight)
        {
            SpinVppRMSCodecRight->deleteLater();
            SpinVppRMSCodecRight = nullptr;
        }
        if(BtnCFGVppRMSCodec)
        {
            BtnCFGVppRMSCodec->deleteLater();
            BtnCFGVppRMSCodec = nullptr;
        }
        if(LabelSinadADPOW)
        {
            LabelSinadADPOW->deleteLater();
            LabelSinadADPOW = nullptr;
        }
        if(SpinSinadADPOWLeft)
        {
            SpinSinadADPOWLeft->deleteLater();
            SpinSinadADPOWLeft = nullptr;
        }
        if(SpinSinadADPOWRight)
        {
            SpinSinadADPOWRight->deleteLater();
            SpinSinadADPOWRight = nullptr;
        }
        if(BtnCFGSinadADPOW)
        {
            BtnCFGSinadADPOW->deleteLater();
            BtnCFGSinadADPOW = nullptr;
        }
        if(LabelVppPTPADPOW)
        {
            LabelVppPTPADPOW->deleteLater();
            LabelVppPTPADPOW = nullptr;
        }
        if(SpinVppPTPADPOWLeft)
        {
            SpinVppPTPADPOWLeft->deleteLater();
            SpinVppPTPADPOWLeft = nullptr;
        }
        if(SpinVppPTPADPOWRight)
        {
            SpinVppPTPADPOWRight->deleteLater();
            SpinVppPTPADPOWRight = nullptr;
        }
        if(BtnCFGVppPTPADPOW)
        {
            BtnCFGVppPTPADPOW->deleteLater();
            BtnCFGVppPTPADPOW = nullptr;
        }
        if(LabelVppRMSADPOW)
        {
            LabelVppRMSADPOW->deleteLater();
            LabelVppRMSADPOW = nullptr;
        }
        if(SpinVppRMSADPOWLeft)
        {
            SpinVppRMSADPOWLeft->deleteLater();
            SpinVppRMSADPOWLeft = nullptr;
        }
        if(SpinVppRMSADPOWRight)
        {
            SpinVppRMSADPOWRight->deleteLater();
            SpinVppRMSADPOWRight = nullptr;
        }
        if(BtnCFGVppRMSADPOW)
        {
            BtnCFGVppRMSADPOW->deleteLater();
            BtnCFGVppRMSADPOW = nullptr;
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
        if(HlayoutSinadCodec)
        {
            HlayoutSinadCodec->deleteLater();
            HlayoutSinadCodec = nullptr;
        }
        if(HlayoutVppPTPCodec)
        {
            HlayoutVppPTPCodec->deleteLater();
            HlayoutVppPTPCodec = nullptr;
        }
        if(HlayoutVppRMSCodec)
        {
            HlayoutVppRMSCodec->deleteLater();
            HlayoutVppRMSCodec = nullptr;
        }
        if(VlayoutCodec)
        {
            VlayoutCodec->deleteLater();
            VlayoutCodec = nullptr;
        }
        if(GboxCodec)
        {
            GboxCodec->deleteLater();
            GboxCodec = nullptr;
        }
        if(HlayoutSinadADPOW)
        {
            HlayoutSinadADPOW->deleteLater();
            HlayoutSinadADPOW = nullptr;
        }
        if(HlayoutVppPTPADPOW)
        {
            HlayoutVppPTPADPOW->deleteLater();
            HlayoutVppPTPADPOW = nullptr;
        }
        if(HlayoutVppRMSADPOW)
        {
            HlayoutVppRMSADPOW->deleteLater();
            HlayoutVppRMSADPOW = nullptr;
        }
        if(VlayoutADPOW)
        {
            VlayoutADPOW->deleteLater();
            VlayoutADPOW = nullptr;
        }
        if(GboxADPOW)
        {
            GboxADPOW->deleteLater();
            GboxADPOW = nullptr;
        }
        if(HlayoutOK)
        {
            HlayoutOK->deleteLater();
            HlayoutOK = nullptr;
        }
        if(VlayoutAll)
        {
            VlayoutAll->deleteLater();
            VlayoutAll = nullptr;
        }
        if(Cond != nullptr)
        {
            Cond.reset();
            Cond = nullptr;
        }

        Cond.reset();

        CLOSE_DLG_SINADCFG_MENU_DBG();
    }

    void DialogSinadCFG::InitUI()
    {
        LabelSinadCodec = new QLabel;
        LabelSinadCodec->setText("SINAD阈值/dB");
        LabelSinadCodec->setFixedSize(100, 24);
        SpinSinadCodecLeft = new QSpinBox(this);
        SpinSinadCodecLeft->setFixedSize(100, 24);
        SpinSinadCodecLeft->setRange(-128, 128);
        SpinSinadCodecLeft->setSingleStep(1);
        SpinSinadCodecLeft->setSuffix("dB");
        SpinSinadCodecLeft->setValue(60);
        SpinSinadCodecRight = new QSpinBox(this);
        SpinSinadCodecRight->setFixedSize(100, 24);
        SpinSinadCodecRight->setRange(-128, 128);
        SpinSinadCodecRight->setSingleStep(1);
        SpinSinadCodecRight->setSuffix("dB");
        SpinSinadCodecRight->setValue(128);
        BtnCFGSinadCodec = new QPushButton;
        BtnCFGSinadCodec->setText("配置");
        LabelVppPTPCodec = new QLabel;
        LabelVppPTPCodec->setText("Vpp阈值");
        LabelVppPTPCodec->setFixedSize(100, 24);
        SpinVppPTPCodecLeft = new QSpinBox(this);
        SpinVppPTPCodecLeft->setFixedSize(100, 24);
        SpinVppPTPCodecLeft->setRange(0, 65535);
        SpinVppPTPCodecLeft->setSingleStep(1);
        SpinVppPTPCodecLeft->setValue(0);
        SpinVppPTPCodecRight = new QSpinBox(this);
        SpinVppPTPCodecRight->setFixedSize(100, 24);
        SpinVppPTPCodecRight->setRange(0, 65535);
        SpinVppPTPCodecRight->setSingleStep(1);
        SpinVppPTPCodecRight->setValue(65535);
        BtnCFGVppPTPCodec = new QPushButton;
        BtnCFGVppPTPCodec->setText("配置");
        LabelVppRMSCodec = new QLabel;
        LabelVppRMSCodec->setText("RMS阈值");
        LabelVppRMSCodec->setFixedSize(100, 24);
        SpinVppRMSCodecLeft = new QSpinBox(this);
        SpinVppRMSCodecLeft->setFixedSize(100, 24);
        SpinVppRMSCodecLeft->setRange(0, 65535);
        SpinVppRMSCodecLeft->setSingleStep(1);
        SpinVppRMSCodecLeft->setValue(0);
        SpinVppRMSCodecRight = new QSpinBox(this);
        SpinVppRMSCodecRight->setFixedSize(100, 24);
        SpinVppRMSCodecRight->setRange(0, 65535);
        SpinVppRMSCodecRight->setSingleStep(1);
        SpinVppRMSCodecRight->setValue(65535);
        BtnCFGVppRMSCodec = new QPushButton;
        BtnCFGVppRMSCodec->setText("配置");
        LabelSinadADPOW = new QLabel;
        LabelSinadADPOW->setText("SINAD阈值");
        LabelSinadADPOW->setFixedSize(100, 24);
        SpinSinadADPOWLeft = new QSpinBox(this);
        SpinSinadADPOWLeft->setFixedSize(100, 24);
        SpinSinadADPOWLeft->setRange(-128, 128);
        SpinSinadADPOWLeft->setSingleStep(1);
        SpinSinadADPOWLeft->setSuffix("dB");
        SpinSinadADPOWLeft->setValue(60);
        SpinSinadADPOWRight = new QSpinBox(this);
        SpinSinadADPOWRight->setFixedSize(100, 24);
        SpinSinadADPOWRight->setRange(-128, 128);
        SpinSinadADPOWRight->setSingleStep(1);
        SpinSinadADPOWRight->setSuffix("dB");
        SpinSinadADPOWRight->setValue(128);
        BtnCFGSinadADPOW = new QPushButton;
        BtnCFGSinadADPOW->setText("配置");
        LabelVppPTPADPOW = new QLabel;
        LabelVppPTPADPOW->setText("Vpp阈值");
        LabelVppPTPADPOW->setFixedSize(100, 24);
        SpinVppPTPADPOWLeft = new QSpinBox(this);
        SpinVppPTPADPOWLeft->setFixedSize(100, 24);
        SpinVppPTPADPOWLeft->setRange(0, 65535);
        SpinVppPTPADPOWLeft->setSingleStep(1);
        SpinVppPTPADPOWLeft->setValue(0);
        SpinVppPTPADPOWRight = new QSpinBox(this);
        SpinVppPTPADPOWRight->setFixedSize(100, 24);
        SpinVppPTPADPOWRight->setRange(0, 65535);
        SpinVppPTPADPOWRight->setSingleStep(1);
        SpinVppPTPADPOWRight->setValue(65535);
        BtnCFGVppPTPADPOW = new QPushButton;
        BtnCFGVppPTPADPOW->setText("配置");
        LabelVppRMSADPOW = new QLabel;
        LabelVppRMSADPOW->setText("Vpp阈值");
        LabelVppRMSADPOW->setFixedSize(100, 24);
        SpinVppRMSADPOWLeft = new QSpinBox(this);
        SpinVppRMSADPOWLeft->setFixedSize(100, 24);
        SpinVppRMSADPOWLeft->setRange(0, 65535);
        SpinVppRMSADPOWLeft->setSingleStep(1);
        SpinVppRMSADPOWLeft->setValue(0);
        SpinVppRMSADPOWRight = new QSpinBox(this);
        SpinVppRMSADPOWRight->setFixedSize(100, 24);
        SpinVppRMSADPOWRight->setRange(0, 65535);
        SpinVppRMSADPOWRight->setSingleStep(1);
        SpinVppRMSADPOWRight->setValue(65535);
        BtnCFGVppRMSADPOW = new QPushButton;
        BtnCFGVppRMSADPOW->setText("配置");
        BtnOK = new QPushButton;
        BtnOK->setText("确定");
        BtnCancel = new QPushButton;
        BtnCancel->setText("取消");

        // SetFocus();

        HlayoutSinadCodec = new QHBoxLayout;
        HlayoutVppPTPCodec = new QHBoxLayout;
        HlayoutVppRMSCodec = new QHBoxLayout;
        VlayoutCodec = new QVBoxLayout;
        GboxCodec = new QGroupBox;
        GboxCodec->setTitle("Codec配置");
        HlayoutSinadADPOW = new QHBoxLayout;
        HlayoutVppPTPADPOW = new QHBoxLayout;
        HlayoutVppRMSADPOW = new QHBoxLayout;
        VlayoutADPOW = new QVBoxLayout;
        GboxADPOW = new QGroupBox;
        GboxADPOW->setTitle("ADPOW配置");
        HlayoutOK = new QHBoxLayout;
        VlayoutAll = new QVBoxLayout;

        HlayoutSinadCodec->addWidget(LabelSinadCodec);
        HlayoutSinadCodec->addWidget(SpinSinadCodecLeft);
        HlayoutSinadCodec->addWidget(SpinSinadCodecRight);
        HlayoutSinadCodec->addWidget(BtnCFGSinadCodec);
        HlayoutSinadCodec->addStretch();

        HlayoutVppPTPCodec->addWidget(LabelVppPTPCodec);
        HlayoutVppPTPCodec->addWidget(SpinVppPTPCodecLeft);
        HlayoutVppPTPCodec->addWidget(SpinVppPTPCodecRight);
        HlayoutVppPTPCodec->addWidget(BtnCFGVppPTPCodec);
        HlayoutVppPTPCodec->addStretch();

        HlayoutVppRMSCodec->addWidget(LabelVppRMSCodec);
        HlayoutVppRMSCodec->addWidget(SpinVppRMSCodecLeft);
        HlayoutVppRMSCodec->addWidget(SpinVppRMSCodecRight);
        HlayoutVppRMSCodec->addWidget(BtnCFGVppRMSCodec);
        HlayoutVppRMSCodec->addStretch();

        VlayoutCodec->addLayout(HlayoutSinadCodec);
        VlayoutCodec->addLayout(HlayoutVppPTPCodec);
        VlayoutCodec->addLayout(HlayoutVppRMSCodec);
        GboxCodec->setLayout(VlayoutCodec);

        HlayoutSinadADPOW->addWidget(LabelSinadADPOW);
        HlayoutSinadADPOW->addWidget(SpinSinadADPOWLeft);
        HlayoutSinadADPOW->addWidget(SpinSinadADPOWRight);
        HlayoutSinadADPOW->addWidget(BtnCFGSinadADPOW);
        HlayoutSinadADPOW->addStretch();

        HlayoutVppPTPADPOW->addWidget(LabelVppPTPADPOW);
        HlayoutVppPTPADPOW->addWidget(SpinVppPTPADPOWLeft);
        HlayoutVppPTPADPOW->addWidget(SpinVppPTPADPOWRight);
        HlayoutVppPTPADPOW->addWidget(BtnCFGVppPTPADPOW);
        HlayoutVppPTPADPOW->addStretch();

        HlayoutVppRMSADPOW->addWidget(LabelVppRMSADPOW);
        HlayoutVppRMSADPOW->addWidget(SpinVppRMSADPOWLeft);
        HlayoutVppRMSADPOW->addWidget(SpinVppRMSADPOWRight);
        HlayoutVppRMSADPOW->addWidget(BtnCFGVppRMSADPOW);
        HlayoutVppRMSADPOW->addStretch();

        VlayoutADPOW->addLayout(HlayoutSinadADPOW);
        VlayoutADPOW->addLayout(HlayoutVppPTPADPOW);
        VlayoutADPOW->addLayout(HlayoutVppRMSADPOW);
        GboxADPOW->setLayout(VlayoutADPOW);

        HlayoutOK->addStretch();
        HlayoutOK->addWidget(BtnOK);
        HlayoutOK->addWidget(BtnCancel);

        VlayoutAll->addWidget(GboxCodec);
        VlayoutAll->addWidget(GboxADPOW);
        VlayoutAll->addStretch();
        VlayoutAll->addLayout(HlayoutOK);

        setLayout(VlayoutAll);
    }

    // SpinSinadCodecLeft;
    // SpinSinadCodecRight;
    // SpinVppPTPCodecLeft;
    // SpinVppPTPCodecRight;
    // SpinVppRMSCodecLeft;
    // SpinVppRMSCodecRight;
    // SpinSinadADPOWLeft;
    // SpinSinadADPOWRight;
    // SpinVppPTPADPOWLeft;
    // SpinVppPTPADPOWRight;
    // SpinVppRMSADPOWLeft;
    // SpinVppRMSADPOWRight;

    void DialogSinadCFG::SetFocus()
    {
        Filter = new TabFocusEventFilter(this);
        SpinSinadCodecLeft->setFocusPolicy(     Qt::FocusPolicy::StrongFocus);
        SpinSinadCodecRight->setFocusPolicy(    Qt::StrongFocus);
        SpinVppPTPCodecLeft->setFocusPolicy(    Qt::StrongFocus);
        SpinVppPTPCodecRight->setFocusPolicy(   Qt::StrongFocus);
        SpinVppRMSCodecLeft->setFocusPolicy(    Qt::StrongFocus);
        SpinVppRMSCodecRight->setFocusPolicy(   Qt::StrongFocus);
        SpinSinadADPOWLeft->setFocusPolicy(     Qt::StrongFocus);
        SpinSinadADPOWRight->setFocusPolicy(    Qt::StrongFocus);
        SpinVppPTPADPOWLeft->setFocusPolicy(    Qt::StrongFocus);
        SpinVppPTPADPOWRight->setFocusPolicy(   Qt::StrongFocus);
        SpinVppRMSADPOWLeft->setFocusPolicy(    Qt::StrongFocus);
        SpinVppRMSADPOWRight->setFocusPolicy(   Qt::StrongFocus);

        setTabOrder(SpinSinadCodecLeft, SpinSinadCodecRight);
        setTabOrder(SpinSinadCodecRight, SpinVppPTPCodecLeft);
        setTabOrder(SpinVppPTPCodecLeft, SpinVppPTPCodecRight);
        setTabOrder(SpinVppPTPCodecRight, SpinVppRMSCodecLeft);
        setTabOrder(SpinVppRMSCodecLeft, SpinVppRMSCodecRight);
        setTabOrder(SpinVppRMSCodecRight, SpinSinadADPOWLeft);
        setTabOrder(SpinSinadADPOWLeft, SpinSinadADPOWRight);
        setTabOrder(SpinSinadADPOWRight, SpinVppPTPADPOWLeft);
        setTabOrder(SpinVppPTPADPOWLeft, SpinVppPTPADPOWRight);
        setTabOrder(SpinVppPTPADPOWRight, SpinVppRMSADPOWLeft);
        setTabOrder(SpinVppRMSADPOWLeft, SpinVppRMSADPOWRight);
        setTabOrder(SpinVppRMSADPOWRight, 0);
        
        SpinSinadCodecLeft->    setProperty(SPINBOX_PROPERTY_KEY, CODEC_SINAD_L);
        SpinSinadCodecRight->   setProperty(SPINBOX_PROPERTY_KEY, CODEC_SINAD_R);
        SpinVppPTPCodecLeft->   setProperty(SPINBOX_PROPERTY_KEY, CODEC_VPP_L);
        SpinVppPTPCodecRight->  setProperty(SPINBOX_PROPERTY_KEY, CODEC_VPP_R);
        SpinVppRMSCodecLeft->   setProperty(SPINBOX_PROPERTY_KEY, CODEC_RMS_L);
        SpinVppRMSCodecRight->  setProperty(SPINBOX_PROPERTY_KEY, CODEC_RMS_R);
        SpinSinadADPOWLeft->    setProperty(SPINBOX_PROPERTY_KEY, ADPOW_SINAD_L);
        SpinSinadADPOWRight->   setProperty(SPINBOX_PROPERTY_KEY, ADPOW_SINAD_R);
        SpinVppPTPADPOWLeft->   setProperty(SPINBOX_PROPERTY_KEY, ADPOW_VPP_L);
        SpinVppPTPADPOWRight->  setProperty(SPINBOX_PROPERTY_KEY, ADPOW_VPP_R);
        SpinVppRMSADPOWLeft->   setProperty(SPINBOX_PROPERTY_KEY, ADPOW_RMS_L);
        SpinVppRMSADPOWRight->  setProperty(SPINBOX_PROPERTY_KEY, ADPOW_RMS_R);

        SpinSinadCodecLeft->installEventFilter(Filter);
        SpinSinadCodecRight->installEventFilter(Filter);
        SpinVppPTPCodecLeft->installEventFilter(Filter);
        SpinVppPTPCodecRight->installEventFilter(Filter);
        SpinVppRMSCodecLeft->installEventFilter(Filter);
        SpinVppRMSCodecRight->installEventFilter(Filter);
        SpinSinadADPOWLeft->installEventFilter(Filter);
        SpinSinadADPOWRight->installEventFilter(Filter);
        SpinVppPTPADPOWLeft->installEventFilter(Filter);
        SpinVppPTPADPOWRight->installEventFilter(Filter);
        SpinVppRMSADPOWLeft->installEventFilter(Filter);
        SpinVppRMSADPOWRight->installEventFilter(Filter);
    }

    void DialogSinadCFG::Connect()
    {
        connect(BtnCFGSinadCodec, &QPushButton::clicked, this, [this](){
            CFGSinadCodecSlot();
        });

        connect(BtnCFGVppPTPCodec, &QPushButton::clicked, this, [this](){
            CFGVppPTPCodecSlot();
        });

        connect(BtnCFGVppRMSCodec, &QPushButton::clicked, this, [this](){
            CFGVppRMSCodecSlot();
        });

        connect(BtnCFGSinadADPOW, &QPushButton::clicked, this, [this](){
            CFGSinadADPOWSlot();
        });

        connect(BtnCFGVppPTPADPOW, &QPushButton::clicked, this, [this](){
            CFGVppPTPADPOWSlot();
        });

        connect(BtnCFGVppRMSADPOW, &QPushButton::clicked, this, [this](){
            CFGVppRMSADPOWSlot();
        });

        connect(BtnOK, &QPushButton::clicked, this, &QDialog::accept);

        connect(BtnCancel, &QPushButton::clicked, this, &QDialog::reject);

        connect(this, &QDialog::accepted, this, [this](){
            this->OKSlot();
        });

        connect(this, &QDialog::rejected, this, [this](){
            this->CancelSlot();
        });
    }

    void DialogSinadCFG::CFGSinadCodecSlot()
    {
        Cond.get()[0].SetRangeSINAD(TCOND::Range(SpinSinadCodecLeft->value(), SpinSinadCodecRight->value()));        
    }

    void DialogSinadCFG::CFGVppPTPCodecSlot()
    {
        Cond.get()[0].SetRangeVppPTP(TCOND::Range(SpinVppPTPCodecLeft->value(), SpinVppPTPCodecRight->value()));
    }

    void DialogSinadCFG::CFGVppRMSCodecSlot()
    {
        Cond.get()[0].SetRangeVppRMS(TCOND::Range(SpinVppRMSCodecLeft->value(), SpinVppRMSCodecRight->value()));
    }

    void DialogSinadCFG::CFGSinadADPOWSlot()
    {
        Cond.get()[1].SetRangeSINAD(TCOND::Range(SpinSinadADPOWLeft->value(), SpinSinadADPOWRight->value()));        
    }

    void DialogSinadCFG::CFGVppPTPADPOWSlot()
    {
        Cond.get()[1].SetRangeVppPTP(TCOND::Range(SpinVppPTPADPOWLeft->value(), SpinVppPTPADPOWRight->value()));
    }

    void DialogSinadCFG::CFGVppRMSADPOWSlot()
    {
        Cond.get()[1].SetRangeVppRMS(TCOND::Range(SpinVppRMSADPOWLeft->value(), SpinVppRMSADPOWRight->value()));
    }

    void DialogSinadCFG::OKSlot()
    {
        CFGSinadCodecSlot();
        CFGVppPTPCodecSlot();
        CFGVppRMSCodecSlot();
        CFGSinadADPOWSlot();
        CFGVppPTPADPOWSlot();
        CFGVppRMSADPOWSlot();        
        WRITE_DLG_SINADCFG_MENU_DBG("Cond Codec Sinad Left = %u, Right = %u\n\\
            Cond Codec Vpp Left = %u, Right = %u\n\\
            Cond Codec Rms Left = %u, Right = %u\n\\
            Cond Adpow Sinad Left = %u, Right = %u\n\\
            Cond AdPow Vpp Left = %u, Right = %u\n\\
            Cond AdPow Rms Left = %u, Right = %u\n", 
            Cond[0].GetRangeSINAD().GetLeft(),  Cond[0].GetRangeSINAD().GetRight(),
            Cond[0].GetRangeVppPTP().GetLeft(), Cond[0].GetRangeVppPTP().GetRight(),
            Cond[0].GetRangeVppRMS().GetLeft(), Cond[0].GetRangeVppRMS().GetRight(),
            Cond[1].GetRangeSINAD().GetLeft(),  Cond[1].GetRangeSINAD().GetRight(),
            Cond[1].GetRangeVppPTP().GetLeft(), Cond[1].GetRangeVppPTP().GetRight(),
            Cond[1].GetRangeVppRMS().GetLeft(), Cond[1].GetRangeVppRMS().GetRight());
        DCR::DeviceCheckResultGlobal->GetCondition()[0] = Cond[0];
        DCR::DeviceCheckResultGlobal->GetCondition()[1] = Cond[1];
    }

    void DialogSinadCFG::CancelSlot()
    {
        SpinSinadCodecLeft      ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[0].GetRangeSINAD().GetLeft());
        SpinSinadCodecRight     ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[0].GetRangeSINAD().GetRight());
        SpinVppPTPCodecLeft     ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[0].GetRangeVppPTP().GetLeft());
        SpinVppPTPCodecRight    ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[0].GetRangeVppPTP().GetRight());
        SpinVppRMSCodecLeft     ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[0].GetRangeVppRMS().GetLeft());
        SpinVppRMSCodecRight    ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[0].GetRangeVppRMS().GetRight());
        SpinSinadADPOWLeft      ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[1].GetRangeSINAD().GetLeft());
        SpinSinadADPOWRight     ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[1].GetRangeSINAD().GetRight());
        SpinVppPTPADPOWLeft     ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[1].GetRangeVppPTP().GetLeft());
        SpinVppPTPADPOWRight    ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[1].GetRangeVppPTP().GetRight());
        SpinVppRMSADPOWLeft     ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[1].GetRangeVppRMS().GetLeft());
        SpinVppRMSADPOWRight    ->setValue(DCR::DeviceCheckResultGlobal->GetCondition()[1].GetRangeVppRMS().GetRight());
    }

}; // namespace MenuSINADCFG