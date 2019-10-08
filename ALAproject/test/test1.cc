#include<iostream>
#include<fstream>

using namespace std;

int main()
{
    char* str = "ls -l";
    FILE* fp = popen(str, "r");
    char* c;
    while(fread(c, 1, 1, fp))
    {
        fwrite(c, 1, 1, stdout);
    }
    return 0;
}
