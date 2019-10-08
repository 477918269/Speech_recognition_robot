#include<iostream>
#include<json/json.h>
#include<sstream>
#include<memory>
#include<string>

using namespace std;

int main()
{
    Json::Value root;
    Json::StreamWriterBuilder wb;
    std::ostringstream os;

    root["你好"] = "hellow";
    root["绿色"] = "grean";

    std::unique_ptr<Json::StreamWriter> sw(wb.newStreamWriter());
    sw->write(root, &os);
    std::string s = os.str();
    
    std::cout << s << std::endl;
    return 0;
}

