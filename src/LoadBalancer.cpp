#include "LoadBalancer.h"



void trim(std::string& s) {
    auto notSpace = [](unsigned char c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
}

std::set<std::string> parseIpListS(std::string& str) {
    std::set<std::string> out;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        trim(item);
        if (!item.empty()) out.insert(item);
    }
    return out;
};
std::vector<std::string> parseIpListV(std::string& str) {
    std::vector<std::string> out;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        trim(item);
        if (!item.empty()) out.push_back(item);
    }
    return out;
};

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

        else if (key == "rest") config.rest = std::stoi(val);
        else if (key == "requestGenProb") config.requestGenProb = std::stoi(val);
        else if (key == "minRequestTime") config.minRequestTime = std::stoi(val);
        else if (key == "maxRequestTime") config.maxRequestTime = std::stoi(val);
        else if (key == "genRange") config.genRange = parseIpListV(val);

        else if (key == "range") config.range = parseIpListS(val);
        else if (key == "maxPing") config.maxPing = std::stoi(val);
        else if (key == "pingWindow") config.pingWindow = std::stoi(val);
        else if (key == "blockDuration") config.blockDuration = std::stoi(val);


        else if (key == "logFile") config.logFile = val;

        else if (key == "clockPeriod") config.clockPeriod = std::stoi(val);

    }
    configFile.close(); 
    return true; 
}

LoadBalancer::LoadBalancer(JobType type, int numServers): type(type), time(0), q(), servers(), config(), stats(), banned(), seen(){
    loadConfig(); 
    for(int i = 0; i < numServers; ++i){
        Server sev(type);
        servers.push_back(sev);
    }
    logFile.open(config.logFile, std::ios::app);
    if (!logFile.is_open()){
        logEvent("Failed to open logs"); 
    }
}

bool LoadBalancer::sendRequestLB(LoadBalancer& lb){
    if (q.size() == 0){
        logEvent("Queue Empty, send failed");
        return false; 
    }
    Request req = q.front();
    q.pop();
    lb.q.push(req);
    logEvent("Request sent to " + JobUtil::jobToString(lb.getType())); 
    return true; 
}

void LoadBalancer::initQueue(LoadBalancer& LB){
    std::vector<std::string> vRange; 
    for (std::string str:config.range){
        vRange.push_back(str);
    } 
    for(int i = 0; i < 100 * LB.serverCount(); ++i){
        Request req = generateRequest(100, true, vRange);
        req.type = LB.type;
        LB.q.push(req);
    }
}

bool LoadBalancer::sendRequest(){
    //false if no servers avaliable 
    bool flag = false; 
    for(int i = 0; i < servers.size(); ++i){
        if(servers[i].isIdle()){
            Request req = q.front(); 
            q.pop();
            servers[i].assign(req);
            std::string msg = "Request (";
            msg += std::to_string(req.time);
            msg += ") assigned to ";
            msg +=JobUtil::jobToString(type);
            msg += " server";
            logEvent(msg);
            flag = true;
            break;
        }
    }
    if(flag)return true; 
    return false; 
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

int LoadBalancer::tick(std::vector<LoadBalancer*> LBs){
    ++(this->time); 
    for (int lb = 0; lb < LBs.size(); ++lb){
        ++(LBs[lb]->time); 
        for(int i = 0; i < LBs[lb]->servers.size() ; ++i){
            if (!LBs[lb]->servers[i].isIdle()){
                if(LBs[lb]->servers[i].decrementRequest()){
                    LBs[lb]->logEvent(JobUtil::jobToString(LBs[lb]->type) + " request completed");
                    ++LBs[lb]->stats.completed;
                }
            }
        }
        while (LBs[lb]->queueSize() > 0) {
            if(!LBs[lb]->sendRequest())break; 
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(config.clockPeriod));
    return time; 
}

void LoadBalancer::setClock(int time){
    this->time = time; 
}

int LoadBalancer::getTime(){
    return time; 
}

int LoadBalancer::addServer(const Server& ser){
    servers.push_back(ser);
    ++stats.scaleUps;
    std::string msg = "Added ";
    msg += JobUtil::jobToString(type); 
    msg += " Server";
    logEvent(msg);
    return servers.size();
}

int LoadBalancer::deleteServer(){
    for(int i = servers.size() - 1; i >=0; --i){
        if(servers[i].isIdle()){
            servers.erase(servers.begin() + i);
            std::string msg = "Deleted ";
            msg += JobUtil::jobToString(type); 
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

LBConfig LoadBalancer::getConfig(){
    return config; 
}

LBStats LoadBalancer::getStats(){
    return stats; 
}

int LoadBalancer::queueSize(){
    return q.size(); 
}

JobType LoadBalancer::getType(){
    return type; 
}

void LoadBalancer::logEvent(std::string msg){
    std::string color = "\033[0m"; // default reset

    if (msg.find("Rejected") != std::string::npos)
        color = "\033[31m"; // red
    else if (msg.find("Accepted") != std::string::npos)
        color = "\033[32m"; // green
    else if (msg.find("Added") != std::string::npos)
        color = "\033[34m"; // blue
    else if (msg.find("Deleted") != std::string::npos)
        color = "\033[33m"; // yellow
    else if (msg.find("completed") != std::string::npos)
        color = "\033[36m"; // cyan
    else if (msg.find("assigned") != std::string::npos)
        color = "\e[0;35m"; // purple
    else if (msg.find("No request generated") != std::string::npos)
        color = "\033[90m"; // gray

    std::cout << color << msg << "\033[0m" << std::endl;
    logFile << time << " : "<< msg << std::endl; 
}

bool LoadBalancer::firewall(Request& req){
    std::cout<<std::endl; 
    if (!config.range.count(req.inIP)) {
        ++stats.rejected;
        logEvent(req.inIP + " Rejected");
        return false;
    }
    if (banned.count(req.inIP)){
        if(banned[req.inIP] + config.blockDuration <= time){
            banned.erase(req.inIP);
            seen[req.inIP].first = 0;
            seen[req.inIP].second = time; 
        }
        else{
            ++stats.rejected;
            logEvent(req.inIP + " Rejected");
            return false; 
        }
    }

    if (!seen.count(req.inIP)){
        seen.insert(std::make_pair(req.inIP, std::make_pair(0, time)));
    }
    else if(seen[req.inIP].second + config.pingWindow < time){
        seen[req.inIP].first = 0;
        seen[req.inIP].second = time; 
    }
    ++seen[req.inIP].first;
    
    if(seen[req.inIP].first > config.maxPing){
        banned.insert(std::make_pair(req.inIP, time));
        ++stats.rejected;
        logEvent(req.inIP + " Rejected");
        return false; 
    }
    ++stats.accepted;
    logEvent(req.inIP + " Accepted");
    return true; 

}


Request LoadBalancer::generateRequest(int prob, bool vflag, std::vector<std::string> valid = {}){
    bool flag= false; 
    if ((std::rand() % 100) < prob) {
        flag = true; 
    }
    if (flag){
        int range_size = config.genRange.size(); 
        int random_ip_index_1 = rand() % (range_size );
        int random_ip_index_2 = rand() % (range_size );
        int random_time = rand() % (config.maxRequestTime-config.minRequestTime + 1) + config.minRequestTime;
        JobType type = static_cast<JobType>(rand() % (2));
        Request req(config.genRange[random_ip_index_1], config.genRange[random_ip_index_2], random_time, type);
        if(vflag){
            random_ip_index_1 = rand() % (valid.size());
            req.inIP = valid[random_ip_index_1]; 
        }
        ++stats.generated;
    return req; 
    }
    Request null_req;
    return null_req;
}

void LoadBalancer::logStats(){
    logFile << "Accepted: " << stats.accepted << std::endl;
    logFile << "Completed: " <<stats.completed << std::endl;
    logFile << "Generated: " <<stats.generated << std::endl;
    logFile << "Rejected: " <<stats.rejected << std::endl;
    logFile << "Scale Downs: " <<stats.scaleDowns << std::endl;
    logFile << "Scale Ups: " <<stats.scaleUps << std::endl;
    logFile << "Accepted Range: ";
    for (std::string str:config.range){
        logFile << str << "| ";
    }
    logFile << std::endl; 
    logFile << std::endl; 
    std::cout << "Accepted: " << stats.accepted << std::endl;
    std::cout << "Completed: " << stats.completed << std::endl;
    std::cout << "Generated: " << stats.generated << std::endl;
    std::cout << "Rejected: " << stats.rejected << std::endl;
    std::cout << "Scale Downs: " << stats.scaleDowns << std::endl;
    std::cout << "Scale Ups: " << stats.scaleUps << std::endl;
    std::cout << "Accepted Range: ";
    for (std::string str:config.range){
        std::cout << str << "| ";
    }
}

void LoadBalancer::logQueue(int fqs, int fqp, int lqs, int lqp){
    logFile << "Starting streaming queue size: " << fqs<< std::endl;
    logFile << "Starting processing queue size: " << fqp << std::endl;
    logFile << "Ending streaming queue size: " << lqs << std::endl;
    logFile << "Ending processing queue size: " << lqp << std::endl;
    std::cout << "Starting streaming queue size: " << fqs<< std::endl;
    std::cout << "Starting processing queue size: " << fqp << std::endl;
    std::cout << "Ending streaming queue size: " << lqs << std::endl;
    std::cout << "Ending processing queue size: " << lqp << std::endl;
}
void LoadBalancer::syncStats(std::vector<LoadBalancer*> LBs){
    for (int i = 0; i < LBs.size(); ++i){
        this->stats.scaleDowns += LBs[i]->stats.scaleDowns;
        this->stats.scaleUps += LBs[i]->stats.scaleUps; 
        this->stats.completed += LBs[i]->stats.completed;
        this->stats.accepted += LBs[i]->stats.accepted;
    }
}

