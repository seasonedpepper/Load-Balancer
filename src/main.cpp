#include "LoadBalancer.h"
#include "Request.h"
#include "Server.h"
#include <climits>
int main(){
    int maxTime, numServers;
    std:: cout << "Enter number of cycles and number of starting servers" << std::endl; 
    std::cin >> maxTime >> numServers; 
    LoadBalancer managementLB(JobType::Management, 0);
    LoadBalancer streamingLB(JobType::Streaming, numServers/2);
    LoadBalancer processingLB(JobType::Processing, numServers/2);
    managementLB.initQueue(streamingLB);
    managementLB.initQueue(processingLB);
    int startingStreamingQ = 0;
    int startingProcessingQ = 0; 
    int endingStreamingQ = 0;
    int endingProcessingQ = 0; 
    std::vector<LoadBalancer*> vec;
    vec.push_back(&streamingLB);
    vec.push_back(&processingLB);
    std::srand(std::time(0)); //seed rand()
    int last_upscale[2]   = {-10000, -10000};
    int last_downscale[2] = {-10000, -10000};
    while(managementLB.getTime() < maxTime){
        std::cout << "Current time: " << managementLB.getTime();
        if (managementLB.getTime() != 0){
            Request req = managementLB.generateRequest(managementLB.getConfig().requestGenProb, false, {});
            if(req.type != JobType::Management){
                if (managementLB.firewall(req)){
                    if(managementLB.recieveRequest(req)){
                        if(managementLB.sendRequestLB(*vec[JobUtil::jobToInt(req.type)])){
                        }
                    }
                }
            }
            else{
                std::cout << std::endl;
                managementLB.logEvent("No request generated");
            }
            for (int i = 0; i < 2; ++i){
                if(managementLB.getTime() - last_downscale[i] >= managementLB.getConfig().rest && vec[i]->queueSize() < managementLB.getConfig().lowThreshold * vec[i]->serverCount() && vec[i]->serverCount() > 1){
                    vec[i]->deleteServer();
                    last_downscale[i] = managementLB.getTime();
                }
                if(managementLB.getTime() - last_upscale[i] >= managementLB.getConfig().rest && vec[i]->queueSize() > managementLB.getConfig().highThreshold * vec[i]->serverCount()){
                    Server sev(JobUtil::intToJob(i)); 
                    vec[i]->addServer(sev);
                    last_upscale[i] = managementLB.getTime();   
                }
            }
        }
        managementLB.tick(vec);
        if (managementLB.getTime() == 1){
            startingStreamingQ= vec[0]->queueSize();
            startingProcessingQ = vec[1]->queueSize();
        }
        if (managementLB.getTime() == maxTime){
            endingStreamingQ= vec[0]->queueSize();
            endingProcessingQ = vec[1]->queueSize();
        }
        std::cout << std::endl; 
    }
    managementLB.syncStats(vec);
    managementLB.logEvent("Simulation ended");
    managementLB.logQueue(startingStreamingQ, startingProcessingQ, endingStreamingQ, endingProcessingQ); 
    managementLB.logStats();
    
}