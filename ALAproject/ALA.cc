#include"ALA.hpp"

using namespace std;
int main()
{
    /*Robot r;
    string str;
    volatile bool quit = false;
    while(!quit)
    {
        cout << "我: ";
        cin >> str;
        string s = r.Talk(str);
        cout << "胡必说: " << s << endl;
    }*/
    Jarvis *js = new Jarvis();
    if(!js->LoadEtc())
    {
        return 1;
    }
    js->Run();
    return 0;
}
