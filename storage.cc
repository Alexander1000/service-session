/**
 * Storage Tarantool
 */

#include <iostream>
#include <string>

#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>

#include <msgpuck.h>

class Storage
{
public:
    Storage(std::string address) {
        this->address = address;
        this->spaceNo = 0;
        this->indexNo = 0;
    }

    void getBySessId(std::string sessId) {
        std::cout << "GetBySessId called with: " << sessId << std::endl;

        struct tnt_stream* tnt = this->connect();
        if (tnt == NULL) {
            std::cout << "error on get connect in get sessid" << std::endl;
            return;
        }

        tnt_reload_schema(tnt);

        int spaceNo = tnt_get_spaceno(tnt, "us", strlen("us"));
        if (spaceNo == -1) {
            std::cout << "error while get space no" << std::endl;
            this->free_connect(tnt);
            return;
        }

        std::cout << "space no: " << spaceNo << std::endl;

        int indexNo = tnt_get_indexno(tnt, spaceNo, "pk", strlen("pk"));
        if (indexNo == -1) {
            std::cout << "error while get index no" << std::endl;
            this->free_connect(tnt);
            return;
        }

        std::cout << "index no: " << indexNo << std::endl;

        struct tnt_stream *obj = NULL;
        obj = tnt_object(NULL);
        tnt_object_reset(obj);

        tnt_object_add_array(obj, 1);
        tnt_object_add_str(obj, sessId.c_str(), strlen(sessId.c_str()));

        int limit = 1, offset = 0;

        int bytes_number = tnt_select(tnt, spaceNo, indexNo, limit, offset, 0, obj);
        if (bytes_number == -1) {
            std::cout << "error: " << tnt_error(tnt) << std::endl;
            std::cout << "error while read data" << std::endl;
            this->free_connect(tnt);
            return;
        }

        struct tnt_reply * reply = tnt_reply_init(NULL); // Initialize reply
        tnt->read_reply(tnt, reply); // Read reply from server

        // unpack reply

        if (reply->code != 0) {
            std::cout << "Fail: " << reply->code << std::endl;
            this->free_connect(tnt);
            return;
        } else {
            std::cout << "TypeOf: " << mp_typeof(*reply->data) << std::endl;

            if (mp_typeof(*reply->data) != MP_ARRAY) {
                std::cout << "Bad reply format" << std::endl;
                this->free_connect(tnt);
                return;
            } else if (mp_decode_array(&reply->data) == 0) {
                std::cout << "Zero" << std::endl;
                this->free_connect(tnt);
                return;
            } else if (mp_decode_array(&reply->data) < 0) {
                std::cout << "Bad reply format" << std::endl;
                this->free_connect(tnt);
                return;
            }

            std::cout << "begin decode reply" << std::endl;

            const char *data = reply->data;
            uint32_t len = mp_decode_array(&data);
            if (len < 2) {
                std::cout << "err len: " << len << std::endl;
                this->free_connect(tnt);
                return;
            }

            uint32_t str_len = 0;

            // sessid
            if (mp_typeof(*data) != MP_STR) {
                std::cout << "bad reply format(sessid) " << mp_typeof(*data) << std::endl;
                this->free_connect(tnt);
                return;
            }
            char* tSessid = (char *)mp_decode_str(&data, &str_len);
            tSessid = strndup(tSessid, str_len);

            std::cout << "F: sessid: " << tSessid << std::endl;

            // user id
            if (mp_typeof(*data) != MP_UINT) {
                std::cout << "bad reply format" << std::endl;
                this->free_connect(tnt);
                return;
            }
            int userId = mp_decode_uint(&data);

            std::cout << "F: useId: " << userId << std::endl;

            // access token
            if (mp_typeof(*data) != MP_STR) {
                std::cout << "bad reply format(a-token)" << mp_typeof(*data) << std::endl;
                this->free_connect(tnt);
                return;
            }
            char* tAccessToken = (char *)mp_decode_str(&data, &str_len);
            tAccessToken = strndup(tAccessToken, str_len);
            std::cout << "F: access token: " << tAccessToken << std::endl;

            // refresh token
            if (mp_typeof(*data) != MP_STR) {
                std::cout << "bad reply format" << std::endl;
                this->free_connect(tnt);
                return;
            }
            char* tRefreshToken = (char *)mp_decode_str(&data, &str_len);
            tRefreshToken = strndup(tRefreshToken, str_len);
            std::cout << "F: refresh token: " << tRefreshToken << std::endl;
        }

        tnt_reply_free(reply); // Free reply

        this->free_connect(tnt);
    }
private:
    std::string address;

    int spaceNo;

    int indexNo;

    struct tnt_stream* connect() {
        struct tnt_stream *tnt = tnt_net(NULL); // Allocating stream
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

    int getSpaceNo() {
        if (this->spaceNo > 0) {
            return this->spaceNo;
        }

        struct tnt_stream* tnt = this->connect();
        if (tnt == NULL) {
            std::cout << "error on get connect in getSpaceNo" << std::endl;
            return -1;
        }

        this->spaceNo = tnt_get_spaceno(tnt, "us", strlen("us"));

        this->free_connect(tnt);

        return this->spaceNo;
    }

    int getIndexNo() {
        if (this->indexNo > 0) {
            return this->indexNo;
        }

        struct tnt_stream* tnt = this->connect();
        if (tnt == NULL) {
            std::cout << "error on get connect in getIndexNo" << std::endl;
            return -1;
        }

        this->indexNo = tnt_get_indexno(tnt, this->getSpaceNo(), "pk", 2);

        this->free_connect(tnt);

        return this->indexNo;
    }
};
