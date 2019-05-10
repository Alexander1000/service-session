/**
 * Storage Tarantool
 */

#include <string>

#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>

class Storage
{
public:
    Storage(std::string address) {
        this->address = address;
        this->spaceNo = 0;
    }

    void getBySessId(std::string sessId) {
        struct tnt_stream * tnt = tnt_net(NULL); // Allocating stream
        tnt_set(tnt, TNT_OPT_URI, this->address.c_str()); // Setting URI
        tnt_set(tnt, TNT_OPT_SEND_BUF, 0); // Disable buffering for send
        tnt_set(tnt, TNT_OPT_RECV_BUF, 0); // Disable buffering for recv
        tnt_connect(tnt); // Initialize stream and connect to Tarantool
        tnt_ping(tnt); // Send ping request
        struct tnt_reply * reply = tnt_reply_init(NULL); // Initialize reply
        tnt->read_reply(tnt, reply); // Read reply from server
        tnt_reply_free(reply); // Free reply
        tnt_close(tnt);
        tnt_stream_free(tnt); // Close connection and free stream object
    }
private:
    std::string address;

    int spaceNo;

    int getSpaceNo() {
        if (this->spaceNo > 0) {
            return this->spaceNo;
        }

        struct tnt_stream * tnt = tnt_net(NULL); // Allocating stream
        tnt_set(tnt, TNT_OPT_URI, this->address.c_str()); // Setting URI
        tnt_set(tnt, TNT_OPT_SEND_BUF, 0); // Disable buffering for send
        tnt_set(tnt, TNT_OPT_RECV_BUF, 0); // Disable buffering for recv
        tnt_connect(tnt); // Initialize stream and connect to Tarantool
        this->spaceNo = tnt_get_spaceno(tnt, "us", 2);
        tnt_close(tnt);
        tnt_stream_free(tnt); // Close connection and free stream object

        return this->spaceNo;
    }
};
