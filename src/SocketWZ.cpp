#include "SocketWZ.hpp"
#include <QDebug>
#include "LogWZ.hpp"
#include <winsock2.h>
#include <chrono>

SOCKWZ::Socket::Socket()
    :wsa(),                                                                      // 初始化 WinSock 数据结构，通常为 WSADATA 类型
    localAddr(),                                                                 // 初始化本地地址结构，通常为 sockaddr_in 类型
    chipAddr(),                                                                  // 初始化目标芯片地址结构，通常为 sockaddr_in 类型
    localPort(),                                                                 // 初始化本地端口，类型应为整型，初值为0
    chipPort(),                                                                  // 初始化芯片端口，类型应为整型，初值为0
    sock(INVALID_SOCKET),                                                        // 初始化 socket 句柄为无效值，表示尚未创建 socket
    SockAddr(),                                                                  // 初始化地址字符串数组，通常为 QString[4]
    LOCK(),                                                                      // 初始化原子锁变量，通常为 std::atomic<int>
    mtx()                                                                        // 初始化互斥锁，通常为 std::mutex
{
    LOCK.store(0);                                                               // 将原子锁变量设置为0，表示未加锁状态
    for (int i = 0; i < 4; i++)
    {
        SockAddr[i] = "";                                                        // 将地址字符串数组的每一项初始化为空字符串
    }
}

SOCKWZ::Socket::~Socket()
{}

//bool SOCKWZ::Socket::Connect(QString localip,
//    QString localport,
//    QString chipip,
//    QString chipport)
//{
//    Lock();                                                                      // 加锁，防止多线程同时操作 socket
//
//    // std::unique_lock<std::mutex> lk(mtx);                                     // 这行被注释掉，原本用于互斥锁保护
//
//    if (sock == INVALID_SOCKET)                                                  // 如果当前 socket 还未创建
//    {
//        SockAddr[0] = localip;                                                   // 保存本地IP
//        SockAddr[1] = localport;                                                 // 保存本地端口
//        SockAddr[2] = chipip;                                                    // 保存芯片IP
//        SockAddr[3] = chipport;                                                  // 保存芯片端口
//
//        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)                               // 初始化 WinSock2，失败则清理并返回
//        {
//            WSACleanup();
//            UnLock();
//            return false;
//        }
//        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);                         // 创建 UDP socket
//        if (sock == INVALID_SOCKET)                                              // 创建失败
//        {
//            "error";                                                             // 这行无实际作用
//            closesocket(sock);                                                   // 关闭 socket（虽然已经无效）
//            WSACleanup();                                                        // 清理 WinSock
//            UnLock();
//            return false;
//        }
//        //u_long iMode = 1;
//        //u_long iMode = 0;                                                        //直接设置为阻塞模式
//        //ioctlsocket(sock, FIONBIO, &iMode);                                      // 设置 socket 为非阻塞模式
//
//        localPort = localport.toUShort();                                        // 将本地端口字符串转为无符号短整型
//        localAddr.sin_family = AF_INET;                                          // 地址族设为 IPv4
//        localAddr.sin_port = htons(localPort);                                   // 本地端口号，主机字节序转网络字节序
//        inet_pton(AF_INET, localip.toStdString().c_str(), &localAddr.sin_addr);  // 本地IP字符串转为网络地址
//
//        if (bind(sock, (sockaddr*)&localAddr, sizeof(sockaddr)) == SOCKET_ERROR) // 绑定本地地址和端口
//        {
//            closesocket(sock);                                                   // 绑定失败，关闭 socket
//            WSACleanup();                                                        // 清理 WinSock
//            UnLock();
//            return false;
//        }
//
//        memset(&chipAddr, 0, sizeof(sockaddr));                                  // 清空芯片地址结构体
//        chipPort = chipport.toUShort();                                          // 芯片端口字符串转为无符号短整型
//        chipAddr.sin_family = AF_INET;                                           // 地址族设为 IPv4
//        chipAddr.sin_port = htons(chipPort);                                     // 芯片端口号，主机字节序转网络字节序
//        inet_pton(AF_INET, chipip.toStdString().c_str(), &chipAddr.sin_addr);    // 芯片IP字符串转为网络地址
//    }
//    UnLock();                                                                    // 解锁
//    return true;                                                                 // 返回连接成功
//}

int SOCKWZ::Socket::Connect(QString localip,
    QString localport,
    QString chipip,
    QString chipport)
{
    Lock();                                                                      // 加锁，防止多线程同时操作 socket

    // std::unique_lock<std::mutex> lk(mtx);                                     // 这行被注释掉，原本用于互斥锁保护

    if (sock == INVALID_SOCKET)                                                  // 如果当前 socket 还未创建
    {
        SockAddr[0] = localip;                                                   // 保存本地IP
        SockAddr[1] = localport;                                                 // 保存本地端口
        SockAddr[2] = chipip;                                                    // 保存芯片IP
        SockAddr[3] = chipport;                                                  // 保存芯片端口

        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            int err = WSAGetLastError();
            WSACleanup();
            UnLock();
            return err;
        }
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET) {
            int err = WSAGetLastError();
            closesocket(sock);
            WSACleanup();
            UnLock();
            return err;
        }
        //u_long iMode = 1;
        //u_long iMode = 0;                                                        //直接设置为阻塞模式
        //ioctlsocket(sock, FIONBIO, &iMode);                                      // 设置 socket 为非阻塞模式

        localPort = localport.toUShort();                                        // 将本地端口字符串转为无符号短整型
        localAddr.sin_family = AF_INET;                                          // 地址族设为 IPv4
        localAddr.sin_port = htons(localPort);                                   // 本地端口号，主机字节序转网络字节序
        inet_pton(AF_INET, localip.toStdString().c_str(), &localAddr.sin_addr);  // 本地IP字符串转为网络地址

        if (bind(sock, (sockaddr*)&localAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
            int err = WSAGetLastError();
            closesocket(sock);
            WSACleanup();
            UnLock();
            return err;
        }

        memset(&chipAddr, 0, sizeof(sockaddr));                                  // 清空芯片地址结构体
        chipPort = chipport.toUShort();                                          // 芯片端口字符串转为无符号短整型
        chipAddr.sin_family = AF_INET;                                           // 地址族设为 IPv4
        chipAddr.sin_port = htons(chipPort);                                     // 芯片端口号，主机字节序转网络字节序
        inet_pton(AF_INET, chipip.toStdString().c_str(), &chipAddr.sin_addr);    // 芯片IP字符串转为网络地址
    }
    UnLock();                                                                    // 解锁
    return 0;                                                                 // 返回连接成功
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
    if(sock == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }
    //// 记录开始时间
    //auto start = std::chrono::high_resolution_clock::now();

    //// 记录结束时间
    //auto end = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    int ret = sendto(sock, buf, len, 0, (sockaddr*)&chipAddr, sizeof(sockaddr));

   /* if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        qDebug() << "[Socket::Send] sendto failed, WSAGetLastError=" << err;
    }*/

    //// 打印耗时（微秒）
    //qDebug() << "[Socket::Send] sendto耗时:" << duration << "微秒";
    return ret;
}

//int SOCKWZ::Socket::Recv(char* buf, int len)
//{
//    int socklen = sizeof(sockaddr);
//    //Lock();
//    // std::unique_lock<std::mutex> lk(mtx);
//    if(sock == INVALID_SOCKET)
//    {
//        //UnLock();
//        return SOCKET_ERROR;
//    }
//    int ret = recvfrom(sock, buf, len, 0, (sockaddr*)&chipAddr, &socklen);
//    //UnLock();
//    return ret;
//}

int SOCKWZ::Socket::Recv(char* buf, int len)
{
    int socklen = sizeof(sockaddr);
    //Lock();
    if (sock == INVALID_SOCKET)
    {
        //UnLock();
        return SOCKET_ERROR;
    }

    // 设置8ms超时
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 8000; // 8ms

    int sel = select(0, &readfds, nullptr, nullptr, &timeout);
    //qDebug() << "select returned:" << sel;
    int ret = SOCKET_ERROR;
    if (sel > 0 && FD_ISSET(sock, &readfds)) {
        ret = recvfrom(sock, buf, len, 0, (sockaddr*)&chipAddr, &socklen);
        /*qDebug() << "recvfrom ret:" << ret;*/
    }
   /* else {
        qDebug() << "no data, timeout or error";
    }*/
    //UnLock();
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

//bool SOCKWZ::SockGlob::Connect(QString localip, 
//    QString localport, 
//    QString chipip, 
//    QString chipport)
//{
//    std::unique_lock<std::mutex> lk(mtx);
//    if(Sock != nullptr)
//    {
//        return Sock->Connect(localip, localport, chipip, chipport);
//    }
//    return false;
//}

int SOCKWZ::SockGlob::Connect(QString localip,
    QString localport,
    QString chipip,
    QString chipport)
{
    std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        return Sock->Connect(localip, localport, chipip, chipport);
    }
    return WSAENOTSOCK; // 或其他合适的错误码
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
    //std::unique_lock<std::mutex> lk(mtx);
    if(Sock != nullptr)
    {
        return Sock->Send(buf, len);
    }
    return SOCKET_ERROR;
}

int SOCKWZ::SockGlob::Recv(char* buf, int len)
{
    //std::unique_lock<std::mutex> lk(mtx);
    if(Sock != nullptr)
    {
        return Sock->Recv(buf, len);
    }
    return SOCKET_ERROR;
}

std::shared_ptr<SOCKWZ::Socket> SOCKWZ::SockGlob::Sock = std::make_shared<SOCKWZ::Socket>();
std::mutex SOCKWZ::SockGlob::mtx;