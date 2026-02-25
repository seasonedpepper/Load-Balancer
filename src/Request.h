#pragma once

#include <string>

enum JobType{
    Managemnet, 
    Proessing,
    Streaming
};

struct Request{
    std::string inIP;
    std::string outIP; 
    int time; 
    JobType type;
    int id; 
};