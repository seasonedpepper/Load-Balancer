#pragma once

#include <string>

enum class JobType {
    Management = 2,
    Processing = 1,
    Streaming = 0


};

namespace JobUtil{
    JobType intToJob(int i);
    std::string jobToString(JobType type);
    int jobToInt(JobType type);
}

struct Request {
    std::string inIP;
    std::string outIP;
    int time;
    JobType type;
    Request();
    Request(std::string inIP, std::string outIP, int time, JobType type);
};