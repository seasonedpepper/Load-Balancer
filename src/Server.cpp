#include "Server.h"


Server::Server(JobType type) : type(type), idle(true), current_request() {
}

bool Server::isIdle(){
    return this->idle; 
}

JobType Server::getType(){
    return this->type;
}

bool Server::assign(const Request& request){
    if (this->type != request.type){
        return false; 
    }
    current_request.inIP = request.inIP;
    current_request.outIP = request.outIP;
    current_request.time = request.time;
    current_request.type = request.type; 
    idle = false; 
    return true; 
}

bool Server::decrementRequest(){
    if (idle) return false; 
    --(this->current_request.time);
    if (current_request.time == 0){
        idle = true; 
        return true; 
    }
    return false; 
}