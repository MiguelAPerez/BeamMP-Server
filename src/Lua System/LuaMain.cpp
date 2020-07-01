///
/// Created by Anonymous275 on 5/20/2020
///

#include "LuaSystem.hpp"
#include "../logger.h"
#include <thread>
std::set<Lua*> PluginEngine;
bool NewFile(const std::string&Path){
    for(Lua*Script : PluginEngine){
        if(Path == Script->GetFileName())return false;
    }
    return true;
}
void RegisterFiles(const std::string& Path,bool HotSwap){
    std::string Name = Path.substr(Path.find_last_of('\\')+1);
    if(!HotSwap)info("Loading plugin : " + Name);
    for (const auto &entry : fs::directory_iterator(Path)){
        int pos = entry.path().string().find(".lua");
        if (pos != std::string::npos && entry.path().string().length() - pos == 4) {
            if(!HotSwap || NewFile(entry.path().string())){
                Lua *Script = new Lua();
                PluginEngine.insert(Script);
                Script->SetFileName(entry.path().string());
                Script->SetPluginName(Name);
                Script->SetLastWrite(fs::last_write_time(Script->GetFileName()));
                Script->Init();
                if(HotSwap)info("[HOTSWAP] Added : " +
                Script->GetFileName().substr(Script->GetFileName().find('\\')));
            }
        }
    }
}
void FolderList(const std::string& Path,bool HotSwap){
    for (const auto &entry : fs::directory_iterator(Path)) {
        int pos = entry.path().filename().string().find('.');
        if (pos == std::string::npos) {
            RegisterFiles(entry.path().string(),HotSwap);
        }
    }
}
[[noreturn]]void HotSwaps(const std::string& path){
    while(true){
        for(Lua*Script : PluginEngine){
            struct stat Info{};
            if(stat(Script->GetFileName().c_str(), &Info) != 0){
                PluginEngine.erase(Script);
                info("[HOTSWAP] Removed : "+
                Script->GetFileName().substr(Script->GetFileName().find('\\')));
                break;
            }
            if(Script->GetLastWrite() != fs::last_write_time(Script->GetFileName())){
                Script->SetLastWrite(fs::last_write_time(Script->GetFileName()));
                Script->Reload();
                info("[HOTSWAP] Updated : "+
                Script->GetFileName().substr(Script->GetFileName().find('\\')));
            }
        }
        FolderList(path,true);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}



void LuaMain(std::string Path){
    Path += "/Server";
    struct stat Info{};
    if(stat( Path.c_str(), &Info) != 0){
        fs::create_directory(Path);
    }
    FolderList(Path,false);
    std::thread t1(HotSwaps,Path);
    t1.detach();
    info("Lua system online");
}
