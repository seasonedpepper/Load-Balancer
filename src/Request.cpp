#include "Request.h"

Request::Request() : inIP(""), outIP(""), time(0), type(JobType::Management){}

Request::Request(std::string inIP, std::string outIP, int time, JobType type) : inIP(inIP), outIP(outIP), time(time), type(type){}

JobType JobUtil::intToJob(int i){
    return static_cast<JobType>(i);
};

std::string JobUtil::jobToString(JobType type){
    if(type == JobType::Management){
        return "Management";
    }
    if(type == JobType::Processing){
        return "Processing";
    }
    if(type == JobType::Streaming){
        return "Streaming";
    }
    return "";
};

int JobUtil::jobToInt(JobType type){
    if(type == JobType::Management){
        return 2;
    }
    if(type == JobType::Processing){
        return 1;
    }
    if(type == JobType::Streaming){
        return 0;
    }
    return -1;
};
