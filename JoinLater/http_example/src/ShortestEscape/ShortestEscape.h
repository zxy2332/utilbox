#ifndef SHORTEST_ESCAPE_H
#define SHORTEST_ESCAPE_H

#include <vector>
#include <string>
#include "shortestescape_export.h"

struct PriveDate;
class SHORTESTESCAPE_EXPORT ShortestEscape {
public:
    ShortestEscape();
    //调试用
    ShortestEscape(bool debug);

    virtual ~ShortestEscape();

    //设置开启debug
    bool SetDebug(bool debug);

    //读取路径数据文件
    bool ReadPathDataFile(const std::string &file_name);
    //TODO::读取路径数据文件内容
    bool ReadPathDataContent(const std::string &file_content);

    // 根据json文件内容计算最短路径
    // 1、解析json文件内容
    // 2、更新路径点状态信息
    // 3、计算出最短路径后输出路径文件内容:out_path_content
    bool GetShortestPath(const std::string &json_content,std::string &out_path_content);
    bool GetShortestPath_file(const std::string &json_file,std::string &out_path_content);

    // 获取从from到to的最短路径
    std::vector<int> GetShortestPath(int from,int to);
private:
    PriveDate *d_ = nullptr;
};

#endif