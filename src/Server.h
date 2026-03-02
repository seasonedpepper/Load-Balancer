#pragma once 

#include "Request.h"

/**
 * @class Server
 * @brief Represents a single server capable of processing one request at a time.
 *
 * A Server instance is responsible for handling requests of a specific
 * JobType (e.g., Streaming or Processing). Each server maintains an
 * internal state indicating whether it is idle or currently processing
 * a request.
 *
 * A server can only process requests that match its configured JobType.
 */
class Server{
    public: 
        /**
         * @brief Constructs a Server object for a specific job type.
         *
         * Initializes the server in an idle state with no active request.
         *
         * @param type The JobType this server is responsible for handling.
         */
        Server(JobType type);
        /**
         * @brief Checks whether the server is currently idle.
         *
         * @return true if the server is not processing a request.
         * @return false if the server is busy processing a request.
         */
        bool isIdle();
        /**
         * @brief Returns the JobType handled by this server.
         *
         * @return The JobType assigned to this server.
         */
        JobType getType(); 
        /**
         * @brief Assigns a request to the server for processing.
         *
         * The request will only be accepted if its JobType matches
         * the server's configured JobType. If the types do not match,
         * the assignment fails and the server remains idle.
         *
         * @param request The Request object to assign.
         * @return true if the request was successfully assigned.
         * @return false if the request type does not match the server type.
         */
        bool assign(const Request& request);
        /**
         * @brief Decrements the remaining processing time of the current request.
         *
         * If the server is processing a request, its remaining time
         * is reduced by one unit (tick). When the remaining time
         * reaches zero, the server transitions back to the idle state.
         *
         * @return true if the request completed during this call.
         * @return false if the server is idle or the request is still processing.
         */
        bool decrementRequest();


    private:
        JobType type; 
        bool idle;
        Request current_request; 
};