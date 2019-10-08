#include<iostream>
#include<string>
#include"mongoose.h"
#include"Util.hpp"

struct mg_serve_http_opts s_http_server_opts;
class OC_server
{
private:
    string port;
    struct mg_mgr mgr;
    struct mg_connection* nc;
    volatile bool quit;
public:
    static void Broadcast(struct mg_connection* nc, string msg)
    {
        struct mg_connection* c;
        for(c = mg_next(nc->mgr, NULL); c!= NULL; c = mg_next(nc->mgr, c))
        {
            //if(c == nc)
            
            mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, msg.c_str(), msg.size());
        }
    }
    static void ev_handler(struct mg_connection* nc, int ev, void* ev_data)
    {
        switch(ev)
        {
            case MG_EV_HTTP_REQUEST:
            {
                struct http_message *hm = (struct http_message*) ev_data;
                //nc->user_data = s;
                //cout << "login nc addr:" << nc << endl;
                mg_serve_http(nc, hm, s_http_server_opts);
            }
            break;
            case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
            {
                std::string sips = "有新人加入";
                Broadcast(nc, "new friend");
            }
            break;
            case MG_EV_WEBSOCKET_FRAME:
            {
                struct websocket_message* wm = (struct websocket_message* )ev_data;
                struct mg_str ms = {(const char*)wm->data, wm->size};
                string msg = Util::mgStrToString(&ms);
                Broadcast(nc, msg);

            }
            break;
            case MG_EV_CLOSE:
                break;
            default:
                cout<<"other ev: " << ev << endl;
                break;
        }
    }

    void InitServer()
    {
        mg_mgr_init(&mgr, NULL);
        nc = mg_bind(&mgr, port.c_str(), ev_handler);
        mg_set_protocol_http_websocket(nc);
        s_http_server_opts.document_root = "web";
    }
    OC_server(string _port = "8080")
        :port(_port),
        quit(false)
    {
    }
    void Start()
    {
        int timeout = 2000;
        while(!quit)
        {
            mg_mgr_poll(&mgr, timeout);
            cout << "time out" << endl;
        }
    }

    ~OC_server()
    {
        mg_mgr_free(&mgr);
    }

  
};
