#pragma once

#include <string>

enum JobType {
    Management = 2,
    Processing = 1,
    Streaming = 0
};

struct Request {
    std::string inIP;
    std::string outIP;
    int time;
    JobType type;
    Request::Request(){
        inIP = "";
        outIP = "";
        time = 0;
        type = JobType::Management; 
    }

    Request::Request(std::string inIP, std::string outIP, int time, JobType type) : inIP(inIP), outIP(outIP), time(time), type(type){}

    JobType intToJob(int i){
        return static_cast<JobType>(i);
    }

};