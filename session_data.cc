#include <string>

#include "session_data.h"

SessionData::SessionData(std::string sessionId, int userId, std::string accessToken, std::string refreshToken) {
    this->sessionId = sessionId;
    this->userId = userId;
    this->accessToken = accessToken;
    this->refreshToken = refreshToken;
}
