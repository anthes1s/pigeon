#ifndef REQUESTTYPE_H
#define REQUESTTYPE_H

#define HOST_IP "192.168.1.143"
#define HOST_PORT 55030

enum RequestType {
    USER_LOGIN,
    USER_LOGIN_SUCCESS,
    USER_LOGIN_FAIL,
    USER_CONNECTED,
    USER_DISCONNECTED, /// delete user from lwUsers when a client disconnects
    USER_REGISTRATION,
    USER_REGISTRATION_SUCCESS,
    USER_REGISTRATION_FAIL,
    SEND_MESSAGE,
};
#endif // REQUESTTYPE_H