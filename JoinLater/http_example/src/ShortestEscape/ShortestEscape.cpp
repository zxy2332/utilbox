#include "ShortestEscape.h"
#include "ShortestAlgorithm.h"
#include "util.h"
#include "fastcsv/csv.h"
#include "json/json.hpp"
#include <unordered_map>
#include <iostream>
#include <set>
#include "cslog.h"



//CSV解析器的线程支持
//#define  CSV_IO_NO_THREAD

//读取json数据
template<class T>
bool GetJsonData(const nlohmann::json &js, const std::string &key, T &data) {
    if (js.count(key)) {
        if (!js.at(key).empty()) {
            data = js[key];
            return true;
        }
    }
    return false;
}

struct NodeInfo {
    int id;
    std::string pos;
};

struct PathInfo {
    //起点
    NodeInfo start;
    //终点
    NodeInfo end;
    //距离
    double dis;
};

struct PriveDate {
    //最短路径算法
    ShortestAlgorithm<int,double> algo;
    //存放初始数据 <id, 节点信息>
    std::unordered_map<int, NodeInfo> node_data;
    //存放节点连接状态 <节点id，<连接节点id，长度> >
    std::unordered_map<int, std::unordered_map<int,double>> node_connect;

    std::string path_data_file;
    bool debug = false;

    void clear() {
        algo.clear();
        node_data.clear();
        node_connect.clear();
    }
};

ShortestEscape::ShortestEscape() {
    //Log_Manager()->CreateLog("logs/log.log");
    d_ = new PriveDate;
}

ShortestEscape::ShortestEscape(bool debug) :ShortestEscape(){
    SetDebug(debug);
}
ShortestEscape::~ShortestEscape() {
    delete d_;
}

bool ShortestEscape::ReadPathDataFile(const std::string &file_name) {
    TimeBegin(ReadPathDataFile_time);
    try {
        d_->clear();
        d_->path_data_file = file_name;
        if (!util::IsExist(file_name)) {
            util::Throw("File does not exist. Path:" + file_name);
        }
        io::CSVReader<5> in(file_name);
        in.read_header(io::ignore_extra_column, "startid", "startpos", "endid", "endpos", "dis");
        PathInfo data;
        while (in.read_row(data.start.id, data.start.pos, data.end.id, data.end.pos, data.dis)) {
            //保存路径
            d_->algo.update_edge(data.start.id, data.end.id, data.dis, false);
            //保存起点
            if (d_->node_data.find(data.start.id) == d_->node_data.end()){
                d_->node_data.emplace(data.start.id, data.start);
            }
            //保存终点
            if (d_->node_data.find(data.end.id) == d_->node_data.end()){
                d_->node_data.emplace(data.end.id, data.end);
            }
            //保存节点连接状态
            d_->node_connect[data.start.id].emplace(data.end.id,data.dis);
        }
    }
    catch (const std::exception &e) {
        util::Throw(e);
        return false;
    }
    catch (...) {
        util::Throw("PathData parse error(...). Path:" + file_name);
        return false;
    }
    TimeEnd(d_->debug,ReadPathDataFile_time);
    return true;
}

bool ShortestEscape::ReadPathDataContent(const std::string &file_content) {
    if (file_content.empty()) {
        util::Throw("PathData is empty");
        return false;
    }
    //TODO
    return false;
}


std::vector<int> ShortestEscape::GetShortestPath(int from, int to) {
    return d_->algo.shortest_path(from, to);
}

bool ShortestEscape::GetShortestPath(const std::string &json_content,std::string &out_path_content) {
    TimeBegin(GetShortestPath_time);
    try {
        if (d_->algo.Vertices().empty()){
            util::Throw("Graph Path Data is empty, Path:"+d_->path_data_file);
            return false;
        }
        auto js = nlohmann::json::parse(json_content);

        int StartID;
        if (!GetJsonData(js, "StartID", StartID)) {
            util::Throw("StartID param missing");
            return false;
        }

        int EndID;
        if (!GetJsonData(js, "EndID", EndID)) {
            util::Throw("EndID param missing");
            return false;
        }

        nlohmann::json State;
        if (!GetJsonData(js, "State", State)) {
            //此处可以为空
            //util::Throw("State field missing");
            //return false;
        }
        for (auto it = State.begin(); it != State.end(); it++) {
            if (it->empty()) {
                continue;
            }

            auto array_value = it.value();
            int NodeID;
            if (!GetJsonData(array_value, "NodeID", NodeID)) {
                continue;
            }
            int State;
            if (!GetJsonData(array_value, "State", State)) {
                continue;
            }
            if (d_->node_connect.find(NodeID) == d_->node_connect.end()){
                continue;
            }
            const auto &connect_info = d_->node_connect[NodeID];
            for (auto c_it = connect_info.begin();c_it!=connect_info.end();c_it++) {
                const auto &c_id = c_it->first;
                const auto &c_dis = c_it->second;
                if(State == 0){
                    //若状态为0，则为关闭状态，此时将长度设置为无穷大
                    d_->algo.update_edge(NodeID,c_id,ShortestAlgorithm<int,double>::INVALID, false);
                }else if(State == 1){
                    //若状态为0，则为开启状态，此时将长度恢复初始设置
                    d_->algo.update_edge(NodeID,c_id,c_dis, false);
                }else{
                    //其他情况
                }
            }
        }
        auto path = GetShortestPath(StartID, EndID);
        std::vector<std::string> pos_path ;
        for (auto it = path.begin(); it != path.end() ; it++) {
            pos_path.push_back(d_->node_data[*it].pos);
        }
        nlohmann::json out;
        out["path"] = pos_path;
        if (d_->debug){
            out["id_path"] = path;
        }
        out_path_content = out.dump();
    } catch (const nlohmann::json::exception &e) {
        util::Throw(e.what());
        return false;
    }catch (const std::exception &e) {
        util::Throw(e);
        return false;
    }
    TimeEnd(d_->debug,GetShortestPath_time);
    return true;
}

bool ShortestEscape::GetShortestPath_file(const std::string &json_file, std::string &out_path_content) {
    return GetShortestPath(util::file_contents(json_file),out_path_content);
}

bool ShortestEscape::SetDebug(bool debug) {
    d_->debug = debug;
    return true;
}

