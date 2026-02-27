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
    this->current_request.inIP = request.inIP;
    this->current_request.outIP = request.outIP;
    this->current_request.time = request.time;
    this->current_request.type = request.type; 
    return true; 
}

bool Server::decrementRequest(){
    --(this->current_request.time);
    if (current_request.time == 0){
        idle = true; 
        return true; 
    }
    return false; 
}