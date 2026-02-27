#pragma once 

#include "Request.h"

class Server{
    public: 
        Server(JobType type);
        bool isIdle();
        JobType getType(); 
        bool assign(const Request& request);
        bool decrementRequest();


    private:
        JobType type; 
        bool idle;
        Request current_request; 
};