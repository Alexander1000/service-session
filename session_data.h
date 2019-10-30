#ifndef SERVICE_SESSION_SESSION_DATA_H
#define SERVICE_SESSION_SESSION_DATA_H

#include <string>

class SessionData
{
public:
    SessionData(std::string sessionId, int userId, std::string accessToken, std::string refreshToken);
    std::string sessionId;
    int userId;
    std::string accessToken;
    std::string refreshToken;
};

#endif //SERVICE_SESSION_SESSION_DATA_H
