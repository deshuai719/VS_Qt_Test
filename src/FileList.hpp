#ifndef _FILELIST_HPP_
#define _FILELIST_HPP_

#include "DataConstruct.hpp"

namespace FLST{
    enum FileOPT{
        NONE,
        ADD_FILE,
        DEL_FIEL,
        EXCHANGE,
        TO_BOTTOM,
    };

    template<typename NodeType>
    class FileNode{
    private:
        NodeType Data;
        FileOPT Opt;
        std::shared_ptr<FileNode<NodeType>> Next;
    public:
        FileNode();
        FileNode(NodeType, FileOPT);
        FileNode(const FileNode<NodeType>& cp);
        FileNode(FileNode<NodeType>& cp);
        ~FileNode();

        void Clear();
        NodeType GetData() const;
        FileOPT GetOpt() const;
        const std::shared_ptr<FileNode<NodeType>>& GetNext() const;
        void SetNext(const std::shared_ptr<FileNode<NodeType>>& next);
    public:
        virtual void operator=(const FileNode<NodeType>& as);
        virtual NodeType GetExchg() const;
    };

    template<typename NodeType>
    FileNode<NodeType>::FileNode()
        :Data(), Opt(FileOPT::NONE), Next(nullptr)
    {}
    
    template<typename NodeType>
    FileNode<NodeType>::FileNode(NodeType data, FileOPT opt)
        :Data(data), Opt(opt), Next(nullptr)
    {}
    
    template<typename NodeType>
    FileNode<NodeType>::FileNode(const FileNode<NodeType>& cp)
        :Data(cp.GetData()), Opt(cp.Opt), Next(cp.Next)
    {}
    
    template<typename NodeType>
    FileNode<NodeType>::FileNode(FileNode<NodeType>& cp)
        :Data(cp.GetData()), Opt(cp.Opt), Next(cp.Next)
    {}

    template<typename NodeType>
    FileNode<NodeType>::~FileNode()
    {
        Clear();
    }
    
    template<typename NodeType>
    void FileNode<NodeType>::Clear()
    {
        if(Next != nullptr)
        {
            Next.reset();
            Next = nullptr;
        }
    }
    
    template<typename NodeType>
    NodeType FileNode<NodeType>::GetData() const
    {
        return Data;
    }
    
    template<typename NodeType>
    FileOPT FileNode<NodeType>::GetOpt() const
    {
        return Opt;
    }
    
    template<typename NodeType>
    const std::shared_ptr<FileNode<NodeType>>& FileNode<NodeType>::GetNext() const
    {
        // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileNode::GetNext() Enter\n");
        return Next;
        // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileNode::GetNext() Leave\n");
    }
    
    template<typename NodeType>
    void FileNode<NodeType>::SetNext(const std::shared_ptr<FileNode<NodeType>>& next)
    {
        // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileNode::SetNext() Enter\n");
        Next = next;
        // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileNode::SetNext() Leave\n");
    }
    
    template<typename NodeType>
    void FileNode<NodeType>::operator=(const FileNode<NodeType>& as)
    {
        Data = as.GetData();
        Opt = as.GetOpt();
        Next = as.GetNext();
    }

    template<typename NodeType>
    NodeType FileNode<NodeType>::GetExchg() const
    {
        // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileNode::GetExchg() Called\n");
        return Data;
    }

    //

    template<typename NodeType>
    class FileNodeEXCH: public FileNode<NodeType>{
        NodeType DataExchg;//移动操作的互换对象
    public:
        FileNodeEXCH();
        FileNodeEXCH(NodeType data, NodeType dataExchg);
        FileNodeEXCH(const FileNodeEXCH<NodeType>& cp);
        FileNodeEXCH(const FileNode<NodeType>& cp);
        ~FileNodeEXCH();

        void operator=(const FileNodeEXCH<NodeType>& as);
        virtual void operator=(const FileNode<NodeType>& as);
        virtual NodeType GetExchg() const override;
    };

    template<typename NodeType>
    FileNodeEXCH<NodeType>::FileNodeEXCH()
        :FileNode<NodeType>(), DataExchg()
    {}

    template<typename NodeType>
    FileNodeEXCH<NodeType>::FileNodeEXCH(NodeType data, NodeType dataExchg)
        :FileNode<NodeType>(data, FileOPT::EXCHANGE), DataExchg(dataExchg)
    {}

    template<typename NodeType>
    FileNodeEXCH<NodeType>::FileNodeEXCH(const FileNodeEXCH<NodeType>& cp)
        :FileNode<NodeType>(cp), DataExchg(cp.GetExchg())
    {}

    template<typename NodeType>
    FileNodeEXCH<NodeType>::FileNodeEXCH(const FileNode<NodeType>& cp)
        :FileNode<NodeType>(cp), DataExchg(cp.GetExchg())
    {}

    template<typename NodeType>
    FileNodeEXCH<NodeType>::~FileNodeEXCH()
    {}

    template<typename NodeType>
    void FileNodeEXCH<NodeType>::operator=(const FileNodeEXCH<NodeType>& as)
    {
        FileNode<NodeType>::operator=(as);
        DataExchg = as.GetExchg();
    }

    template<typename NodeType>
    void FileNodeEXCH<NodeType>::operator=(const FileNode<NodeType>& as)
    {
        FileNode<NodeType>::operator=(as);
        DataExchg = as.GetExchg();
    }

    template<typename NodeType>
    NodeType FileNodeEXCH<NodeType>::GetExchg() const
    {
        // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileNodeEXCH::GetExchg() Called\n");
        return DataExchg;
    }

    /*
        纪录列表的操作，支持对列表的添加、删除、上下移动、单项更新等操作的记录，配合DCWZ::DataList使用
        可实现数据链表与UI列表界面的操作的同步
    */

    template<typename NodeType>
    class FileList{
    private:
        std::shared_ptr<FileNode<NodeType>> Head;
        std::shared_ptr<FileNode<NodeType>> Tail;
    public:
        FileList();
        ~FileList();
        FileList(const FileList<NodeType>& cp);

        void operator=(const FileList<NodeType>& as);
        void Clear();
        void BZero();//
        const std::shared_ptr<FileNode<NodeType>>& GetHead() const;
        const std::shared_ptr<FileNode<NodeType>>& GetTail() const;
        bool AddNode(FileNode<NodeType>& add);
        void DelNode(FileNode<NodeType>& del);
        void DelNode(std::shared_ptr<FileNode<NodeType>> del);
    };

    template<typename NodeType>
    FileList<NodeType>::FileList()
        :Head(nullptr), Tail(nullptr)
    {}
    
    template<typename NodeType>
    FileList<NodeType>::~FileList()
    {
        Clear();
    }
    
    template<typename NodeType>
    FileList<NodeType>::FileList(const FileList<NodeType>& cp)
        :Head(cp.Head), Tail(cp.Tail)
    {}
    
    template<typename NodeType>
    void FileList<NodeType>::operator=(const FileList<NodeType>& as)
    {
        Head = as.GetHead();
        Tail = as.GetTail();
    }
    
    template<typename NodeType>
    void FileList<NodeType>::Clear()
    {
        std::shared_ptr<FileNode<NodeType>> Node = Head;
        std::shared_ptr<FileNode<NodeType>> NextNode = nullptr;
        for(; Node != nullptr; Node = NextNode)
        {
            NextNode = Node->GetNext();
            Node->Clear();
            Node.reset();
            Node = nullptr;
        }
        Head = nullptr;
        Tail = nullptr;
    }

    template<typename NodeType>
    void FileList<NodeType>::BZero()
    {
        if(Head != nullptr)
        {
            Head.reset();
            Head = nullptr;
        }
        if(Tail != nullptr)
        {
            Tail.reset();
            Tail = nullptr;
        }
    }
    
    template<typename NodeType>
    const std::shared_ptr<FileNode<NodeType>>& FileList<NodeType>::GetHead() const
    {
        return Head;
    }
    
    template<typename NodeType>
    const std::shared_ptr<FileNode<NodeType>>& FileList<NodeType>::GetTail() const
    {
        return Tail;
    }
    
    template<typename NodeType>
    bool FileList<NodeType>::AddNode(FileNode<NodeType>& add)
    {
#if 1
        // WRITE_DLG_FILE_MENU("DlgMenu::FileList::AddNode() Enter\n");
#endif
        switch (add.GetOpt())
        {
        case FileOPT::NONE:
            break;
#if 1
        case FileOPT::ADD_FILE: //检查此文件之前是否被删除，若是则与添加操作抵消
        {
            // WRITE_DLG_FILE_MENU("DlgMenu::FileList::AddNode() case ADD_FILE\n");
            std::shared_ptr<FileNode<NodeType>> Node = Head;
            for(; Node != nullptr; Node = Node->GetNext())
            {
                if(Node->GetOpt() == FileOPT::DEL_FIEL && Node->GetData() == add.GetData())
                {
                    DelNode(Node);                    
                    return false;
                }
            }
            if(Head == nullptr)
            {
                Head = std::make_shared<FileNode<NodeType>>(add);
                Tail = Head;
            }
            else
            {
                Tail->SetNext(std::make_shared<FileNode<NodeType>>(add));
                Tail = Tail->GetNext();
            }
            return true;
        }
#endif
#if 1
        case FileOPT::DEL_FIEL://检查此文件此前是否被添加，若是则与删除操作抵消
        {
            // WRITE_DLG_FILE_MENU("DlgMenu::FileList::AddNode() case DEL_FIEL\n");
            std::shared_ptr<FileNode<NodeType>> Node = Head;
            for(; Node != nullptr; Node = Node->GetNext())
            {
                if(Node->GetOpt() == FileOPT::ADD_FILE && Node->GetData() == add.GetData())
                {
                    DelNode(Node);
                    return false;
                }
            }            
            if(Head == nullptr)
            {
                Head = std::make_shared<FileNode<NodeType>>(add);
                Tail = Head;
            }
            else
            {
                Tail->SetNext(std::make_shared<FileNode<NodeType>>(add));
                Tail = Tail->GetNext();
            }
            return true;
        }
#endif

        case FileOPT::EXCHANGE:
        {
#if 1
            std::shared_ptr<FileNode<NodeType>> Node = Head;
            for(; Node != nullptr; Node = Node->GetNext())
            {
                if(Node->GetOpt() == FileOPT::EXCHANGE
                    && Node->GetData() == add.GetData()
                    && Node->GetExchg() == add.GetExchg())
                {
                    DelNode(Node);
                    return false;
                }
            }
#endif
#if 1
            if(Head == nullptr)
            {
                Head = std::make_shared<FileNodeEXCH<NodeType>>(add.GetData(), add.GetExchg());
                Tail = Head;
            }
            else
            {
                // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileList::AddNode(), EXCHANGE Tail != nullptr\n");
                Tail->SetNext(std::make_shared<FileNodeEXCH<NodeType>>(add.GetData(), add.GetExchg()));
                Tail = Tail->GetNext();
            }
#endif
        }
            return true;
#if 1
        case FileOPT::TO_BOTTOM:
        {
            if(Head == nullptr)
            {
                Head = std::make_shared<FileNode<NodeType>>(add);
                Tail = Head;
            }
            else
            {
                Tail->SetNext(std::make_shared<FileNode<NodeType>>(add));
                Tail = Tail->GetNext();
            }
            return true;
        }
#endif
        default:
            break;
        }
        // WRITE_DLG_MENU_AUDIO_ARG_DBG("FLST::FileList::AddNode(), Default return false\n");
        return false;
    }
    
    template<typename NodeType>
    void FileList<NodeType>::DelNode(FileNode<NodeType>& del)
    {
        std::shared_ptr<FileNode<NodeType>> Node = Head;
        std::shared_ptr<FileNode<NodeType>> PreNode = nullptr;
        for(; Node != nullptr; PreNode = Node, Node = Node->GetNext())
        {
            // WRITE_DLG_FILE_MENU("FileList::DelNode, Cycle Start\n");
            if(Node->GetData() == del.GetData() && Node->GetOpt() == del.GetOpt())
            {
                // WRITE_DLG_FILE_MENU("FileList::DelNode, Compare Success\n");
                if(PreNode == nullptr)
                {
                    // WRITE_DLG_FILE_MENU("FileList::DelNode, PreNode == nullptr\n");
                    Head = Node->GetNext();
                    // WRITE_DLG_FILE_MENU("FileList::DelNode, After Head = Node->GetNext\n");
                    Node->Clear();
                    // WRITE_DLG_FILE_MENU("FileList::DelNode, After Node->Clear()\n");
                    break;
                    // WRITE_DLG_FILE_MENU("FileList::DelNode, After Node = nullptr\n");
                }
                else if(Node->GetNext() == nullptr)
                {
                    PreNode->SetNext(Node->GetNext());
                    Tail = PreNode;
                    Node->Clear();
                    break;
                }
                else
                {
                    PreNode->SetNext(Node->GetNext());
                    Node->Clear();
                    break;
                }
            }
        }
        // WRITE_DLG_FILE_MENU("FileList::DelNode, After for(){}\n");
    }

    template<typename NodeType>
    void FileList<NodeType>::DelNode(std::shared_ptr<FileNode<NodeType>> del)
    {
        std::shared_ptr<FileNode<NodeType>> Node = Head;
        std::shared_ptr<FileNode<NodeType>> PreNode = nullptr;
        for(; Node != nullptr; PreNode = Node, Node = Node->GetNext())
        {
            if(Node->GetData() == del->GetData() && Node->GetOpt() == del->GetOpt())
            {
                if(PreNode == nullptr)
                {
                    Head = Node->GetNext();
                    Node->Clear();
                    break;
                }
                else if(Node->GetNext() == nullptr)
                {
                    PreNode->SetNext(Node->GetNext());
                    Tail = PreNode;
                    Node->Clear();
                    break;
                }
                else
                {
                    PreNode->SetNext(Node->GetNext());
                    Node->Clear();
                    break;
                }
            }
        }
    }
};

#endif // _FILELIST_HPP_