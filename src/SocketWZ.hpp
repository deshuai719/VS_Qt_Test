#ifndef _SOCKETWZ_HPP_
#define _SOCKETWZ_HPP_

#include <QString>
#include <memory>
#include <mutex>

#ifdef _WIN32

#include <Winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

namespace SOCKWZ{
    class Socket{
        WSADATA wsa;
        sockaddr_in localAddr, chipAddr;
        uint16_t localPort, chipPort;
        SOCKET sock{INVALID_SOCKET};
        QString SockAddr[4];
        std::atomic_bool LOCK;
        std::mutex mtx;
    public:
        Socket();
        ~Socket();
        bool Connect(QString localip, 
        QString localport, 
        QString chipip, 
        QString chipport);
        void DisConnect();
        int Send(char* buf, int len);
        int Recv(char* buf, int len);
        void Lock();
        void UnLock();

        const QString const* GetAddr() const;
    };

    class SockGlob{
    public:
        static std::shared_ptr<Socket> Sock;
        static std::mutex mtx;

        static bool Connect(QString localip, 
            QString localport, 
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