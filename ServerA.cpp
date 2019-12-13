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
#include <vector>
#include <fstream>
#include <ctype.h>
#include <math.h>
#include <algorithm>
#include <limits.h>
#include <set>
#include <map>
#include <stack>


#define PORT_A "21175"
#define PORT_AWS_UDP "23175"
#define LOCALHOST "127.0.0.1"
#define MAXBUFLEN 100
const int INF = INT_MAX;


using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool IsSpace(string line){
    for (int i=0;i<line.size();++i)
        if (line[i]==' '){
            return true;
        }
    return false;
}

void Split(string line,int &vertex1,int &vertex2, int &edge){
    vector<string>result;
    line+=' ';
    string::size_type pos;
    for (int i=0;i<line.size();++i){
        pos=line.find(' ',i);
        if(pos<line.size())
        {
            string s=line.substr(i,pos-i);
            result.push_back(s);
            i=pos;
        }

    }
    vertex1=stoi(result[0]);
    vertex2=stoi(result[1]);
    edge=stoi(result[2]);
}
int max_3(int a, int b, int c)
{
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}


int vertex_max(ifstream &map,char map_id) {
    string buffer;
    int count=0;
    char id;
    int vertex1,vertex2,dis;
    int max_num=0;
    while (!map.eof()) {
        getline(map, buffer);
        if (buffer[buffer.size() - 1] == '\r') {
            buffer.pop_back();
        }

        if (isalpha(buffer[0])) {
            id=buffer[0];
        }
        if (id==map_id){
            if(IsSpace(buffer)){
                Split(buffer,vertex1,vertex2,dis);
                max_num=max_3(vertex1,vertex2,max_num);
            }
        }

    }
    return max_num+1;
}


void read_map(ifstream &maps, string &propagation, string &transmission, map <int, int>&dics, int (&edge)[12][12], char map_id) {
    string buffer;
    char tmp,id;
    int vertex1,vertex2,dis;
    map <int, int>::iterator dics_iter;
    int num=0;
    int v1,v2;
    //int mark;
    //memset(edge,0, sizeof(edge));
    //string pro,trans;
    //char map_name;
    while(!maps.eof()){
        getline(maps,buffer);
        if (buffer[buffer.size()-1]=='\r'){
            buffer.pop_back();
        }
        if (isalpha(buffer[0])){
            tmp=buffer[0];
            id=buffer[0];
        }
        if (id==map_id){
            if(IsSpace(buffer)){
                Split(buffer,vertex1,vertex2,dis);
                for(dics_iter=dics.begin();dics_iter!=dics.end();dics_iter++)
                {
                    if(dics_iter->second==vertex1){
                        v1=dics_iter->first;
                        break;
                    }
                }
                if(dics_iter==dics.end()){
                    dics.insert(pair<int,int>(num,vertex1));
                    v1=num;
                    num++;
                }
                for(dics_iter=dics.begin();dics_iter!=dics.end();dics_iter++)
                {
                    if(dics_iter->second==vertex2){
                        v2=dics_iter->first;
                        break;
                    }
                }
                if(dics_iter==dics.end()){
                    dics.insert(pair<int,int>(num,vertex2));
                    v2=num;
                    num++;
                }
                edge[v1][v2]=dis;
                edge[v2][v1]=dis;
            }

            else if(isalnum(buffer[0])&&isalpha(tmp)){
                propagation=buffer;
                tmp=buffer[0];
            }
            else if(isalnum(buffer[0])&&isalnum(tmp)){
                transmission=buffer;
                tmp=buffer[0];
            }
        }

    }
    for(int i=0;i<12;++i){
        for (int j=0;j<12;++j){
            if (edge[i][j]==0&&i!=j){
                edge[i][j]=INF;
            }
        }
    }

}


void show_map(ifstream &map){
    vector<int>edge_num;
    vector<int>vertex_num;
    vector<string>map_name;
    string buffer;
    char tmp;
    int sum_edge=0;
    int vertex1, vertex2,dis;
    set<int>set_tmp;
    while(!map.eof()){
        getline(map,buffer);
        if (buffer[buffer.size()-1]=='\r'){
            buffer.pop_back();
        }

        if (isalpha(buffer[0])){
            if (set_tmp.size()!=0&&sum_edge!=0){
                vertex_num.push_back(set_tmp.size());
                edge_num.push_back(sum_edge);
                set_tmp.clear();
            }
            map_name.push_back(buffer);
            sum_edge=0;
            tmp=buffer[0];

        }

       // else if(isalpha(buffer[0])&&mark>flag){
         //   break;
        //}

        else if(IsSpace(buffer)){
            Split(buffer,vertex1,vertex2,dis);
            set_tmp.insert(vertex1);
            set_tmp.insert(vertex2);
            sum_edge++;
        }
    }
    vertex_num.push_back(set_tmp.size());
    edge_num.push_back(sum_edge);

    cout<<"The Server A has constructed a list of "<<map_name.size()<<" maps :"<<endl;
    cout<<"----------------------------------------------------------"<<endl;
    cout<<"Map ID   Num Vertices   Num Edges"<<endl;
    cout<<"----------------------------------------"<<endl;
    for (int i = 0; i < map_name.size(); ++i) {
        cout<<map_name[i]<<"            "<<vertex_num[i]<<"             "<<edge_num[i]<<endl;
    }
    cout<<"-----------------------------------------------------------"<<endl;


}

stack<int>index_sort(map<int,int>vertex){
    stack<int>index;
    map<int,int>::iterator i,j;
    int flag=0;
    for(i=vertex.begin();i!=vertex.end();i++){
        int max_num=-1;
        for(j=vertex.begin();j!=vertex.end();j++){
            if (index.empty()){
                if (max_num<j->second){
                    max_num=j->second;
                    flag=j->first;
                }
            }
            else{
                if (max_num<j->second&&j->second<vertex[index.top()]){
                    max_num=j->second;
                    flag=j->first;
                }
            }
        }
        index.push(flag);
    }
    return index;
}



void Dijkstra(int (&edge)[12][12],int vertex,int (&dis)[12]){
    //int dis[vertex_num];
    int mark[12];
    int dis_min, tmp;

    for(int i=0;i<12;++i){
        dis[i]=edge[vertex][i];
    }

    for(int i=0;i<12;++i){
        mark[i]=0;
    }
    mark[vertex]=1;

    for(int i=0;i<12;++i){
        dis_min=INF;
        for(int j=0;j<12;++j){
            if (mark[j]==0&&dis[j]<dis_min){
                dis_min=dis[j];
                tmp=j;
            }

        }
        mark[tmp]=1;
        for(int v=0;v<12;++v){
            if (edge[tmp][v]<INF){
                if(dis[v]>dis[tmp]+edge[tmp][v]){
                    dis[v]=dis[tmp]+edge[tmp][v];
                }
            }
        }
    }


}


int main(void) {
    int sockfd,socksend;
    struct addrinfo hints, *servinfo,*servinfo1, *p, *p1;
    int rv,rv1;
    //int numbytes;
    struct sockaddr_storage their_addr;

    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    cout<<"The Server A is up and running using UDP on port 21175."<<endl;
    ifstream map_tmp;
    map_tmp.open("map.txt",ios::in);
    show_map(map_tmp);
    map_tmp.close();


    while (1) {
        char map_name;
        int vertex;
        string trans,pro;
        char propagation[MAXBUFLEN],transmission[MAXBUFLEN];
        int vertex_num=0;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

        if ((rv = getaddrinfo(NULL, PORT_A, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }

        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("serverA: socket\n");
                continue;
            }

            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("listener: bind");
                continue;
            }
            break;
        }
        if (p == NULL) {
            fprintf(stderr, "serverA: failed to bind socket\n");
            return 2;
        }
        freeaddrinfo(servinfo);
        addr_len =  sizeof their_addr;
        map<int,int>dics;
        char vertex_str[MAXBUFLEN];

        recvfrom(sockfd, &map_name, sizeof map_name, 0, (struct sockaddr *)&their_addr, &addr_len);
        recvfrom(sockfd, &vertex_str, sizeof vertex_str, 0, (struct sockaddr *)&their_addr, &addr_len);
        vertex=stoi(vertex_str);
        //recvfrom(sockfd, (char *)&file_size, sizeof file_size, 0, (struct sockaddr *)&their_addr, &addr_len);
        ifstream map_file;
        //cout<<vertex<<endl;
        map_file.open("map.txt",ios::in);
        //maxvertex=vertex_max(map_file,map_name);
        map_file.close();
        int edge[12][12];
        memset(edge,0,sizeof edge);
        ifstream map_dup;
        map_dup.open("map.txt",ios::in);
        //cout<<maxvertex<<endl;
        cout<<"The Server A has received input for finding shortest paths: starting vertex "<<vertex<<" of map "<<map_name<<"."<<endl;

        read_map(map_dup,pro,trans,dics,edge,map_name);
        strncpy(propagation, pro.c_str(), pro.length() + 1);
        strncpy(transmission, trans.c_str(), trans.length() + 1);
        //cout<<num<<endl;
        map<int,int>::iterator dics_iter;
        int vertex_trans;
        for(dics_iter=dics.begin();dics_iter!=dics.end();dics_iter++)
        {
            if (dics_iter->second==vertex){
                vertex_trans=dics_iter->first;
            }
        }


        int path[12];
        memset(path,0,sizeof path);
        Dijkstra(edge,vertex_trans,path);
        stack<int>index_tmp=index_sort(dics);
        int vertex_dic[12];
        int flag_tmp;
        int path_trans[12];
        memset(path_trans,0, sizeof path_trans);
        for(int i=0;i<dics.size();++i)
        {
            flag_tmp=index_tmp.top();
            vertex_dic[i]=dics[flag_tmp];
            path_trans[i]=path[flag_tmp];
            index_tmp.pop();
        }


        cout<<"The Server A has identified the following shortest paths:"<<endl;
        cout<<"----------------------------------------------------------"<<endl;
        cout<<"Destination"<<"          "<<"Min Length"<<endl;
        cout<<"----------------------------------------------------------"<<endl;
        for(int i=0;i<dics.size();++i){
            if (path_trans[i]!=0&&path_trans[i]!=INF){
                cout<<vertex_dic[i]<<"                     "<<path_trans[i]<<endl;
            }
        }




        close(sockfd);

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        if ((rv1 = getaddrinfo(LOCALHOST, PORT_AWS_UDP, &hints, &servinfo1)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv1));
            return 1;
        }

        for (p1 = servinfo1; p1 != NULL; p1 = p1->ai_next) {
            if ((socksend = socket(p1->ai_family, p1->ai_socktype, p1->ai_protocol)) == -1) {
                perror("serverA: socket\n");
                continue;
            }
            break;
        }
        if (p1 == NULL) {
            fprintf(stderr, "serverA: failed to bind socket\n");
            return 2;
        }

        //cout<<dics[1]<<endl<<pro<<endl<<trans<<endl;

        char tmp[MAXBUFLEN];
        char vertex_dic_str[MAXBUFLEN];
        int flag=0;
        for (int i = 0; i < dics.size(); ++i) {
            if (path_trans[i] != 0 && path_trans[i] != INF) {
                sprintf(tmp, "%d", vertex_dic[i]);
                //cout<<tmp<<endl;
                if (flag==0) {
                    strcpy(vertex_dic_str, tmp);
                    //cout<<vertex_dic_str<<endl;
                    flag++;
                }
                else {
                    strcat(vertex_dic_str, " ");
                    strcat(vertex_dic_str, tmp);
                }
            }
        }
        char path_char[MAXBUFLEN];
        char tmp1[MAXBUFLEN];
        flag=0;
        for(int i=0;i<dics.size();++i) {
            if (path_trans[i] != 0 && path_trans[i] != INF) {
                sprintf(tmp1, "%d", path_trans[i]);
                //cout<<tmp1<<endl;
                if (flag==0) {
                    strcpy(path_char, tmp1);
                    //cout<<path_char<<endl;
                    flag++;
                }
                else {
                    strcat(path_char, " ");
                    strcat(path_char, tmp1);
                }
            }
        }
        //cout<<vertex_dic_str[2]<<endl<<path_str[2]<<endl;

        if (sendto(socksend, &vertex_dic_str, sizeof vertex_dic_str, 0,
               p1->ai_addr, p1->ai_addrlen)==-1){
            perror("send");
        };

        sendto(socksend, &path_char, sizeof path_char, 0,
                p1->ai_addr, p1->ai_addrlen);

        sendto(socksend, &propagation, sizeof propagation, 0,
                p1->ai_addr, p1->ai_addrlen);
        sendto(socksend, &transmission, sizeof transmission, 0,
                    p1->ai_addr, p1->ai_addrlen);

        //cout<<vertex_dic_str[0]<<endl<<path_char[0]<<endl;

        cout<<"The Server A has sent shortest paths to AWS."<<endl;
        freeaddrinfo(servinfo1);
        close(socksend);




        //cout<<pro<<endl<<trans<<endl;
        //cout<<edge[1][2]<<endl;
        //cout<<map_name<<endl<<vertex<<endl<<file_size<<endl;
        //close(sockfd);
    }


    





}