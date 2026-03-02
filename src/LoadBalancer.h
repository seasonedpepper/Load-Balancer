#pragma once 

#include "Server.h"
#include "Request.h"
#include <queue>
#include <vector>
#include <map>
#include <string>
#include<fstream>
#include<iostream>
#include<set>
#include <utility>
#include <random>
#include<sstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <ctime>


struct LBConfig {
    //queue threshold 
    int lowThreshold;
    int highThreshold;

    //request generator 
    int rest = 10;
    std::vector<std::string> genRange;
    int requestGenProb ;
    int minRequestTime ;
    int maxRequestTime ;

    // Firewall
    std::set<std::string> range;
    int maxPing ;
    int pingWindow ;
    int blockDuration ;

    //logging
    std::string logFile ;
    std::string statsFile ;

    //time 
    int clockPeriod = 0; 
};

struct LBStats {
    int generated = 0;
    int accepted = 0;
    int rejected = 0;
    int completed = 0;
    int scaleUps = 0;
    int scaleDowns = 0;
};

class LoadBalancer{
    public:
        LoadBalancer(JobType type, int numServers);
        Request generateRequest(int prob, bool vFlag, std::vector<std::string> valid);
        bool loadConfig();

        bool sendRequestLB(LoadBalancer& lb);
        void initQueue(LoadBalancer& LB);

        bool sendRequest();
        bool recieveRequest(Request& req);
        int tick(std::vector<LoadBalancer*> LBs);
        void setClock(int time); 
        int getTime();
        int addServer(const Server& ser); 
        int deleteServer();
        LBConfig getConfig(); 

        LBStats getStats();
        int queueSize();
        int serverCount();
        JobType getType();

        void logEvent(std::string msg); 

        bool firewall(Request& req);

        void logStats();
        void logQueue(int fqs, int fqp, int lqs, int lqp);
        void syncStats(std::vector<LoadBalancer*> LBs);


    private:
        JobType type; 
        int time; 
        std::queue<Request> q;
        std::vector<Server> servers; 
        LBConfig config; 
        LBStats stats;

        std::map<std::string, int> banned; //ip : when first blocked
        std::map<std::string, std::pair<int, int> > seen; //ip : (count, first seen)
        std::ofstream logFile; 
        std::ifstream configFile; 
};