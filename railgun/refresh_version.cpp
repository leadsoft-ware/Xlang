#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
using namespace std;

struct verinfo{
    enum{_stable,_insider} branch;
    int time; // 202106
    int major,minjor;
    int build;
};

verinfo open_verinfo_file(char* path){
    int fd = open(path,O_RDWR|O_CREAT,777);
    verinfo ret;
    read(fd,&ret,sizeof(verinfo));
    close(fd);
    return ret;
}

void update_verinfo_time(verinfo& f){
    f.branch = verinfo::_insider;
    f.time = time(0);
}

char* generateConfigFile(verinfo v){
    char* ret = (char*)malloc(1024);
    snprintf(
        ret,
        1024,
        "#define __xlang_%s\n"
        "#define __xlang_version \"%d.%db%d\"\n"
        "#define __xlang_build %d\n",
        (v.branch ? "insider" : "stable"),
        v.major,v.minjor,v.build,
        v.time
    );
    return ret;
}

void refreshConfigFile(char* f,verinfo ver){
    int fd = open("include/config.hpp",O_RDWR|O_CREAT,777);
    write(fd,f,strlen(f));
    close(fd);
    int fd1 = open("build/version",O_RDWR|O_CREAT,777);
    write(fd1,&ver,sizeof(ver));
    close(fd1);
}

void init(verinfo &ver){
    ver.branch = verinfo::_insider;
    ver.time = time(0);
    ver.build = 0;
    ver.major = 0;
    ver.minjor = 0;
}

int main(int argc,char** argv){
    int input;
    verinfo ver = open_verinfo_file("build/version");
    //init(ver);
    update_verinfo_time(ver);
    if(argc > 1 && std::string(argv[1]) == "fast"){ input=0; }
    else {
        std::cout << "Input update mode(1/0):";
        std::cin >> input;
    }
    if(!input){
        ver.build++;
        refreshConfigFile(generateConfigFile(ver),ver);
        return 0;
    }
    std::cout << "Would you like to update major(1/0):";
    std::cin >> input;
    if(input){
        ver.major++;
        ver.minjor = 0;
        ver.build = 0;
    }
    std::cout << "Would you like to update minjor(1/0):";
    std::cin >> input;
    if(input){
        ver.minjor++;
    }
    ver.build++;
    refreshConfigFile(generateConfigFile(ver),ver);
}