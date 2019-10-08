#ifndef __ALA__
#define __ALA__

#include<iostream>
#include<string>
#include<sstream>
#include <memory>
#include<json/json.h>
#include<fstream>
#include<cstdio>
#include<unistd.h>
#include<fcntl.h>
#include<unordered_map>
#include<fstream>
#include<map>
#include<sys/types.h>
#include<sys/stat.h>
#include"base/http.h"
#include"speech.h"

#define ASR_PATH "temp_file/asr.wav"
#define CMD_ETC "command.etc"
#define TTS_PATH "temp_file/tts.mp3"

class Util
{
public:
    static bool Exec(std::string command, bool is_printf)
    {
        if(!is_printf)
        {
            command += ">/dev/null 2>&1";

        }
        FILE* fp = popen(command.c_str(), "r");
        if(nullptr == fp)
        {
            std::cerr << "popen exec \'" << command << "\' Error" << std :: endl;
            return false;
        }
        if(is_printf)
        {
            char ch;
            while(fread(&ch, 1, 1, fp) > 0)
            {
                fwrite(&ch, 1, 1, stdout);
            }
        }
        pclose(fp);
        return true; 
    }
};

class Robot 
{
private:
    std::string url;//对应的url平台
    std::string api_key;//注册对应的api
    std::string user_id;//用户自定义ID
    aip::HttpClient client;//直接调用百度相关接口
private:
    std::string MessageToJson(std::string& message)
    {
        Json::Value root;
        Json::StreamWriterBuilder wb;
        std::ostringstream ss;

        Json::Value item_;
        item_["text"] = message;

        Json::Value item;
        item["inputText"] = item_;

        root["reqType"] = 0;
        root["perception"] = item;

        item.clear();
        item["apiKey"] = api_key;
        item["userId"] = user_id;
        root["userInfo"] = item;


        std::unique_ptr<Json::StreamWriter> sw(wb.newStreamWriter());
        sw->write(root, &ss);
        std::string json_string = ss.str();
        //std::cout << "debug: " << json_string << std::endl;
        return json_string;
    }
    std::string RequestTL(std::string &request_json)
    {
        std::string response;
        int status_code = client.post(url, nullptr, request_json, nullptr, &response);
        if(status_code != CURLcode::CURLE_OK)
        {
            std::cerr << "post error" << std::endl;
            return "";
        }
        return response;
    }
    std::string JsonToBack(std::string &str)
    {
        
        JSONCPP_STRING errs;
        Json::Value root;
        Json::CharReaderBuilder rb;
        std::unique_ptr<Json::CharReader> const cr(rb.newCharReader());
        bool res = cr->parse(str.data(), str.data() + str.size(), &root, &errs);
        if(!res || !errs.empty())
        {
            std::cerr << "parse error!" << std::endl;
            
        }
        int code = root["intent"]["code"].asInt();
        if(code == 5000 )
        {
            std::cerr << "response code error" << std::endl;
            return "";
        }
        Json::Value item = root["results"][0];
        std::string msg = item["values"]["text"].asString();
        return msg;
    }
public:
    Robot(std::string id = "1")
    {
        this->url = "http://openapi.tuling123.com/openapi/api/v2";
        this->api_key = "77545728c320448288de0c21f8db99d0";
        this->user_id = id;
    }
    std::string Talk(std::string message)
    {
        std::string json = MessageToJson(message);
        std::string response  = RequestTL(json);
        std::string echo_message = JsonToBack(response);

        return echo_message;
    }
    ~Robot()
    {}
};
class SpeechRec
{
private:
    std::string app_id;
    std::string api_key;
    std::string secret_key;
    aip::Speech *client;
public:
    SpeechRec()
    {
        app_id = "16928783";
        api_key = "GYRSAGLlek53Vwvfaci9mMNi";
        secret_key = "t8qqYOdRFGU4GKxdhi0KUEIuAlp137Dj";
        client = new aip::Speech(app_id,api_key, secret_key);
    }

    bool ASR(std::string path, std::string &out)
    {
        std::map<std::string, std::string> options;
        options["dev_pid"] = "1536";
        std::string file_content;
        aip::get_file_content(ASR_PATH, &file_content);
        Json::Value result = client->recognize(file_content, "wav", 16000, options);
        // std::cout << "debug: "<< result.toStyledString() << std::endl;
        
        int code = result["err_no"].asInt();
        if(code == 3000)
        {
            std::cerr << "recognize error" << std::endl;
            return false;
        }

        out = result["result"][0].asString();
        return true;
    }
    bool TTS(std::string message)
    {
        bool ret;
        std::ofstream ofile;
        std::string ret_file;
        std::map<std::string, std::string> options;
        ofile.open(TTS_PATH, std::ios::out | std::ios::binary);

        Json::Value result = client->text2audio(message, options, ret_file);
        if(!ret_file.empty())
        {
            ofile << ret_file;
            ofile.close();
            ret = true;
        }
        else
        {
            std::cerr << result.toStyledString() << std::endl;
            ret = false;
        }
        ofile.close();
        return ret;
    }

    ~SpeechRec()
    {}
};


class Jarvis
{
private:
    Robot rt;
    SpeechRec sr;
    std::unordered_map<std::string, std::string> commands;
private:
    bool Record()
    {
        std::cout << "debug: " << "Record ...." << std::endl;
        std::string command = "arecord -t wav -c 1 -r 16000 -d 5 -f S16_LE ";
        command += ASR_PATH;
        bool ret =  Util::Exec(command, false);


        std::cout << "debug : Recoed ... done" << std::endl;
        return ret;
    }

    bool Play()
    {
       std::string command = "cvlc --play-and-exit ";
       command += TTS_PATH;
       return Util::Exec(command, false);
    }
public:
    Jarvis()
    {}
    bool LoadEtc()
    {
        std::ifstream in(CMD_ETC);
        if(!in.is_open())
        {
            std::cerr << "open error" <<std::endl;
            return false;
        }

        std::string sep = ":";
        char line[256];
        while(in.getline(line, sizeof(line)))
        {
            std::string str = line;
            std::size_t pos = str.find(sep);
            if(std::string::npos == pos)
            {
                std::cerr<< "not find:" << std::endl;
                continue;
            }
            std::string k = str.substr(0,pos);
            std::string v = str.substr(pos+sep.size());
            k += "。";
            commands.insert({k, v});
        }

        std:: cerr << "load command etc done ... success" << std::endl;
        in.close();
        return true;
    }
    
    bool IsCommand(std::string message, std::string &cmd)
    {
        auto iter = commands.find(message);
        if(iter == commands.end())
        {
            return false;
        }
        cmd = iter->second;
        return true;
    }
    void Run()
    {
        volatile bool quit = false;
        while(!quit)
        {
            if(this->Record())
            {
                std::string message;
                if(sr.ASR(ASR_PATH, message))
                {
                    std::string cmd="";
                    if(IsCommand(message, cmd))
                    {
                        std::cout << "[ALA@localhost]$ " << cmd << std::endl;
                        Util::Exec(cmd, true);
                        continue;
                    }
                    std::cout<< "我# " << message <<std::endl;
                    std::string echo = rt.Talk(message);
                    std::cout << "图灵机器人# " << echo << std::endl;
                    if(sr.TTS(echo))
                    {
                        this->Play();
                    }
                }

                else
                {
                    std::cerr << "Recognize error..." << std::endl;
                }
            }
            else
            {
                std::cerr << "Record error ...." << std::endl;
            }
            sleep(2);
        }
    }
    ~Jarvis()
    {}

};




















#endif
