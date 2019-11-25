#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <string


//在Linux平台获取本机的ip地址,非本地回环和docker
std::string getLocalIP(){

    char buf[512];
    struct ifconf ifconf;
    ifconf.ifc_len =512;
    ifconf.ifc_buf = buf;

    int sockfd;
    if( (sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0 ){
        return std::string("");
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf);

    struct ifreq *ifreq = (struct ifreq*)ifconf.ifc_buf;
    int size = ifconf.ifc_len / static_cast<int>(sizeof (struct ifreq));
    for (auto i = size; i > 0; i--) {
        if(ifreq->ifr_flags == AF_INET){
            if( (std::string(ifreq->ifr_name).find("lo") == std::string::npos) &&
                (std::string(ifreq->ifr_name).find("docker") == std::string::npos)){
                return std::string(inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
            }
            ifreq++;
        }
    }
    return std::string("");
}
