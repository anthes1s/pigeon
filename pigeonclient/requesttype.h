#ifndef REQUESTTYPE_H
#define REQUESTTYPE_H

#define HOST_IP "192.168.1.143"

enum RequestType {
    USER_CONNECTED,
    USER_DISCONNECTED, /// delete user from lwUsers when a client disconnects
    USER_REGISTRATION,
    USER_REGISTRATION_SUCCESS,
    USER_REGISTRATION_FAIL,
    SEND_MESSAGE,
};
#endif // REQUESTTYPE_H
