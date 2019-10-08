#include<iostream>
#include<json/json.h>
#include<sstream>
#include<memory>
#include<string>

using namespace std;

void ParseJson(std::string &s)
{
    JSONCPP_STRING errs;
    Json::Value root;
    Json::CharReaderBuilder rb;
    std::unique_ptr<Json::CharReader> jr (rb.newCharReader());
    bool res = jr->parse(s.data(), s.data() + s.size(), &root, &errs);

    std::cout << "Name :" << root["你好"].asString() << std::endl;
    std::cout << "绿色 :" << root["绿色"].asString() << std::endl;


}

int main()
{
    std::string s = "{\"你好\" : \"hellow\",\"绿色\" : \"grean\"}";
    cout << s << endl;
    ParseJson(s);
    return 0;
}

