#ifndef BEHAVIOR_UTIL_H
#define BEHAVIOR_UTIL_H

//C C++

//Qt
#include <QObject>
#include <QTableWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include <QComboBox>
#include <QString>
#include <QJsonObject>
#include <QMenu>
#include <QDebug>
#include <QAction>
#include <QWebEngineView>
#include <QUrl>
#include <QMessageBox>
#include <QJsonArray>
//obj
#include "behavior_creator_define.h"
#include "json_convert.h"
#include "behavior_util_export.h"

namespace behavior::util {
//添加按钮到表格的row行col列
template <typename T>
QPushButton *AddBtn2Table(QTableWidget *table, int row, int col,
                          void(T::*slot)(int row, int col), T *obj) {
  if (obj && slot) {
    auto f = std::bind(slot, obj, std::placeholders::_1, std::placeholders::_2);
    return AddBtn2Table(table, row, col, f);
  }
//   if (table && row < table->rowCount() && col < table->columnCount()) {
//     auto item = table->item(row, col);
//     if (item == nullptr) {
//       item = new QTableWidgetItem();
//       table->setItem(row, col, item);
//     }
//     auto cell = table->cellWidget(row, col);
//     QFrame *frame = nullptr;
//     if (cell == nullptr) {
//       frame = new QFrame();
//       auto layout = new QHBoxLayout(frame);
//       layout->setMargin(0);
//       layout->addStretch();
//       frame->setLayout(layout);
//       layout->setSpacing(4);
//     } else {
//       frame = dynamic_cast<QFrame *>(cell);
//     }
//     if (frame == nullptr) {
//       return nullptr;
//     }
//     QPushButton *btn = new QPushButton(frame);
//
//     QObject::connect(btn, &QPushButton::clicked, [=]() {
//       if (obj && slot) {
//         (obj->*slot)(table->row(item), table->column(item));
//       }
//     });
//     frame->layout()->addWidget(btn);
//     auto h = dynamic_cast<QHBoxLayout *>(frame->layout());
//     if (h) {
//       h->addStretch();
//     }
//     table->setCellWidget(row, col, frame);
//     return btn;
//   }
//   return nullptr;
  return nullptr;
}
//添加按钮到表格的row行col列
BEHAVIOR_UTIL_EXPORT QPushButton *AddBtn2Table(QTableWidget *table, int row,
    int col,
    std::function<void(int row, int col)> slot);
//添加下拉框到表格的row行col列
template <typename T>
QComboBox * AddComboBoxTable(QTableWidget *table, int row,
                             int col, void(T::*slot)(int row, int col, QString str), T *obj) {
  if (obj && slot) {
    auto f = std::bind(slot, obj, std::placeholders::_1, std::placeholders::_2,
                       std::placeholders::_3);
    return AddComboBoxTable(table, row, col, f);
  }
  return nullptr;
}
//添加下拉框到表格的row行col列
BEHAVIOR_UTIL_EXPORT QComboBox * AddComboBoxTable(QTableWidget *table, int row,
    int col, std::function<void(int row, int col, QString str)> slot);
//下拉列表居中
BEHAVIOR_UTIL_EXPORT void SetComboboxAlignment(QComboBox *com);

//生成map中不重复Name 和 DisplayName
//name和displayname均为in&out参数
template<typename M>
std::pair<std::string,std::string> GenerateNameDisplayName(
  const std::unordered_map<std::string, M> &map,
  std::string &name,std::string &display_name) {

  std::string temp_name = name;
  std::string temp_display = display_name;
  auto display_cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.display_name == temp_display;
  };

  auto name_cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.name == temp_name;
  };

  for (int i = 1;
       std::find_if(map.begin(), map.end(), display_cmp) != map.end()
       || std::find_if(map.begin(), map.end(), name_cmp) != map.end();
       i++) {
    temp_display = display_name + "_" + std::to_string(i);
    temp_name = name + "_" + std::to_string(i);
  }
  name = temp_name;
  display_name = temp_display;
  return std::make_pair(temp_name,temp_display);
}

//生成map中不重复DisplayName
template<typename M>
std::string GenerateDisplayName(const std::unordered_map<std::string, M> &map,
                                const std::string &name) {
  std::string display_name = name;

  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.display_name == display_name;
  };
  for (int i = 1; std::find_if(map.begin(), map.end(), cmp) != map.end(); i++) {
    display_name = name + "_" + std::to_string(i);
  }
  return display_name;
}

//生成map中不重复DisplayName
template<typename M>
std::string GenerateDisplayName(const std::map<std::string, M> &map,
                                const std::string &name) {
  std::string display_name = name;

  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.display_name == display_name;
  };
  for (int i = 1; std::find_if(map.begin(), map.end(), cmp) != map.end(); i++) {
    display_name = name + "_" + std::to_string(i);
  }
  return display_name;
}

//生成map中不重复DisplayName
template<typename M>
std::string GenerateDescription(const std::unordered_map<std::string, M> &map,
                                const std::string &name) {
  std::string description = name;

  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.description == description;
  };
  for (int i = 1; std::find_if(map.begin(), map.end(), cmp) != map.end(); i++) {
    description = name + "_" + std::to_string(i);
  }
  return description;
}

//生成map中不重复Name
template<typename M>
std::string GenerateName(const std::unordered_map<std::string, M> &map,
                         const std::string &name) {
  std::string new_name = name;

  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.name == new_name;
  };
  for (int i = 1; std::find_if(map.begin(), map.end(), cmp) != map.end(); i++) {
    new_name = name + "_" + std::to_string(i);
  }
  return new_name;
}
//生成vector中不重复Name
template<typename M>
std::string GenerateName(const std::vector<M> &vec,
                         const std::string &name) {
  std::string new_name = name;

  auto cmp = [&](const M &item) {
    return item.name == new_name;
  };
  for (int i = 1; std::find_if(vec.begin(), vec.end(), cmp) != vec.end(); i++) {
    new_name = name + "_" + std::to_string(i);
  }
  return new_name;
}
//检查map中是否以及存在此DisplayName(重名检测)
template<typename M>
bool IsDuplicationDisplayName(const std::unordered_map<std::string, M>
                              &map, const std::string &name) {
  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.display_name == name;
  };
  auto rv = std::find_if(map.begin(), map.end(), cmp) != map.end();
  if (rv) {
    ShowMessageBox(u8"提示", "显示名称重复");
  }
  return rv;
}
//检查map中是否以及存在此DisplayName(重名检测)
template<typename M>
bool IsDuplicationDisplayName(const std::map<std::string, M>
                              &map, const std::string &name) {
  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.display_name == name;
  };
  auto rv = std::find_if(map.begin(), map.end(), cmp) != map.end();
  if (rv) {
    ShowMessageBox(u8"提示", "名称重复");
  }
  return rv;
}

template<typename M>
bool IsDuplicationDescription(const std::unordered_map<std::string, M>
                              &map, const std::string &name) {
  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.description == name;
  };
  auto rv = std::find_if(map.begin(), map.end(), cmp) != map.end();
  if (rv) {
    ShowMessageBox(u8"提示", "名称重复");
  }
  return rv;
}

//检查map中是否以及存在此Name(重名检测)
template<typename M>
bool IsDuplicationName(const std::unordered_map<std::string, M> &map,
                       const std::string &name) {
  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.name == name;
  };
  auto rv = std::find_if(map.begin(), map.end(), cmp) != map.end();
  if (rv) {
    ShowMessageBox(u8"提示", "名称重复");
  }
  return rv;
}
//检查map中是否以及存在此Name(重名检测)
template<typename M>
bool IsDuplicationName(const std::map<std::string, M> &map,
                       const std::string &name) {
  auto cmp = [&](const std::pair<std::string, M> &item) {
    return item.second.name == name;
  };
  auto rv = std::find_if(map.begin(), map.end(), cmp) != map.end();
  if (rv) {
    ShowMessageBox(u8"提示", "名称重复");
  }
  return rv;
}
//中文名非法检查
BEHAVIOR_UTIL_EXPORT bool IsIllegalNameCN(const std::string &name);
//英文名非法检查
BEHAVIOR_UTIL_EXPORT bool IsIllegalNameEN(const std::string &name);

//显示弹窗
BEHAVIOR_UTIL_EXPORT int ShowMessageBox(const QString &title,
                                        const QString &text,
                                        const std::vector<int> &roles = { (int)QMessageBox::YesRole },
                                        int width = -1,  int hight = -1);
//显示弹窗
BEHAVIOR_UTIL_EXPORT int ShowMessageBox(const QString &title,
                                        const QString &text,
                                        const QStringList &btn_str);

//整理参数顺序 返回值为指针 参数不能为右值
//BEHAVIOR_UTIL_EXPORT
//std::vector<std::unordered_map<std::string,const Param *const>>
//    ArrangeOrder(std::unordered_map<std::string, Param> &origin);

//QStringList 转 std::set<std::string>
BEHAVIOR_UTIL_EXPORT std::set<std::string> QStringListToStdStringSet(
  const QStringList &list);
BEHAVIOR_UTIL_EXPORT std::vector<std::string> QStringListToStdStringVector(
  const QStringList &list);
//QStringList转不重复vector
BEHAVIOR_UTIL_EXPORT std::vector<std::string>
QStringListToStdStringUnRepetVector(
  const QStringList &list);
//std::set<std::string> 转 QStringList
BEHAVIOR_UTIL_EXPORT QStringList StdStringSetToQStringList(
  const std::set<std::string> &list);
//std::vector<std::string> 转 QStringList
BEHAVIOR_UTIL_EXPORT QStringList StdStringVectorToQStringList(
  const std::vector<std::string> &vec);
//生成后缀字符串
BEHAVIOR_UTIL_EXPORT void GenerateConsequentsStr(AtomicRule & atomic_rule);
//生成后缀字符串
BEHAVIOR_UTIL_EXPORT void GenerateConsequentsStr(AtomicRule & atomic_rule,
    const std::unordered_map<std::string, Param>& params);
//读取文件内容
BEHAVIOR_UTIL_EXPORT std::string file_contents(const std::string &filename);

//把map按DisplayName排序 返回map<display_name - id>
//template<typename M>
//std::map<std::string, std::string> CoverDisplayNameIDMap(
//  const std::unordered_map<std::string, M> &map) {
//  std::map<std::string, std::string> temp;
//  std::transform(map.begin(), map.end(), std::inserter(temp, temp.begin()),
//  [](std::pair<std::string, M> item) {
//    return std::make_pair(item.second.display_name, item.first);
//  });
//  return std::move(temp);
//}

//将map 按优先级排序 返回vector
//BEHAVIOR_UTIL_EXPORT std::vector< SingleRule *> CoverPriority(
//  std::unordered_map<std::string, SingleRule> &map);
////将map 按优先级排序 返回vector
//BEHAVIOR_UTIL_EXPORT std::vector< SubRuleSet *> CoverPriority(
//  std::unordered_map<std::string, SubRuleSet> &map);

//生成uuid
BEHAVIOR_UTIL_EXPORT std::string GenerateUUid();
//更新表格索引
BEHAVIOR_UTIL_EXPORT void UpdateTableIndex(QTableWidget *table);

//设置文本与提示文本
//类型T中必须包含setText 与 setToolTip
template<typename T>
void SetTextAndToolTip(T *item, const QString &text) {
  item->setText(text);
  item->setToolTip(text);
  item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

}

BEHAVIOR_UTIL_EXPORT void SetTextAndToolTip(QLabel *label, const QString &text);
BEHAVIOR_UTIL_EXPORT void SetTextAndToolTip(QTreeWidgetItem *item,int col,
    const QString &text);
BEHAVIOR_UTIL_EXPORT void SetTextAndToolTip(QLineEdit *line,
    const QString &text);

//关联自动ToolTip文本
BEHAVIOR_UTIL_EXPORT void SetAutoToolTip(QTableWidget *table);
//关联自动ToolTip文本
BEHAVIOR_UTIL_EXPORT void SetAutoToolTip(QLineEdit *line);
//关联自动ToolTip文本
BEHAVIOR_UTIL_EXPORT void SetAutoToolTip(QComboBox *cbox);
//关联自动ToolTip文本
BEHAVIOR_UTIL_EXPORT void SetAutoToolTip(QTreeWidget *tree);

//js函数 获取表达式
BEHAVIOR_UTIL_EXPORT QString JsGetExpression();



//参数类型映射到表达式支持的数据类型
//DataType GetExpressionTypeEnum(const std::string &type);
/*
* 表达式校验，返回成功或错误信息
* expr:表达式字符串
* param_table:<参数名称，参数类型>
*/
BEHAVIOR_UTIL_EXPORT std::vector<std::string> ExpressionValidation(
  std::string expr,
  std::map<std::string, std::string> param_table);

//map的关键字转json数组
template<typename T>
QJsonArray SequenceMapToJsonArray(const SequenceMap<std::string,T> &map) {
  QJsonArray temp;
  for (const auto &k:map.to_key_vector()) {
    temp.push_back(k.c_str());
  }
  return std::move(temp);
}

BEHAVIOR_UTIL_EXPORT QJsonArray StdMapToJsonArray(const
    std::unordered_map<std::string, behavior::Param> &map);

//控件添加快捷键
template<typename T,typename F>
void AddShortCut(T *obj, const QKeySequence &shortcut,F slot) {
  auto action = new QAction(obj);
  action->setShortcut(shortcut);
  QObject::connect(action, &QAction::triggered, [=]() {
    slot();
  });
  obj->addAction(action);
}

BEHAVIOR_UTIL_EXPORT QMenu *AddMenu();

BEHAVIOR_UTIL_EXPORT QJsonObject StringToQJson(const QString &str);
BEHAVIOR_UTIL_EXPORT QJsonObject CJsonToQJson(const nlohmann::json &json);
template<typename T>
QJsonObject StructToQJson(const T &struct_data) {
  nlohmann::json json;
  json = struct_data;
  return CJsonToQJson(json);
}
template<typename T>
QString StructToJsonString(const T &struct_data) {
  return QJsonToString(StructToQJson(struct_data));
}
BEHAVIOR_UTIL_EXPORT QString QJsonToString(const QJsonObject& json);


//字符串分割函数
BEHAVIOR_UTIL_EXPORT std::vector<std::string> split(std::string str,
    std::string pattern);


BEHAVIOR_UTIL_EXPORT void LoadCanvas(QWebEngineView *view,
                                     const std::string &url);
BEHAVIOR_UTIL_EXPORT void LoadCanvas(QWebEngineView *view, const QUrl &url);
BEHAVIOR_UTIL_EXPORT void AddWebPageShortCut(QWebEngineView *view);

//显示浮动提示框
BEHAVIOR_UTIL_EXPORT void ShowToolMessage(const QString &msg,
    QWidget *parent=nullptr);


BEHAVIOR_UTIL_EXPORT QString DisplayToExpression(const QVariant& data,
    std::unordered_map<std::string, behavior::Param> &params);
BEHAVIOR_UTIL_EXPORT QString DisplayExpression(const QVariant& data);

BEHAVIOR_UTIL_EXPORT std::wstring string2wstring(const std::string &str);
BEHAVIOR_UTIL_EXPORT std::string wstring2string(const std::wstring &str);


BEHAVIOR_UTIL_EXPORT std::string GetBasicType(const std::string &type);

//大字符串比较,采用MD5方法进行比较，若MD5相同，且precise ==true 则进行精确比较(逐字符比较)
BEHAVIOR_UTIL_EXPORT bool BigStringCompare(const QString &l,
    const QString &r, bool precise = false);

template <class T>
bool GetJsonData(const nlohmann::json& js, const std::string &key, T &data) {
  if (js.count(key)) {
    if (!js.at(key).empty()) {
      data = js[key];
      return true;
    }
  }
  return false;
}
};


#endif //BEHAVIOR_UTIL_H
