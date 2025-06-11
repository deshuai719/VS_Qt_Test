#include "SocketWZ.hpp"

SOCKWZ::Socket::Socket()
    :wsa(),
    localAddr(),
    chipAddr(),
    localPort(),
    chipPort(),
    sock(INVALID_SOCKET),
    SockAddr(),
    LOCK(),
    mtx()
{
    LOCK.store(0);
    for(int i = 0; i < 4; i++)
    {
        SockAddr[i] = "";
    }
}

SOCKWZ::Socket::~Socket()
{}

bool SOCKWZ::Socket::Connect(QString localip, 
    QString localport, 
    QString chipip, 
    QString chipport)
{
    Lock();
    // std::unique_lock<std::mutex> lk(mtx);
    if(sock == INVALID_SOCKET)
    {
        SockAddr[0] = localip;
        SockAddr[1] = localport;
        SockAddr[2] = chipip;
        SockAddr[3] = chipport;

        if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            WSACleanup();
            UnLock();
            return false;
        }
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sock == INVALID_SOCKET)
        {
            "error";
            closesocket(sock);
            WSACleanup();
            UnLock();
            return false;
        }
        u_long iMode = 1;
        ioctlsocket(sock, FIONBIO, &iMode);//设为非阻塞
        localPort = localport.toUShort();
        localAddr.sin_family = AF_INET;
        localAddr.sin_port = htons(localPort);
        inet_pton(AF_INET, localip.toStdString().c_str(), &localAddr.sin_addr);
        if(bind(sock, (sockaddr*)&localAddr, sizeof(sockaddr)) == SOCKET_ERROR)
        {
            closesocket(sock);
            WSACleanup();
            UnLock();
            return false;
        }
        memset(&chipAddr, 0, sizeof(sockaddr));
        chipPort = chipport.toUShort();
        chipAddr.sin_family = AF_INET;
        chipAddr.sin_port = htons(chipPort);
        inet_pton(AF_INET, chipip.toStdString().c_str(), &chipAddr.sin_addr);
    }
    UnLock();
    return true;
}

void SOCKWZ::Socket::DisConnect()
{
    Lock();
    // std::unique_lock<std::mutex> lk(mtx);
    if(sock == INVALID_SOCKET)
    {
        UnLock();
        return;
    }
    shutdown(sock, SD_BOTH);
    closesocket(sock);
    WSACleanup();
    sock = INVALID_SOCKET;
    UnLock();
}

int SOCKWZ::Socket::Send(char* buf, int len)
{
    Lock();
    // std::unique_lock<std::mutex> lk(mtx);
    if(sock == INVALID_SOCKET)
    {
        UnLock();
        return SOCKET_ERROR;
    }
    int ret = sendto(sock, buf, len, 0, (sockaddr*)&chipAddr, sizeof(sockaddr));
    UnLock();
    return ret;
}

int SOCKWZ::Socket::Recv(char* buf, int len)
{
    int socklen = sizeof(sockaddr);
    Lock();
    // std::unique_lock<std::mutex> lk(mtx);
    if(sock == INVALID_SOCKET)
    {
        UnLock();
        return SOCKET_ERROR;
    }
    int ret = recvfrom(sock, buf, len, 0, (sockaddr*)&chipAddr, &socklen);
    UnLock();
    return ret;
}

void SOCKWZ::Socket::Lock()
{
    while(LOCK.exchange(1));
}

void SOCKWZ::Socket::UnLock()
{
    LOCK.exchange(0);
}

const QString const* SOCKWZ::Socket::GetAddr() const
{
    return SockAddr;
}

//

bool SOCKWZ::SockGlob::Connect(QString localip, 
    QString localport, 
    QString chipip, 
    QString chipport)
{
    std::unique_lock<std::mutex> lk(mtx);
    if(Sock != nullptr)
    {
        return Sock->Connect(localip, localport, chipip, chipport);
    }
    return false;
}

void SOCKWZ::SockGlob::DisConnect()
{
    std::unique_lock<std::mutex> lk(mtx);
    if(Sock != nullptr)
    {
        Sock->DisConnect();
    }
}

void SOCKWZ::SockGlob::Clear()
{
    std::unique_lock<std::mutex> lk(mtx);
    if(Sock != nullptr)
    {   
        Sock->DisConnect();
        Sock.reset();
        Sock = nullptr;
    }
}

int SOCKWZ::SockGlob::Send(char *buf, int len)
{
    std::unique_lock<std::mutex> lk(mtx);
    if(Sock != nullptr)
    {
        return Sock->Send(buf, len);
    }
    return SOCKET_ERROR;
}

int SOCKWZ::SockGlob::Recv(char* buf, int len)
{
    std::unique_lock<std::mutex> lk(mtx);
    if(Sock != nullptr)
    {
        return Sock->Recv(buf, len);
    }
    return SOCKET_ERROR;
}

std::shared_ptr<SOCKWZ::Socket> SOCKWZ::SockGlob::Sock = std::make_shared<SOCKWZ::Socket>();
std::mutex SOCKWZ::SockGlob::mtx;