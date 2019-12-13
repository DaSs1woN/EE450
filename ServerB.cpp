#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>
#include <vector>
#include <iomanip>
#include <map>

#define PORT_B "22175"
#define PORT_AWS_UDP "23175"
#define LOCALHOST "127.0.0.1"
int INF=INT_MAX;
int MAXBUFF=100;

using namespace std;

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


int main(void) {
    int sockfd,socksend;
    struct addrinfo hints, *servinfo, *servinfo1, *p, *p1;
    int rv,rv1;
    struct sockaddr_storage their_addr,their_addr1;
    socklen_t addr_len,addr_len1;
    //char s[INET6_ADDRSTRLEN];
    cout<<"The Server B is up and running using UDP on port 22175"<<endl;


    while(1){
        char pro[MAXBUFF];
        char trans[MAXBUFF];
        char file_size[MAXBUFF];

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

        if ((rv = getaddrinfo(NULL, PORT_B, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }

        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("serverB: socket\n");
                continue;
            }

            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("serverB: bind\n");
                continue;
            }
            break;
        }
        if (p == NULL) {
            fprintf(stderr, "serverB: failed to bind socket\n");
            return 2;
        }
        freeaddrinfo(servinfo);	// all done with this structure
        addr_len = sizeof their_addr;
        char dics_char[MAXBUFF];
        char path_char[MAXBUFF];
        int dics[12];
        int path[12];
        double propagation,transmission;

        if ((recvfrom(sockfd, &pro, sizeof pro, 0, (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        if ((recvfrom(sockfd, &trans, sizeof trans, 0, (struct sockaddr *) &their_addr, &addr_len)) ==-1) {
                perror("recvfrom");
                exit(1);
            }

        if ((recvfrom(sockfd, &dics_char, sizeof dics_char, 0, (struct sockaddr *) &their_addr, &addr_len)) ==
            -1) {
            perror("recvfrom");
            exit(1);
        }


        if ((recvfrom(sockfd, &path_char, sizeof path_char, 0, (struct sockaddr *) &their_addr, &addr_len)) ==
            -1) {
            perror("recvfrom");
            exit(1);
        }
        if ((recvfrom(sockfd, &file_size, sizeof file_size, 0, (struct sockaddr *) &their_addr, &addr_len)) ==
            -1) {
            perror("recvfrom");
            exit(1);
        }

        string dics_str(dics_char);
        string path_str(path_char);

        vector<string>dics_tmp,path_tmp;
        dics_tmp=split(dics_str," ");
        path_tmp=split(path_str," ");
        for(int i=0; i<dics_tmp.size();++i){
            dics[i]=stoi(dics_tmp[i]);
        }
        for(int i=0;i<path_tmp.size();++i){
            path[i]=stoi(path_tmp[i]);
        }
        propagation=stod(pro);
        transmission=stod(trans);

        double file=stod(file_size);

        cout << "The Server has received data for calculation:" << endl;
        cout << "* Propagation speed: " << propagation << " km/s;" << endl;
        cout << "*Transmission speed: " << transmission << " Bytes/s" << endl;
        for(int i=0;i<dics_tmp.size();++i){
            if (path[i]!=0&&path[i]!=INF){
                cout<<"Path length for destination "<<dics[i]<<": "<<path[i]<<";"<<endl;
            }

        }
        close(sockfd);

        double tp[12];
        double delay[12];
        double tt=file/(transmission*8.0);
        cout<<setiosflags(ios::fixed)<<setprecision(2);
        cout<<"The Server B has finished the calculation of the delays:"<<endl;
        cout<<"---------------------------------------------------------------:"<<endl;
        cout<<"Destination                Delay"<<endl;
        cout<<"---------------------------------------------------------------:"<<endl;

        for(int i=0;i<dics_tmp.size();++i){
            if (path[i]!=0&&path[i]!=INF){
                tp[i]=path[i]/propagation;
                delay[i]=tt+tp[i];
                cout<<dics[i]<<"                      "<<delay[i]<<endl;
            }
        }

        char tp_char[MAXBUFF];
        char tmp[MAXBUFF];
        int flag=0;
        for(int i=0;i<dics_tmp.size();++i) {
                sprintf(tmp, "%f", tp[i]);
                //cout<<tmp1<<endl;
                if (flag==0) {
                    strcpy(tp_char, tmp);
                    //cout<<tp_char<<endl;
                    flag++;
                }
                else {
                    strcat(tp_char, " ");
                    strcat(tp_char, tmp);
                }

        }

        char delay_char[MAXBUFF];
        char tmp1[MAXBUFF];
        flag=0;
        for(int i=0;i<dics_tmp.size();++i) {
            sprintf(tmp1, "%f", delay[i]);
            //cout<<tmp1<<endl;
            if (flag==0) {
                strcpy(delay_char, tmp1);
                //cout<<delay_char<<endl;
                flag++;
            }
            else {
                strcat(delay_char, " ");
                strcat(delay_char, tmp1);
            }

        }
        char tt_char[MAXBUFF];
        sprintf(tt_char,"%f",tt);
        //cout<<tt_char<<endl;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        if ((rv1 = getaddrinfo(LOCALHOST, PORT_AWS_UDP, &hints, &servinfo1)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv1));
            return 1;
        }

        for (p1 = servinfo1; p1 != NULL; p1 = p1->ai_next) {
            if ((socksend = socket(p1->ai_family, p1->ai_socktype, p1->ai_protocol)) == -1) {
                perror("serverB: socket\n");
                continue;
            }

            break;
        }
        if (p1 == NULL) {
            fprintf(stderr, "serverB: failed to bind socket\n");
            return 2;
        }
        // all done with this structure

        if ((sendto(socksend, &tt_char, sizeof tt_char, 0,
                    p1->ai_addr, p1->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        if ((sendto(socksend, &tp_char, sizeof tp_char, 0,
                    p1->ai_addr, p1->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        if ((sendto(socksend, &delay_char, sizeof delay_char, 0,
                    p1->ai_addr, p1->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        freeaddrinfo(servinfo1);
        close(socksend);
        cout<<"The Server B has finished sending the output to AWS"<<endl;












        }


        //printf("listener: packet is %d bytes long\n", numbytes);

}