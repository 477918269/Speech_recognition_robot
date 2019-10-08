#include"OC_server.hpp"

int main()
{
    OC_server* os = new OC_server();
    os->InitServer();
    os->Start();
    return 0;
}
