#ifndef JSON_CONVERT_H
#define JSON_CONVERT_H

//C++
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <QString>
#include "qnamespace.h"
#include "json/json.hpp"
#include "behavior_creator_define.h"
#include "behavior_util_export.h"
//Qt

namespace behavior {
namespace util {
//流向枚举转字符串
BEHAVIOR_UTIL_EXPORT QString DirectionToStr(const DataDirection &direction);
BEHAVIOR_UTIL_EXPORT std::string DirectionToStrEN(const DataDirection
    &direction);
//字符串转流向枚举
BEHAVIOR_UTIL_EXPORT behavior::DataDirection StrToDirection(const QString &str);
//流向枚举数组转字符串
BEHAVIOR_UTIL_EXPORT QString DirectVectorToStr(const std::vector<DataDirection>
    &direct);
BEHAVIOR_UTIL_EXPORT std::string DirectVectorToStrEN(const
    std::vector<DataDirection> &direct);
//字符串转流向枚举数组
BEHAVIOR_UTIL_EXPORT std::vector<DataDirection> StrToDirectVector(
  const QString &direct);
BEHAVIOR_UTIL_EXPORT std::vector<DataDirection> StrToDirectVectorEN(
  const std::string &direct);

};

BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, Param& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j,
                                    SequenceMap<std::string, Param>& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, AtomicRule& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, Rules& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, SingleRule& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, SubRuleSet& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, RuleSet& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, FlowStruct& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, FlowDiagram& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j,
                                    ComposeRuleInfo& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j,
                                    GraphicAtomicRule& p);

BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j,
                                    ParamValue& p);
BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j,
                                    ParamValueMap& p);


template<typename T>
void to_json(nlohmann::json& j,
             const SequenceMap<std::string, T>& p) {
  if (p.empty()) {
    j = nlohmann::json::array();
  } else {
    for (const auto &[key, value] : p.to_vector()) {
      j.push_back(value);
    }
  }
}
template<typename T>
void to_json(nlohmann::json& j,
             const std::unordered_map<std::string, T>& p) {
  if (p.empty()) {
    j = nlohmann::json::array();
  } else {
    for (const auto &[key, value] : p) {
      j.push_back(value);
    }
  }
}
template<typename T>
void to_json(nlohmann::json& j,
             const std::map<std::string, T>& p) {
  if (p.empty()) {
    j = nlohmann::json::array();
  } else {
    for (const auto &[key, value] : p) {
      j.push_back(value);
    }
  }
}
template<typename T>
void to_json(nlohmann::json& j,
             const std::vector<T>& p) {
  if (p.empty()) {
    j = nlohmann::json::array();
  } else {
    for (const auto &value : p) {
      j.push_back(value);
    }
  }
}
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const Param& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const AtomicRule& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const Rules& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const SingleRule& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const SubRuleSet& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const RuleSet& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const FlowStruct& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const FlowDiagram& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const ComposeRuleInfo& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const ParamValue& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const ParamValueMap& p);

BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const SingleRule& p,
                                  const SequenceMap<std::string, Param>& param);

BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j,
                                  const UserDataType::Param& p);
BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const UserDataType& p);

};


#endif //JSON_CONVERT_H
