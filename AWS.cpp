#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <limits.h>
#include <iomanip>
#include <map>
#include <vector>


#define AWSPORT "24175"  // the port users will be connecting to
#define AWS_UDP_PORT "23175"
#define PORT_A "21175"
#define PORT_B "22175"
#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100
#define LOCALHOST "127.0.0.1"
const int INF=INT_MAX;

using namespace std;

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

vector<string> split(const string &str, const string &pattern)
{
    vector<string> res;
    if(str == "")
        return res;
    string strs = str + pattern;
    size_t pos = strs.find(pattern);

    while(pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        strs = strs.substr(pos+1, strs.size());
        pos = strs.find(pattern);
    }

    return res;
}

int main(void)
{
    int sockfd, new_fd,sock_UDP,sockrev;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints,hints1,hints2,hints3, *servinfo1,*servinfo2,*servinfo3,*servinfo, *servinfo4, *p1,*p2,*p3,*p,*p4;
    //int numbytes;
    struct sockaddr_storage their_addr,their_addr1,their_addr2,their_addr3; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv1,rv2,rv3,rv,rv4;
    socklen_t addr_len, addr_len2,addr_len3;
    char map_name[MAXDATASIZE],vertex_num[MAXDATASIZE],file_size[MAXDATASIZE];
    int sockfd_UDP_A,sockfd_UDP_B;

    cout << "The AWS is up and running." << endl;

    //////////////////////connect to the client/////////////////////////////

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, AWSPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }







    //printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        addr_len=sizeof their_addr1;
        char propagation[MAXDATASIZE];
        char transmission[MAXDATASIZE];

        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        //printf("server: got connection from %s\n", s);

        if((recv(new_fd,&map_name,sizeof map_name,0))==-1){
            perror("recv");
            exit(1);
        }
        if((recv(new_fd,&vertex_num,sizeof vertex_num,0))==-1){
            perror("recv");
            exit(1);
        }
        if((recv(new_fd,&file_size,sizeof file_size,0))==-1){
            perror("recv");
            exit(1);
        }
        cout<<"The AWS has received map ID "<<map_name<<", start vertex "<<vertex_num<<" and file size "<<file_size<<" from the client using TCP over port 24175"<<endl;
        /////////////////////////connect to serverA///////////////////////////

        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        //hints.ai_flags=AI_PASSIVE;

        if ((rv1=getaddrinfo(LOCALHOST,PORT_A,&hints,&servinfo1))!= 0){
            fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(rv1));
            return 1;
        }
        // loop through all the results and make a socket

        for(p1 = servinfo1; p1 != NULL; p1 = p1->ai_next) {
            if ((sockfd_UDP_A = socket(p1->ai_family, p1->ai_socktype,p1->ai_protocol)) == -1) {
                perror("talker: socket");
                continue;
            }

            break;
        }

        if (p1 == NULL) {
            fprintf(stderr, "talker: failed to create socket\n");
            return 2;
        }


        //int vertex=stoi(vertex_num);
        //int file=stoi(file_size);

        if ((sendto(sockfd_UDP_A, &map_name, sizeof map_name, 0,
                    p1->ai_addr, p1->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        if ((sendto(sockfd_UDP_A, &vertex_num, sizeof vertex_num, 0,
                    p1->ai_addr, p1->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        //close(sockfd_UDP_A);

        cout<<"The AWS has sent map ID and starting vertex to server A using UDP over port 23175"<<endl;
        close(sockfd_UDP_A);
        freeaddrinfo(servinfo1);


        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags=AI_PASSIVE;

        if ((rv2=getaddrinfo(NULL,AWS_UDP_PORT,&hints,&servinfo2))!= 0){
            fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(rv2));
            return 1;
        }
        // loop through all the results and make a socket

        for(p2 = servinfo2; p2 != NULL; p2 = p2->ai_next) {
            if ((sock_UDP = socket(p2->ai_family, p2->ai_socktype,p2->ai_protocol)) == -1) {
                perror("talker: socket");
                continue;
            }
            if (bind(sock_UDP, p2->ai_addr, p2->ai_addrlen) == -1) {
                close(sock_UDP);
                perror("listener: bind");
                continue;
            }


            break;
        }

        if (p2 == NULL) {
            fprintf(stderr, "talker: failed to create socket\n");
            return 2;
        }
        freeaddrinfo(servinfo2);
        int dics[12];
        char dics_char[MAXDATASIZE];
        char path_char[MAXDATASIZE];
        int path[12];
        memset(path,0,sizeof path);
        addr_len2 = sizeof their_addr2;


        if (recvfrom(sock_UDP, &dics_char, sizeof dics_char, 0,(struct sockaddr *)&their_addr2, &addr_len2)==-1){
            perror("recvfrom");
            exit(1);
        };

        if (recvfrom(sock_UDP, &path_char, sizeof path_char, 0,(struct sockaddr *)&their_addr2, &addr_len2)==-1){
            perror("recvfrom");
            exit(1);
        };

        //cout<<path[1]<<endl;
        if (recvfrom(sock_UDP, &propagation, sizeof propagation, 0,(struct sockaddr *)&their_addr2, &addr_len2)==-1){
            perror("recvfrom");
            exit(1);
        };
        //cout<<propagation<<endl;
        if (recvfrom(sock_UDP, &transmission, sizeof transmission, 0,(struct sockaddr *)&their_addr2, &addr_len2)==-1){
            perror("recvfrom");
            exit(1);
        };
        //cout<<transmission<<endl;

        //cout<<path_str<<endl<<dics_str<<endl;

        //cout<<propagation<<endl<<transmission<<endl;
        string dics_str(dics_char);
        string path_str(path_char);
        vector<string>dics_tmp,path_tmp;
        dics_tmp=split(dics_str," ");
        path_tmp=split(path_str," ");
        for(int i =0;i<dics_tmp.size();++i){
            dics[i]=stoi(dics_tmp[i]);
        }
        for(int i =0;i<path_tmp.size();++i){
            path[i]=stoi(path_tmp[i]);
        }


        cout<<"The AWS has received shortest path from server A:"<<endl;
        cout<<"-------------------------------------------------"<<endl;
        cout<<"Destination           Min Length"<<endl;
        cout<<"-------------------------------------------------"<<endl;

        for(int i=0;i<dics_tmp.size();++i) {
            if (path[i] != 0 && path[i] != INF) {
                cout << dics[i] << "                     " << path[i] << endl;

            }
        }
        close(sock_UDP);
        ///////////////////////////connect to serverB//////////////////////////////////
        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        //hints.ai_flags=AI_PASSIVE;
        if ((rv3=getaddrinfo(LOCALHOST,PORT_B,&hints,&servinfo3))!= 0){
            fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(rv3));
            return 1;
        }
        // loop through all the results and make a socket
        for(p3 = servinfo3; p3 != NULL; p3 = p3->ai_next) {
            if ((sockfd_UDP_B = socket(p3->ai_family, p3->ai_socktype,p3->ai_protocol)) == -1) {
                perror("talker: socket");
                continue;
            }
            break;
        }

        if (p3 == NULL) {
            fprintf(stderr, "talker: failed to create socket\n");
            return 2;
        }


        if ((sendto(sockfd_UDP_B, &propagation, sizeof propagation, 0,
                p3->ai_addr, p3->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        if ((sendto(sockfd_UDP_B, &transmission, sizeof transmission, 0,
                    p3->ai_addr, p3->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        if ((sendto(sockfd_UDP_B, &dics_char, sizeof dics_char, 0,
                    p3->ai_addr, p3->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        if ((sendto(sockfd_UDP_B, &path_char, sizeof path_char, 0,
                    p3->ai_addr, p3->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        if ((sendto(sockfd_UDP_B, &file_size, sizeof file_size, 0,
                    p3->ai_addr, p3->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }


        cout<<"The AWS has sent path length, propagation speed and transmission speed to server B using UDP over port 23175"<<endl;
        //cout<<propagation<<endl<<transmission<<endl;
        freeaddrinfo(servinfo3);
        close(sockfd_UDP_B);


        ///////////////////////////////////////////////RECV from Server B///////////////////////////////////////////
        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags=AI_PASSIVE;
        if ((rv4=getaddrinfo(NULL,AWS_UDP_PORT,&hints,&servinfo4))!= 0){
            fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(rv4));
            return 1;
        }
        // loop through all the results and make a socket
        for(p4 = servinfo4; p4 != NULL; p4 = p4->ai_next) {
            if ((sockrev = socket(p4->ai_family, p4->ai_socktype,p4->ai_protocol)) == -1) {
                perror("listener: socket");
                continue;
            }
            if (bind(sockrev, p4->ai_addr, p4->ai_addrlen) == -1) {
                close(sockrev);
                perror("listener: bind");
                continue;
            }
            break;
        }

        if (p4 == NULL) {
            fprintf(stderr, "listener: failed to create socket\n");
            return 2;
        }
        freeaddrinfo(servinfo4);

        addr_len3 = sizeof their_addr3;

        double tt;
        double tp[12];
        double delay[12];
        char tt_char[MAXDATASIZE],tp_char[MAXDATASIZE],delay_char[MAXDATASIZE];
        if (recvfrom(sockrev, &tt_char, sizeof tt_char, 0,(struct sockaddr *)&their_addr3, &addr_len3)==-1){
            perror("recvfrom");
            exit(1);
        };
        if (recvfrom(sockrev, &tp_char, sizeof tp_char, 0,(struct sockaddr *)&their_addr3, &addr_len3)==-1){
            perror("recvfrom");
            exit(1);
        };
        if (recvfrom(sockrev, &delay_char, sizeof delay_char, 0,(struct sockaddr *)&their_addr3, &addr_len3)==-1){
            perror("recvfrom");
            exit(1);
        };
//string tt_str(tt_char);
        //cout<<tt_char<<endl<<tp_char<<endl<<delay_char<<endl;
        tt=stod(tt_char);
        string tp_str(tp_char);
        string delay_str(delay_char);
        vector<string>tp_tmp,delay_tmp;
        tp_tmp=split(tp_str," ");
        delay_tmp=split(delay_str," ");
        for (int i =0; i< tp_tmp.size();++i){
            tp[i]=stod(tp_tmp[i]);
        }
        for(int i =0; i<delay_tmp.size();++i){
            delay[i]=stod(delay_tmp[i]);
        }



        cout<<"The AWS has received delays from server B:"<<endl;
        cout<<"--------------------------------------------------"<<endl;
        cout<<"Destination        Tt        Tp        Delay"<<endl;
        cout<<"--------------------------------------------------"<<endl;
        cout<<setiosflags(ios::fixed)<<setprecision(2);
        for(int i=0;i<tp_tmp.size();++i){
            if (path[i]!=0&&path[i]!=INF){
                cout<<dics[i]<<"           "<<tt<<"          "<<tp[i]<<"         "<<delay[i]<<endl;

            }
        }
        cout<<"--------------------------------------------------"<<endl;
        close(sockrev);



        //////////////////////////////////Send to Client////////////////////////////////////////////////

        if (send(new_fd, &dics_char, sizeof dics_char, 0) == -1)
            perror("send");


        if (send(new_fd, &path_char, sizeof path_char, 0) == -1)
            perror("send");

        if (send(new_fd, &tt_char, sizeof tt_char, 0) == -1)
            perror("send");

        if (send(new_fd, &tp_char, sizeof tp_char, 0) == -1)
            perror("send");

        if (send(new_fd, &delay_char, sizeof delay_char, 0) == -1)
            perror("send");

        cout<<"The AWS has sent calculated delay to client using client TCP over port 24175."<<endl;





        //freeaddrinfo(servinfo);

        //printf("talker: sent %d bytes to %s\n", numbytes, buf);
        //close(sockfd_UDP);

        //buf[numbytes]='\0';


        /*if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }*/
        //close(new_fd);  // parent doesn't need this
    }



}
