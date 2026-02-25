#pragma once 

#include "Server.h"
#include "Request.h"
#include <queue>
#include <vector>
#include <map>
#include <string>


struct LBConfig {
    //queue threshold 
    int lowThreshold  = 50;
    int highThreshold = 80;

    //request generator 
    int rest = 10;
    double requestGenProb = 0.30;
    int minRequestTime = 1;
    int maxRequestTime = 10;

    // Firewall
    std::vector<std::string> range = {"107.115.5.74"};
    int maxPing = 20;
    int pingWindow = 50;
    int blockDuration = 100;

    //logging
    std::string logFile = "events.log";
};

struct LBStats {
    uint64_t generated = 0;
    uint64_t accepted = 0;
    uint64_t rejected = 0;
    uint64_t routed = 0;
    uint64_t completed = 0;
    uint64_t scaleUps = 0;
    uint64_t scaleDowns = 0;
};

class LoadBalancer{
    public:
        LoadBalancer(int id, JobType type); 
        bool sendRequestLB(int id, Request req);
        bool recieveRequest(Request& req);
        void tick();
        void addServer(Server& ser); 
        void deleteServer(int id);

        LBStats stats();
        int queueSize();
        int serverCount();

    private:
        int id; 
        JobType type; 
        std::queue<Request> q;
        std::vector<Server> servers; 
        LBConfig config; 
        LBStats stats;
        std::map<std::string, int> blocked; //ip : time left before unblocked
        int time; 

        bool firewall(Request& req);
        Request generateRequest();


};