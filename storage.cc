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

        int spaceNo = 0, indexNo = 0;
        // spaceNo = this->getSpaceNo();
        // indexNo = this->getIndexNo();

        struct tnt_stream* tnt = this->connect();
        if (tnt == NULL) {
            std::cout << "error on get connect in get sessid" << std::endl;
            return;
        }

        spaceNo = tnt_get_spaceno(tnt, "us", strlen("us"));
        if (spaceNo == -1) {
            std::cout << "error in gen space no" << std::endl;
            this->free_connect(tnt);
            return;
        }

        std::cout << "space no: " << spaceNo << std::endl;

        struct tnt_stream *obj = NULL;
        obj = tnt_object(NULL);
        tnt_object_add_str(obj, sessId.c_str(), strlen(sessId.c_str()));

        tnt_select(tnt, spaceNo, 0, 1, 0, 0, obj);

        struct tnt_reply * reply = tnt_reply_init(NULL); // Initialize reply
        tnt->read_reply(tnt, reply); // Read reply from server

        // unpack reply

        if (reply->code != 0) {
            std::cout << "Fail: " << reply->code << std::endl;
        } else {
            std::cout << "TypeOf: " << mp_typeof(*reply->data) << std::endl;
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

        this->spaceNo = tnt_get_spaceno(tnt, "us", 2);

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
