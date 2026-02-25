#pragma once 

#include "Request.h"

class Server{
    public: 
        Server(int id, JobType type);
        bool isIdle();
        JobType type(); 
        bool assign(const Request& request);
        int id(); 


    private:
        int id; 
        JobType type; 
        bool idle;
        Request current_request; 
};