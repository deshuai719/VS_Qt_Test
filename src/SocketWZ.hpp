#ifndef _SOCKETWZ_HPP_
#define _SOCKETWZ_HPP_

#include <QString>
#include <memory>
#include <mutex>

#ifdef _WIN32

#include <Winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

namespace SOCKWZ {
    class Socket {
        WSADATA wsa;
        sockaddr_in localAddr, chipAddr;
        sockaddr_in sendLocalAddr;  // 发送socket的本地地址
        uint16_t localPort, chipPort;
        uint16_t sendLocalPort;     // 发送socket的本地端口
        SOCKET RecvSock{ INVALID_SOCKET };  // 接收socket
        SOCKET SendSock{ INVALID_SOCKET };  // 发送socket
        QString SockAddr[5];        // 扩展为5个：本地IP、接收端口、发送端口、芯片IP、芯片端口
        std::atomic_bool LOCK;
        std::mutex mtx;
    public:
        Socket();
        ~Socket();
        bool Connect(QString localip,
            QString recvport,        // 接收端口参数
            QString sendport,        // 发送端口参数
            QString chipip,
            QString chipport);
        void DisConnect();
        int Send(char* buf, int len);
        int Recv(char* buf, int len);
        void Lock();
        void UnLock();

        const QString const* GetAddr() const;
    };

    class SockGlob {
    public:
        static std::shared_ptr<Socket> Sock;
        static std::mutex mtx;

        static bool Connect(QString localip,
            QString recvport,        // 接收端口参数
            QString sendport,        // 发送端口参数
            QString chipip,
            QString chipport);
        static void DisConnect();
        static void Clear();
        static int Send(char* buf, int len);
        static int Recv(char* buf, int len);
    };
};

#endif

#endif // _SOCKETWZ_HPP_