#pragma once

#include <string>

enum JobType {
    Management,
    Processing,
    Streaming
};

struct Request {
    std::string inIP;
    std::string outIP;
    int time;
    JobType type;
    int id;
};