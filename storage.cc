/**
 * Storage Tarantool
 */

#include <iostream>
#include <string>
#include <memory.h>
#include <map>

#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>

#include <msgpuck.h>

#define UNDEFINED_VALUE -1

#define TARANTOOL_SPACE_USER_SESSION "us"

void gen_random(char *s, const int len) {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

class Storage
{
public:
    Storage(std::string address) {
        this->address = address;
        this->spaceNo = UNDEFINED_VALUE;
        this->indexNo = UNDEFINED_VALUE;
        this->spaces = new std::map<std::string, int>;
    }

    SessionData* getById(std::string sessId) {
        std::cout << "GetBySessId called with: " << sessId << std::endl;

        struct tnt_stream* tnt = this->connect();
        if (tnt == NULL) {
            std::cout << "error on get connect in get sessid" << std::endl;
            return NULL;
        }

        int spaceNo = this->getSpaceNo(tnt, TARANTOOL_SPACE_USER_SESSION);

        if (spaceNo == UNDEFINED_VALUE) {
            std::cout << "error while get space no" << std::endl;
            this->free_connect(tnt);
            return NULL;
        }

        std::cout << "space no: " << spaceNo << std::endl;

        if (this->indexNo == UNDEFINED_VALUE) {
            int indexNo = tnt_get_indexno(tnt, spaceNo, "pk", strlen("pk"));
            if (indexNo == -1) {
                std::cout << "error while get index no" << std::endl;
                this->free_connect(tnt);
                return NULL;
            }
            this->indexNo = indexNo;
        }

        std::cout << "index no: " << this->indexNo << std::endl;

        struct tnt_stream *obj = NULL;
        obj = tnt_object(NULL);
        tnt_object_reset(obj);

        tnt_object_add_array(obj, 1);
        tnt_object_add_str(obj, sessId.c_str(), strlen(sessId.c_str()));

        int limit = 1, offset = 0;

        int bytes_number = tnt_select(tnt, spaceNo, this->indexNo, limit, offset, TNT_ITER_EQ, obj);
        if (bytes_number == -1) {
            std::cout << "error: " << tnt_error(tnt) << std::endl;
            std::cout << "error while read data" << std::endl;
            this->free_connect(tnt);
            return NULL;
        }

        std::cout << "count bytes: " << bytes_number << std::endl;

        struct tnt_reply* reply = tnt_reply_init(NULL); // Initialize reply

        // Read reply from server
        if (tnt->read_reply(tnt, reply) == -1) {
            std::cout << "Failed to recv/parse result" << std::endl;
            if (tnt_error(tnt)) {
                std::cout << "Error: " << tnt_strerror(tnt) << std::endl;
            }
            tnt_reply_free(reply); // Free reply
            this->free_connect(tnt);
            return NULL;
        }

        // unpack reply

        if (reply->code != 0) {
            std::cout << "Fail: " << reply->code << std::endl;
            tnt_reply_free(reply); // Free reply
            this->free_connect(tnt);
            return NULL;
        }

        std::cout << "TypeOf: " << mp_typeof(*reply->data) << std::endl;

        if (mp_typeof(*reply->data) != MP_ARRAY) {
            std::cout << "Bad reply format (not array)" << std::endl;
            this->free_connect(tnt);
            return NULL;
        } else if (mp_decode_array(&reply->data) == 0) {
            // session data not found
            std::cout << "Zero" << std::endl;
            this->free_connect(tnt);
            return NULL;
        } else if (mp_decode_array(&reply->data) < 0) {
            std::cout << "Bad reply format (decode size)" << std::endl;
            this->free_connect(tnt);
            return NULL;
        }

        std::cout << "begin decode reply" << std::endl;

        const char* data = reply->data;

        uint32_t str_len = 0;

        // sessid
        if (mp_typeof(*data) != MP_STR) {
            std::cout << "bad reply format(sessid) " << mp_typeof(*data) << std::endl;
            this->free_connect(tnt);
            return NULL;
        }
        char* tSessid = (char *)mp_decode_str(&data, &str_len);
        tSessid = strndup(tSessid, str_len);

        std::cout << "F: sessid: " << tSessid << std::endl;

        // user id
        if (mp_typeof(*data) != MP_UINT) {
            std::cout << "bad reply format(user-id)" << std::endl;
            this->free_connect(tnt);
            return NULL;
        }
        int userId = mp_decode_uint(&data);

        std::cout << "F: useId: " << userId << std::endl;

        // access token
        if (mp_typeof(*data) != MP_STR) {
            std::cout << "bad reply format(a-token)" << mp_typeof(*data) << std::endl;
            this->free_connect(tnt);
            return NULL;
        }
        char* tAccessToken = (char *)mp_decode_str(&data, &str_len);
        tAccessToken = strndup(tAccessToken, str_len);
        std::cout << "F: access token: " << tAccessToken << std::endl;

        // refresh token
        if (mp_typeof(*data) != MP_STR) {
            std::cout << "bad reply format(r-token)" << std::endl;
            this->free_connect(tnt);
            return NULL;
        }
        char* tRefreshToken = (char *)mp_decode_str(&data, &str_len);
        tRefreshToken = strndup(tRefreshToken, str_len);
        std::cout << "F: refresh token: " << tRefreshToken << std::endl;

        tnt_reply_free(reply); // Free reply

        this->free_connect(tnt);

        SessionData* sessionData = (SessionData*) malloc(sizeof(SessionData));
        sessionData = new SessionData(std::string(tSessid), userId, std::string(tAccessToken), std::string(tRefreshToken));
        return sessionData;
    }

    int save(SessionData *sessionData) {
        struct tnt_stream* conn = this->connect();
        if (conn == NULL) {
            return -1;
        }

        if (this->spaceNo == UNDEFINED_VALUE) {
            tnt_reload_schema(conn);
        }

        if (this->spaceNo == UNDEFINED_VALUE) {
            int spaceNo = tnt_get_spaceno(conn, TARANTOOL_SPACE_USER_SESSION, strlen(TARANTOOL_SPACE_USER_SESSION));
            if (spaceNo == -1) {
                std::cout << "error while get space no" << std::endl;
                this->free_connect(conn);
                return -1;
            }
            this->spaceNo = spaceNo;
        }

        struct tnt_stream *tuple = tnt_object(NULL);

        tnt_object_reset(tuple);
        tnt_object_add_array(tuple, 4);

        tnt_object_add_str(tuple, sessionData->sessionId.c_str(), strlen(sessionData->sessionId.c_str()));
        tnt_object_add_int(tuple, sessionData->userId);
        tnt_object_add_str(tuple, sessionData->accessToken.c_str(), strlen(sessionData->accessToken.c_str()));
        tnt_object_add_str(tuple, sessionData->refreshToken.c_str(), strlen(sessionData->refreshToken.c_str()));

        ssize_t bytes_count = tnt_replace(conn, this->spaceNo, tuple);
        if (bytes_count == -1) {
            std::cout << "error: " << tnt_error(conn) << std::endl;
            return -1;
        }

        std::cout << "wrote bytes: " << bytes_count << std::endl;

        this->free_connect(conn);
        return 0;
    }

    int create(char* sessid) {
        struct tnt_stream* conn = this->connect();
        if (conn == NULL) {
            return -1;
        }

        if (this->spaceNo == UNDEFINED_VALUE) {
            tnt_reload_schema(conn);
        }

        if (this->spaceNo == UNDEFINED_VALUE) {
            int spaceNo = tnt_get_spaceno(conn, TARANTOOL_SPACE_USER_SESSION, strlen(TARANTOOL_SPACE_USER_SESSION));
            if (spaceNo == -1) {
                std::cout << "error while get space no" << std::endl;
                this->free_connect(conn);
                return -1;
            }
            this->spaceNo = spaceNo;
        }

        struct tnt_stream *tuple = tnt_object(NULL);

        tnt_object_reset(tuple);
        tnt_object_add_array(tuple, 4);

        memset(sessid, 0, sizeof(char) * 65);

        // pos: 0-11
        gen_random(sessid, 12);
        // pos: 12
        sessid[12] = '.';
        // pos: 13-25
        gen_random(sessid + 13, 13);
        // pos: 26
        sessid[26] = ':';
        // pos: 27-38
        gen_random(sessid + 27, 13);
        // pos: 39
        sessid[39] = '-';
        // pos: 40-50
        gen_random(sessid + 40, 11);
        // pos: 51
        sessid[51] = '.';
        // pos: 52-64
        gen_random(sessid + 52, 12);

        tnt_object_add_str(tuple, sessid, strlen(sessid));
        tnt_object_add_int(tuple, 0);
        tnt_object_add_str(tuple, "", 0);
        tnt_object_add_str(tuple, "", 0);

        // todo: check exist
        ssize_t bytes_count = tnt_insert(conn, this->spaceNo, tuple);
        if (bytes_count == -1) {
            std::cout << "error: " << tnt_error(conn) << std::endl;
            return -1;
        }

        std::cout << "wrote bytes: " << bytes_count << std::endl;

        this->free_connect(conn);

        return 0;
    }
private:
    std::string address;

    int spaceNo;

    int indexNo;

    std::map<std::string, int>* spaces;

    struct tnt_stream* connect() {
        struct tnt_stream* tnt = tnt_net(NULL); // Allocating stream
        std::cout << "connection to " << this->address.c_str() << std::endl;
        tnt_set(tnt, TNT_OPT_URI, this->address.c_str()); // Setting URI
        tnt_set(tnt, TNT_OPT_SEND_BUF, 0); // Disable buffering for send
        tnt_set(tnt, TNT_OPT_RECV_BUF, 0); // Disable buffering for recv

        // Initialize stream and connect to Tarantool
        if (tnt_connect(tnt) == -1) {
            tnt_stream_free(tnt);
            return NULL;
        }

        return tnt;
    }

    void free_connect(struct tnt_stream* tnt) {
        tnt_close(tnt);
        tnt_stream_free(tnt); // Close connection and free stream object
    }

    int getSpaceNo(struct tnt_stream* tnt, std::string spaceName)
    {
        if (this->spaces->find(spaceName) != this->spaces->end()) {
            return this->spaces->at(spaceName);
        }

        tnt_reload_schema(tnt);
        int spaceNo = tnt_get_spaceno(tnt, spaceName.c_str(), strlen(spaceName.c_str()));
        if (spaceNo > 0) {
            (*this->spaces)[spaceName] = spaceNo;
            return spaceNo;
        }

        int tntError = tnt_errno(tnt);
        if (tntError == 0) {
            // try create space
            tnt_reload_schema(tnt);
        }

        return UNDEFINED_VALUE;
    }
};
