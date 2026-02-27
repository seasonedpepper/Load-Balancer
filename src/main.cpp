#include "LoadBalancer.h"
#include "Request.h"
#include "Server.h"
#include <climits>
int main(){
    int maxTime; 
    std::cin >> maxTime; 
    LoadBalancer managementLB(0, JobType::Management);
    LoadBalancer streamingLB(1, JobType::Streaming);
    LoadBalancer processingLB(2, JobType::Processing);
    std::vector<LoadBalancer> vec;
    vec.push_back(streamingLB);
    vec.push_back(processingLB);
    while(managementLB.getTime() <= maxTime){
        Request req = managementLB.generateRequest();
        if(req.type != JobType::Management){
            if (managementLB.firewall(req)){
                if(managementLB.sendRequestLB(req)){
                    vec[req.type].recieveRequest(req);
                }
            }
        }
        int last_upscale = -10000;
        int last_downscale = -10000; 
        for (int i = 0; i < 2; ++i){
            if(managementLB.getTime() - last_downscale > managementLB.getConfig().rest){
                vec[i].deleteServer();
                last_downscale = managementLB.getTime();
            }
            if(managementLB.getTime() - last_upscale > managementLB.getConfig().rest){
                Server sev(); 
                vec[i].addServer(req.intToJob(i));
                last_upscale = managementLB.getTime();
            }
        }
        managementLB.tick(vec);
        managementLB.loadConfig();
        streamingLB.loadConfig();
        processingLB.loadConfig();
    }
    
}