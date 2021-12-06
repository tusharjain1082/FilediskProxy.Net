#pragma once

#define DEFAULT_BUFLEN 512
static char DEFAULT_PORT[] = "27015";


typedef struct _CONTEXT_REQUEST {
    UCHAR MajorFunction;
    INT64 Length;
}CONTEXT_REQUEST, * PCONTEXT_REQUEST;
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04
static PCONTEXT_REQUEST request = NULL;
 

#define BUFFER_SIZE 104857600
static unsigned char __buffer[BUFFER_SIZE];//SOCKET_BUFFER_SIZE];
#define SOCKET_BUFFER_SIZE 10485760
#define REQUEST_BUFFER_SIZE 1024
static unsigned char __requestBuffer[REQUEST_BUFFER_SIZE + 1];


#define POOL_TAG 'ksiD'

static const char USERMODEAPP_REQUESTDATAEVENT_NAME[] = "Global\\ReqData";
static const char USERMODEAPP_SERVERIDLEEVENT_NAME[] = "Global\\ServerIdle";
static const char USERMODEAPP_REQUESTCOMPLETEEVENT_NAME[] = "Global\\RequestComplete";

#define DRIVER_REQUESTDATAEVENT_NAME L"\\BaseNamedObjects\\ReqData"
#define DRIVER_SERVERIDLEEVENT_NAME L"\\BaseNamedObjects\\ServerIdle"
#define DRIVER_REQUESTCOMPLETEEVENT_NAME L"\\BaseNamedObjects\\RequestComplete"