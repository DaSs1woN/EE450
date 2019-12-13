#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <limits.h>
#include <iomanip>
#include <map>
#include <vector>



#define PORT "24175" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

#define LOCALHOST "127.0.0.1"
int INF=INT_MAX;

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


int main(int argc, char *argv[])
{
    cout<<"The client is up and running "<<endl;

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    //if (argc != 4) {
        //fprintf(stderr,"usage: function  \n");
        //exit(1);
    //}
    char map_name[MAXDATASIZE];
    char vertex_num[MAXDATASIZE];
    char file_size[MAXDATASIZE];
    strcpy(map_name,argv[1]);
    strcpy(vertex_num,argv[2]);
    strcpy(file_size,argv[3]);


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(LOCALHOST, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    //printf("client: connecting to %s\n", s);

    //freeaddrinfo(servinfo); // all done with this structure

    if (send(sockfd,&map_name,sizeof map_name,0)==-1)
        perror("send");

    if (send(sockfd,&vertex_num, sizeof vertex_num,0)==-1)
        perror("send");

    if (send(sockfd,&file_size,sizeof file_size,0)==-1)
        perror("send");

    cout<<"The client has sent query to AWS using TCP over port 24175: start vertex "<<vertex_num<<"; map"<<map_name<<"; file size "<<file_size<<"."<<endl;

/////////////////////////////////////////recv from AWS/////////////////////////////////////////////////////////////////////

    char dics_char[MAXDATASIZE],path_char[MAXDATASIZE],tt_char[MAXDATASIZE],tp_char[MAXDATASIZE],delay_char[MAXDATASIZE];
    double tt;
    int path[12];
    double tp[12],delay[12];
    int dics[12];

    if((recv(sockfd,&dics_char,sizeof dics_char,0))==-1){
        perror("recv");
        exit(1);
    }
    if((recv(sockfd,&path_char,sizeof path_char,0))==-1){
        perror("recv");
        exit(1);
    }
    if((recv(sockfd,&tt_char,sizeof tt_char,0))==-1){
        perror("recv");
        exit(1);
    }
    if((recv(sockfd,&tp_char,sizeof tp_char,0))==-1){
        perror("recv");
        exit(1);
    }
    if((recv(sockfd,&delay_char,sizeof delay_char,0))==-1){
        perror("recv");
        exit(1);
    }

    tt=stod(tt_char);
    string dics_str(dics_char);
    string path_str(path_char);
    string tp_str(tp_char);
    string delay_str(delay_char);

    vector<string>dics_tmp,path_tmp,tp_tmp,delay_tmp;
    dics_tmp=split(dics_str," ");
    path_tmp=split(path_str," ");
    tp_tmp=split(tp_str," ");
    delay_tmp=split(delay_str," ");

    for (int i=0;i<dics_tmp.size();++i){
        dics[i]=stoi(dics_tmp[i]);
    }
    for (int i=0;i<path_tmp.size();++i){
        path[i]=stoi(path_tmp[i]);
    }
    for (int i=0;i<tp_tmp.size();++i){
        tp[i]=stod(tp_tmp[i]);
    }
    for (int i=0;i<delay_tmp.size();++i){
        delay[i]=stod(delay_tmp[i]);
    }





    cout<<"The client has received results from AWS:"<<endl;
    cout<<"-------------------------------------------------------------------------"<<endl;
    cout<<"Destination        Min Length        Tt        Tp        Delay"<<endl;
    cout<<"-------------------------------------------------------------------------"<<endl;
    cout<<setiosflags(ios::fixed)<<setprecision(2);
    for(int i=0;i<dics_tmp.size();++i){
        if (path[i]!=0&&path[i]!=INF){
            cout<<dics[i]<<"             "<<path[i]<<"          "<<tt<<"          "<<tp[i]<<"           "<<delay[i]<<endl;
        }
    }
    cout<<"-------------------------------------------------------------------------"<<endl;

    close(sockfd);
    return 0;


}
