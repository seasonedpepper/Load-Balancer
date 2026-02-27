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
#include<sstream>;


struct LBConfig {
    //queue threshold 
    int lowThreshold  = 50;
    int highThreshold = 80;
    int startingServers = 65; 

    //request generator 
    int rest = 10;
    std::set<std::string> genRange = {"107.115.5.74", "107.115.5.75"};
    double requestGenProb = 0.30;
    int minRequestTime = 1;
    int maxRequestTime = 10;

    // Firewall
    std::set<std::string> range = {"107.115.5.74"};
    int maxPing = 20;
    int pingWindow = 50;
    int blockDuration = 100;

    //logging
    std::string logFile = "events.log";

    //time 
    int clockPeriod = 1; 
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
        LoadBalancer(int id, JobType type);
        bool sendRequestLB(Request req);
        bool recieveRequest(Request& req);
        int tick();
        void setClock(int time); 
        int addServer(Server& ser); 
        int deleteServer(int id);

        LBStats getStats();
        int queueSize();
        int serverCount();
        int getNumIdle(); 

        void logEvent(std::string msg); 

        bool loadConfig();

        bool firewall(Request& req);

        Request generateRequest();


    private:
        int id; 
        JobType type; 
        std::queue<Request> q;
        std::vector<Server> servers; 
        LBConfig config; 
        LBStats stats;
        int time; 
        int numIdle; 

        std::map<std::string, int> banned; //ip : when first blocked
        std::map<std::string, std::pair<int, int> > seen; //ip : (count, first seen)
        std::ofstream logFile; 
        std::ifstream configFile; 




};