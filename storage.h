#ifndef H_SESSION_STORAGE_INCLUDE_
#define H_SESSION_STORAGE_INCLUDE_

#include <string>
#include <map>

#include <tarantool/tarantool.h>

#include "session_data.h"

class Storage
{
    std::string address;

    int indexNo;

    std::map<std::string, int>* spaces;

    struct tnt_stream* connect();

    void free_connect(struct tnt_stream* tnt);

    int getSpaceNo(struct tnt_stream* tnt, std::string spaceName);

public:
    Storage(std::string address);

    SessionData* getById(std::string sessId);

    int save(SessionData *sessionData);

    int create(char* sessid);
};

#endif /* H_SESSION_STORAGE_INCLUDE_ */
