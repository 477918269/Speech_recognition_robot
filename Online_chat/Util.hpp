#include<iostream>
#include<string>
#include"mongoose.h"
using namespace std;

class Util
{
public:
    static string mgStrToString(struct mg_str* ms)
    {
        string str = "";
        for(auto i = 0; i < ms->len; i++)
        {
            str.push_back(ms->p[i]);
        }
        return str;
    }

};
