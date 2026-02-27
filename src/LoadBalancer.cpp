#include "LoadBalancer.h"

LoadBalancer::LoadBalancer(int id, JobType type) : id(id), type(type), q(), servers(), config(), stats(), banned(), seen(){
    for(int i = 0; i < config.startingServers; ++i){
        Server sev(type);
        servers.push_back(sev);
    }
    numIdle = config.startingServers;

    logFile.open("events.log, std::ios::app");
    if (!logFile.is_open()){
        logEvent("Failed to open logs"); 
    }
}

bool LoadBalancer::sendRequestLB(Request req){
    for(int i = 0; i < servers.size(); ++i){
        if(servers[i].isIdle()){
            servers[i].assign(req);
        }
    }
}

bool LoadBalancer::recieveRequest(Request& req){
    int pre_size = q.size(); 
    q.push(req);
    int post_size = q.size(); 
    if (pre_size + 1 != post_size){
        return false; 
    }
    return true; 
}

int LoadBalancer::tick(){
    ++(this->time); 
    for(int i = 0; i < servers.size() ; ++i){
        if (!servers[i].isIdle()){
            if(servers[i].decrementRequest()){
                logEvent(type + " request completed!");
                ++stats.completed;
                ++numIdle; 
            }
        }
    }
    for (auto& [key, val]: banned ){
        --val; 
    }
    return time; 
}

void LoadBalancer::setClock(int time){
    this->time = time; 
}

int LoadBalancer::addServer(Server& ser){
    servers.push_back(ser);
    ++numIdle; 
    ++stats.scaleUps;
    std::string msg = "Added ";
    msg += type; 
    msg += " Server";
    logEvent(msg);
    return servers.size();
}

int LoadBalancer::deleteServer(int id){
    for(int i = servers.size() - 1; i >=0; --i){
        if(servers[i].isIdle()){
            servers.erase(servers.begin() + i);
            --numIdle; 
            std::string msg = "Deleted ";
            msg += type; 
            msg += " Server";
            logEvent(msg);
            ++stats.scaleDowns; 
            break;
        }
    }
    return servers.size(); 
}

int LoadBalancer::serverCount(){
    return servers.size(); 
}

int LoadBalancer::getNumIdle(){
    return numIdle; 
}

LBStats LoadBalancer::getStats(){
    return stats; 
}

int LoadBalancer::queueSize(){
    return q.size(); 
}

void LoadBalancer::logEvent(std::string msg){
    std::cout << time << " : " <<msg << std::endl; 
    logFile << time << " : "<< msg << std::endl; 
}

bool LoadBalancer::firewall(Request& req){
    if(config.range.count(req.inIP)){
        if(!banned.count(req.inIP)){
            if (!seen.count(req.inIP)){
                seen.insert(std::make_pair(req.inIP, std::make_pair(1, time)));
            }
            else{
                ++seen[req.inIP].first;
            }
            if(seen[req.inIP].second + config.pingWindow < time){
                seen[req.inIP].first = 1;
                seen[req.inIP].second = time; 
            }

            if(seen[req.inIP].first >= config.maxPing){
                banned.insert(std::make_pair(req.inIP, time));
            }
        }
        if (banned.count(req.inIP)){
            if(banned[req.inIP] + config.blockDuration < time){
                banned.erase(req.inIP);
            }
            else{
                ++stats.rejected;
                logEvent(req.inIP + " Rejected");
                return false; 
            }
        }
        ++stats.accepted;
        logEvent(req.inIP + " Accepted");
        return true; 
    }
    ++stats.rejected;
    logEvent(req.inIP + " Rejected");
    return false; 
}


std::set<std::string> parseIpList(std::string& str) {
    std::set<std::string> out;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        if (!item.empty()) out.insert(item);
    }
    return out;
};

void trim(std::string& s) {
    auto notSpace = [](unsigned char c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
}

bool LoadBalancer::loadConfig(){
    configFile.open("config.txt", std::ios::in);
    if (!configFile.is_open()){
        logEvent("Failed to open config file");
        return false; 
    }
    std::string line; 
    while(std::getline(configFile, line)){
        trim(line);
        if (line.empty()) continue; 
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        trim(key);
        trim(val);

        if (key == "lowThreshold") config.lowThreshold = std::stoi(val);
        else if (key == "highThreshold") config.highThreshold = std::stoi(val);
        else if (key == "startingServers") config.startingServers = std::stoi(val); 

        else if (key == "rest") config.rest = std::stoi(val);
        else if (key == "requestGenProb") config.requestGenProb = std::stod(val);
        else if (key == "minRequestTime") config.minRequestTime = std::stoi(val);
        else if (key == "maxRequestTime") config.maxRequestTime = std::stoi(val);
        else if (key == "genRange") config.genRange = parseIpList(val);

        else if (key == "range") config.range = parseIpList(val);
        else if (key == "maxPing") config.maxPing = std::stoi(val);
        else if (key == "pingWindow") config.pingWindow = std::stoi(val);
        else if (key == "blockDuration") config.blockDuration = std::stoi(val);


        else if (key == "logFile") config.logFile = val;

        else if (key == "clockPeriod") config.clockPeriod = std::stoi(val);

    }
}