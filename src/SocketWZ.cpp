#include "SocketWZ.hpp"
#include <QDebug>
#include "LogWZ.hpp"
#include <winsock2.h>
#include <chrono>

SOCKWZ::Socket::Socket()
    :wsa(),                                                                      // 初始化 WinSock 数据结构，通常为 WSADATA 类型
    localAddr(),                                                                 // 初始化本地地址结构，通常为 sockaddr_in 类型
    chipAddr(),                                                                  // 初始化目标芯片地址结构，通常为 sockaddr_in 类型
    sendLocalAddr(),                                                             // 初始化发送socket本地地址结构
    localPort(),                                                                 // 初始化本地端口，类型应为整型，初值为0
    chipPort(),                                                                  // 初始化芯片端口，类型应为整型，初值为0
    sendLocalPort(),                                                             // 初始化发送socket本地端口
    RecvSock(INVALID_SOCKET),                                                    // 初始化接收 socket 句柄为无效值
    SendSock(INVALID_SOCKET),                                                    // 初始化发送 socket 句柄为无效值
    SockAddr(),                                                                  // 初始化地址字符串数组，现在为 QString[5]
    LOCK(),                                                                      // 初始化原子锁变量，通常为 std::atomic<int>
    mtx()                                                                        // 初始化互斥锁，通常为 std::mutex
{
    LOCK.store(0);                                                               // 将原子锁变量设置为0，表示未加锁状态
    for (int i = 0; i < 5; i++)                                                  // 更新为5个元素
    {
        SockAddr[i] = "";                                                        // 将地址字符串数组的每一项初始化为空字符串
    }
}

SOCKWZ::Socket::~Socket()
{
}

bool SOCKWZ::Socket::Connect(QString localip,
    QString recvport,
    QString sendport,
    QString chipip,
    QString chipport)
{
    Lock();                                                                      // 加锁，防止多线程同时操作 socket

    if (RecvSock == INVALID_SOCKET)                                              // 如果当前接收socket还未创建
    {
        SockAddr[0] = localip;                                                   // 保存本地IP
        SockAddr[1] = recvport;                                                  // 保存接收端口
        SockAddr[2] = sendport;                                                  // 保存发送端口
        SockAddr[3] = chipip;                                                    // 保存芯片IP
        SockAddr[4] = chipport;                                                  // 保存芯片端口

        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)                               // 初始化 WinSock2，失败则清理并返回
        {
            WSACleanup();
            UnLock();
            return false;
        }

        // ===== 创建接收socket =====
        RecvSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);                     // 创建接收 UDP socket
        if (RecvSock == INVALID_SOCKET)                                          // 创建失败
        {
            closesocket(RecvSock);                                               // 关闭 socket（虽然已经无效）
            WSACleanup();                                                        // 清理 WinSock
            UnLock();
            return false;
        }
        u_long iMode = 1;
        ioctlsocket(RecvSock, FIONBIO, &iMode);                                  // 设置接收socket为非阻塞模式

        // 设置接收socket缓冲区大小
        int recvBufSize = 1024 * 1024; // 1MB接收缓冲区
        setsockopt(RecvSock, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize));

        localPort = recvport.toUShort();                                         // 接收端口
        localAddr.sin_family = AF_INET;                                          // 地址族设为 IPv4
        localAddr.sin_port = htons(localPort);                                   // 接收端口号，主机字节序转网络字节序
        inet_pton(AF_INET, localip.toStdString().c_str(), &localAddr.sin_addr);  // 本地IP字符串转为网络地址

        if (bind(RecvSock, (sockaddr*)&localAddr, sizeof(sockaddr)) == SOCKET_ERROR) // 绑定接收socket到本地接收端口
        {
            closesocket(RecvSock);                                               // 绑定失败，关闭 socket
            WSACleanup();                                                        // 清理 WinSock
            UnLock();
            return false;
        }

        // ===== 创建发送socket =====
        SendSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);                     // 创建发送 UDP socket
        if (SendSock == INVALID_SOCKET)                                          // 创建失败
        {
            closesocket(RecvSock);                                               // 关闭接收socket
            closesocket(SendSock);
            WSACleanup();
            UnLock();
            return false;
        }
        ioctlsocket(SendSock, FIONBIO, &iMode);                                  // 设置发送socket为非阻塞模式

        // 设置发送socket缓冲区大小
        int sendBufSize = 512 * 1024; // 512KB发送缓冲区  
        setsockopt(SendSock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));

        sendLocalPort = sendport.toUShort();                                     // 发送端口
        sendLocalAddr.sin_family = AF_INET;                                      // 地址族设为 IPv4
        sendLocalAddr.sin_port = htons(sendLocalPort);                           // 发送socket本地端口号
        inet_pton(AF_INET, localip.toStdString().c_str(), &sendLocalAddr.sin_addr); // 发送socket本地IP

        if (bind(SendSock, (sockaddr*)&sendLocalAddr, sizeof(sockaddr)) == SOCKET_ERROR) // 绑定发送socket到本地发送端口
        {
            closesocket(RecvSock);                                               // 绑定失败，关闭接收socket
            closesocket(SendSock);                                               // 关闭发送socket
            WSACleanup();
            UnLock();
            return false;
        }

        // ===== 设置目标芯片地址 =====
        memset(&chipAddr, 0, sizeof(sockaddr));                                  // 清空芯片地址结构体
        chipPort = chipport.toUShort();                                          // 芯片端口字符串转为无符号短整型
        chipAddr.sin_family = AF_INET;                                           // 地址族设为 IPv4
        chipAddr.sin_port = htons(chipPort);                                     // 芯片端口号，主机字节序转网络字节序
        inet_pton(AF_INET, chipip.toStdString().c_str(), &chipAddr.sin_addr);    // 芯片IP字符串转为网络地址
    }
    UnLock();                                                                    // 解锁
    return true;                                                                 // 返回连接成功
}

void SOCKWZ::Socket::DisConnect()
{
    Lock();
    
    // 关闭接收socket
    if (RecvSock != INVALID_SOCKET)
    {
        shutdown(RecvSock, SD_BOTH);
        closesocket(RecvSock);
        RecvSock = INVALID_SOCKET;
    }
    
    // 关闭发送socket
    if (SendSock != INVALID_SOCKET)
    {
        shutdown(SendSock, SD_BOTH);
        closesocket(SendSock);
        SendSock = INVALID_SOCKET;
    }
    
    // 清理WinSock
    WSACleanup();
    
    UnLock();
}

int SOCKWZ::Socket::Send(char* buf, int len)
{
    if (SendSock == INVALID_SOCKET)  // 使用发送socket
    {
        return SOCKET_ERROR;
    }

    // 统计两次sendto调用的间隔时间
    static std::chrono::steady_clock::time_point last_send_time;
    auto now = std::chrono::steady_clock::now();
    if (last_send_time.time_since_epoch().count() != 0) {
        auto interval = std::chrono::duration_cast<std::chrono::microseconds>(now - last_send_time).count();
        WRITE_TASK_DATA_SEND_DBG("sendto 间隔时间: %lld us\n", interval);
    }
    last_send_time = now;

    auto t_send_start = std::chrono::steady_clock::now();
    int ret = sendto(SendSock, buf, len, 0, (sockaddr*)&chipAddr, sizeof(sockaddr)); // 使用发送socket
    auto t_send_end = std::chrono::steady_clock::now();
    auto sendto_ms = std::chrono::duration_cast<std::chrono::microseconds>(t_send_end - t_send_start).count();
    WRITE_TASK_DATA_SEND_DBG("sendto 实际耗时: %lld us\n", sendto_ms);

    if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        // 对WSAEWOULDBLOCK错误给予更友好的处理，避免过多日志输出
        if (err == WSAEWOULDBLOCK) {
            // WSAEWOULDBLOCK (10035) 是非阻塞socket的正常行为，表示缓冲区已满
            // 只在调试模式下输出详细信息，减少正常运行时的日志噪音
            WRITE_TASK_DATA_SEND_DBG("[Socket::Send] Socket buffer full (WSAEWOULDBLOCK), this is normal for non-blocking sockets\n");
        } else {
            // 其他错误仍然输出到调试日志
            qDebug() << "[Socket::Send] sendto failed, WSAGetLastError=" << err;
        }
    }
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
    if (RecvSock == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    auto t_recv_start = std::chrono::steady_clock::now(); // 记录开始时间

    // 设置8ms超时
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(RecvSock, &readfds);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 8000; // 8ms

    auto t_select_start = std::chrono::steady_clock::now(); // select前时间
    int sel = select(0, &readfds, nullptr, nullptr, &timeout);
    auto t_select_end = std::chrono::steady_clock::now();   // select后时间
    auto select_cost = std::chrono::duration_cast<std::chrono::microseconds>(t_select_end - t_select_start).count();
    if (select_cost > 20000) {
        WRITE_TASK_DATA_SEND_DBG("!!!select调用耗时超20ms: %lld us\n", select_cost);
    }

    int ret = SOCKET_ERROR;
    auto t_if_check = std::chrono::steady_clock::now(); // select结束到if判断前时间

    if (sel > 0 && FD_ISSET(RecvSock, &readfds)) {
        auto t_recvfrom_start = std::chrono::steady_clock::now(); // 记录开始时间
        auto select_to_if_cost = std::chrono::duration_cast<std::chrono::microseconds>(t_recvfrom_start - t_if_check).count();
        if (select_to_if_cost > 20000) 
        {
            WRITE_TASK_DATA_SEND_DBG("!!!select结束到进入if判断间隔超20ms: %lld us\n", select_to_if_cost);
        }
        ret = recvfrom(RecvSock, buf, len, 0, (sockaddr*)&chipAddr, &socklen);
        auto t_recvfrom_end = std::chrono::steady_clock::now(); // 记录结束时间
        auto recvfrom_cost = std::chrono::duration_cast<std::chrono::microseconds>(t_recvfrom_end - t_recvfrom_start).count();
        if (recvfrom_cost > 20000) {
            WRITE_TASK_DATA_SEND_DBG("!!!Socket Recv发送耗时超20ms: %lld us\n", recvfrom_cost);
        }
        WRITE_TASK_DATA_SEND_DBG("select1=%d, recvfrom ret=%d\n", sel, ret);
    }
    else {
        WRITE_TASK_DATA_SEND_DBG("select0=%d, recvfrom ret=%d\n", sel, ret);
    }

    auto t_recv_end = std::chrono::steady_clock::now(); // 记录结束时间
    auto recv_cost= std::chrono::duration_cast<std::chrono::microseconds>(t_recv_end - t_recv_start).count();
    if (recv_cost > 20000) {
        WRITE_TASK_DATA_SEND_DBG("!!!Socket Recv整体耗时超20ms: %lld us\n", recv_cost);
    }

    return ret;
}

void SOCKWZ::Socket::Lock()
{
    while (LOCK.exchange(1));
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
    QString recvport,
    QString sendport,
    QString chipip,
    QString chipport)
{
    std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        return Sock->Connect(localip, recvport, sendport, chipip, chipport);
    }
    return false;
}

void SOCKWZ::SockGlob::DisConnect()
{
    std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        Sock->DisConnect();
    }
}

void SOCKWZ::SockGlob::Clear()
{
    std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        Sock->DisConnect();
        Sock.reset();
        Sock = nullptr;
    }
}

int SOCKWZ::SockGlob::Send(char* buf, int len)
{
    //std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        return Sock->Send(buf, len);
    }
    return SOCKET_ERROR;
}

int SOCKWZ::SockGlob::Recv(char* buf, int len)
{
    //std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        return Sock->Recv(buf, len);
    }
    return SOCKET_ERROR;
}

std::shared_ptr<SOCKWZ::Socket> SOCKWZ::SockGlob::Sock = std::make_shared<SOCKWZ::Socket>();
std::mutex SOCKWZ::SockGlob::mtx;