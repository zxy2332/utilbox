#include "json_convert.h"

//C++

//Qt

//obj

using json = nlohmann::json;

#define VarName(v) std::string(#v)
#define VarType(v) std::string(typeid(v).name())

//字符串分割函数
std::vector<std::string> split(std::string str, std::string pattern) {
  std::string::size_type pos;
  std::vector<std::string> result;
  str += pattern;//扩展字符串以方便操作
  int size = str.size();

  for (int i = 0; i < size; i++) {
    pos = str.find(pattern, i);
    if (pos < size) {
      std::string s = str.substr(i, pos - i);
      result.push_back(s);
      i = pos + pattern.size() - 1;
    }
  }
  return std::move(result);
}
#define  JsonValueToStructParam(j,x) {						\
  auto s = split(VarName(x), ".");			\
  if (s.size() > 0) {										\
    if (j.count(s[s.size() - 1])) {							\
      if (!j.at(s[s.size() - 1]).empty())					\
        j.at(s[s.size() - 1]).get_to(x);					\
    }														\
  }															\
}
#define  StructParamToJsonValue(j,x) {						\
  auto s = split(VarName(x), ".");			\
  if (s.size() > 0) {										\
	j[s[s.size() - 1]] = x;									\
  }															\
}

namespace behavior {

namespace util {

QString DirectionToStr(const DataDirection &direction) {
  if (direction == DataDirection::Input) {
    return u8"输入";
  } else if (direction == DataDirection::Output) {
    return u8"输出";
  } else if (direction == DataDirection::Intermediate) {
    return u8"中间变量";
  } else {
    return "";
  }
}


DataDirection StrToDirection(const QString &str) {
  if (u8"输入" == str) {
    return DataDirection::Input;
  } else if (u8"输出" == str) {
    return DataDirection::Output;
  } else if (u8"中间变量" == str) {
    return DataDirection::Intermediate;
  } else {
    return DataDirection::Undefine;
  }
}


std::string DirectionToStrEN(const DataDirection &direction) {
  if (direction == DataDirection::Input) {
    return u8"input";
  } else if (direction == DataDirection::Output) {
    return u8"output";
  } else if (direction == DataDirection::Intermediate) {
    return u8"intermediate";
  } else if (direction == DataDirection::Init) {
    return u8"init";
  } else {
    return "";
  }
}

QString DirectVectorToStr(const std::vector<DataDirection> &direct) {
  QString data = "";
  for (const auto &dir : direct) {
    data += util::DirectionToStr(dir) + ",";
  }
  if (data.endsWith(",")) {
    data.remove(data.count() - 1, 1);
  }
  return data;
}

std::string DirectVectorToStrEN(const std::vector<DataDirection> &direct) {
  std::string temp;
  for (const auto &d : direct) {
    temp += util::DirectionToStrEN(d) + ",";
  }

  return temp.substr(0, temp.size() - 1);
}

std::vector<behavior::DataDirection> StrToDirectVector(const QString &direct) {
  std::vector<behavior::DataDirection> temp;
  if (direct.indexOf(u8"输入") != -1) {
    temp.push_back(DataDirection::Input);
  }
  if (direct.indexOf(u8"输出") != -1) {
    temp.push_back(DataDirection::Output);
  }
  if (direct.indexOf(u8"中间变量") != -1) {
    temp.push_back(DataDirection::Intermediate);
  }
  if (direct.indexOf(u8"初始化") != -1) {
    temp.push_back(DataDirection::Init);
  }
  return std::move(temp);
}

std::vector<behavior::DataDirection> StrToDirectVectorEN(
  const std::string &direct) {
  std::vector<behavior::DataDirection> temp;
  if (direct.find(u8"input") != direct.npos) {
    temp.push_back(DataDirection::Input);
  }
  if (direct.find(u8"output") != direct.npos) {
    temp.push_back(DataDirection::Output);
  }
  if (direct.find(u8"intermediate") != direct.npos) {
    temp.push_back(DataDirection::Intermediate);
  }
  if (direct.find(u8"init") != direct.npos) {
    temp.push_back(DataDirection::Init);
  }
  return std::move(temp);
}


};

void from_json(const nlohmann::json& j, Param& p) {
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.type);
  JsonValueToStructParam(j, p.value);
  JsonValueToStructParam(j, p.display_name);
}

void from_json(const nlohmann::json& j,
               SequenceMap<std::string, Param>& p) {
  for (auto js : j) {
    Param pa = js;
    p.emplace(pa.name, pa);
  }
}

void from_json(const nlohmann::json& j, AtomicRule& p) {
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.description);
  JsonValueToStructParam(j, p.antecedents);
  JsonValueToStructParam(j, p.consequents);
  JsonValueToStructParam(j, p.output);
  JsonValueToStructParam(j, p.is_switch);
  JsonValueToStructParam(j, p.display_antecedents);
}

void from_json(const nlohmann::json& j, Rules& p) {
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.atomic_rule);
}

void from_json(const nlohmann::json& j, SingleRule& p) {
  JsonValueToStructParam(j, p.id);
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.display_name);
  JsonValueToStructParam(j, p.description);
  JsonValueToStructParam(j, p.rules);
  //JsonValueToStructParam(j, p.param);
  JsonValueToStructParam(j, p.intermediate);
}

void from_json(const nlohmann::json& j, SubRuleSet& p) {
  JsonValueToStructParam(j, p.id);
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.display_name);
  JsonValueToStructParam(j, p.description);
  JsonValueToStructParam(j, p.single_rule);
}

void from_json(const nlohmann::json& j, RuleSet& p) {
  JsonValueToStructParam(j, p.id);
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.display_name);
  JsonValueToStructParam(j, p.description);
  JsonValueToStructParam(j, p.sub_rule_set);
  JsonValueToStructParam(j, p.params);
  JsonValueToStructParam(j, p.create_time);
  JsonValueToStructParam(j, p.update_time);
}

void from_json(const nlohmann::json& j, FlowStruct& p) {
  JsonValueToStructParam(j, p.id);
  JsonValueToStructParam(j, p.display_name);
  JsonValueToStructParam(j, p.flow_diagram_json);
  JsonValueToStructParam(j, p.flow_diagram_xml);

}

void from_json(const nlohmann::json& j, FlowDiagram& p) {

}

void from_json(const nlohmann::json& j, ComposeRuleInfo& p) {

}

void from_json(const nlohmann::json & j, GraphicAtomicRule & p) {
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.description);
  JsonValueToStructParam(j, p.antecedents);
  JsonValueToStructParam(j, p.consequents);
  JsonValueToStructParam(j, p.output);
  JsonValueToStructParam(j, p.is_switch);
  JsonValueToStructParam(j, p.display_antecedents);
  JsonValueToStructParam(j, p.id);
  JsonValueToStructParam(j, p.next);
}


BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, ParamValue& p) {
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.name);
  JsonValueToStructParam(j, p.type);
  JsonValueToStructParam(j, p.value);
  JsonValueToStructParam(j, p.display_value);
  JsonValueToStructParam(j, p.struct_param);
  JsonValueToStructParam(j, p.array_param);
}

BEHAVIOR_UTIL_EXPORT void from_json(const nlohmann::json& j, ParamValueMap& p) {
  p = j;
}


void to_json(nlohmann::json& j, const Param& p) {
  StructParamToJsonValue(j, p.name);
  StructParamToJsonValue(j, p.type);
  StructParamToJsonValue(j, p.value);
  StructParamToJsonValue(j, p.display_name);
  j["usage"] = util::DirectVectorToStr(p.usage).toStdString();
}

void to_json(nlohmann::json& j, const AtomicRule& p) {
  auto new_p = p;
  //std::swap(new_p.antecedents, new_p.display_antecedents);
  //std::swap(new_p.output, new_p.display_output);
  StructParamToJsonValue(j, new_p.name);
  StructParamToJsonValue(j, new_p.description);
  StructParamToJsonValue(j, new_p.antecedents);
  StructParamToJsonValue(j, new_p.display_antecedents);
  StructParamToJsonValue(j, new_p.consequents);
  StructParamToJsonValue(j, new_p.output);
  StructParamToJsonValue(j, new_p.is_switch);
}

void to_json(nlohmann::json & j, const Rules & p) {
  //StructParamToJsonValue(j, p.name);
  StructParamToJsonValue(j, p.atomic_rule);
  //j["atomic_rule"].push_back(p.default_atomic);
  /*nlohmann::json da;
  da["description"] = p.default_atomic.description;
  da["name"] = p.default_atomic.name;
  da["output"] = p.default_atomic.output;
  da["is_switch"] = p.default_atomic.is_switch;
  j["atomic_rule"].push_back(da);*/

}

void to_json(nlohmann::json & j, const SingleRule & p) {
  StructParamToJsonValue(j, p.id);
  StructParamToJsonValue(j, p.display_name);
  nlohmann::json details;
  for (const auto &param : p.param) {

  }
  j["inputs"] = nlohmann::json::array();
  j["outputs"] = nlohmann::json::array();
  j["temporary"] = nlohmann::json::array();
  for (const auto &[_, temporary] : p.intermediate) {

    nlohmann::json temp_json;
    StructParamToJsonValue(temp_json, temporary.name);
    StructParamToJsonValue(temp_json, temporary.type);
    StructParamToJsonValue(temp_json, temporary.value);
    StructParamToJsonValue(temp_json, temporary.display_value);
    j["temporary"].push_back(temp_json);
  }
  //原子规则表
  for (const auto &rule : p.rules.front().atomic_rule) {
    nlohmann::json rule_json;

    rule_json["antecedents"] = rule.antecedents;
    rule_json["display_antecedents"] = rule.description;
    rule_json["consequents"] = nlohmann::json::array();
    for (const auto &[o_name, o_value] : rule.output.params.to_vector()) {
      nlohmann::json con_json;
      con_json["name"] = o_name;
      //TODO:待修改：查找display_name
      con_json["display_name"] = o_name;
      con_json["value"] = o_value.value;
      con_json["display_value"] = o_value.display_value;
      rule_json["consequents"].push_back(con_json);
    }
    j["details"].push_back(rule_json);
  }
}

void to_json(nlohmann::json & j, const SubRuleSet & p) {
  StructParamToJsonValue(j, p.id);
  StructParamToJsonValue(j, p.display_name);
  StructParamToJsonValue(j, p.single_rule);
}

void to_json(nlohmann::json & j, const RuleSet & p) {
  StructParamToJsonValue(j, p.id);
  StructParamToJsonValue(j, p.display_name);
  StructParamToJsonValue(j, p.sub_rule_set);
  // StructParamToJsonValue(j, p.params);
}

void to_json(nlohmann::json & j, const FlowStruct & p) {
  StructParamToJsonValue(j, p.id);
  //StructParamToJsonValue(j, p.name);
  StructParamToJsonValue(j, p.display_name);
  //StructParamToJsonValue(j, p.description);
  //StructParamToJsonValue(j, p.relevance_ruleset);
  //StructParamToJsonValue(j, p.params);
  StructParamToJsonValue(j, p.flow_diagram_json);
  //StructParamToJsonValue(j, p.flow_diagram_xml);
  //StructParamToJsonValue(j, p.rule_info);
  //StructParamToJsonValue(j, p.flow_diagram);
}

void to_json(nlohmann::json & j, const FlowDiagram & p) {

}

void to_json(nlohmann::json & j, const ComposeRuleInfo & p) {
  StructParamToJsonValue(j, p.id);
  StructParamToJsonValue(j, p.display_name);
  StructParamToJsonValue(j, p.description);
}

void to_json(nlohmann::json& j, const SingleRule& p,
             const SequenceMap<std::string, Param>& param) {
  StructParamToJsonValue(j, p.rules);
  StructParamToJsonValue(j, param);
  StructParamToJsonValue(j, p.intermediate);
}

BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j,
                                  const UserDataType::Param& p) {
  StructParamToJsonValue(j, p.name);
  StructParamToJsonValue(j, p.type);
  StructParamToJsonValue(j, p.displayName);
  StructParamToJsonValue(j, p.complex_type);
}

BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const UserDataType& p) {
  StructParamToJsonValue(j, p.name);
  StructParamToJsonValue(j, p.params);
  StructParamToJsonValue(j, p.storage_format_type);
}

BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const ParamValue& p) {
  StructParamToJsonValue(j, p.name);
  StructParamToJsonValue(j, p.value);
  StructParamToJsonValue(j, p.type);
  StructParamToJsonValue(j, p.display_value);
  StructParamToJsonValue(j, p.struct_param);
  StructParamToJsonValue(j, p.array_param);
}

BEHAVIOR_UTIL_EXPORT void to_json(nlohmann::json& j, const ParamValueMap& p) {
  j = p.params;
}

};
