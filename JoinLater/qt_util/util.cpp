#include "util.h"

//C++
#include <fstream>
#include <locale>
#include <codecvt>
#include <unordered_set>
//Qt
#include <QRegExp>
#include<QFrame>
#include <QHBoxLayout>
#include <QUuid>
#include <QJsonDocument>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QLineEdit>
#include <QGroupBox>
#include <QDir>
#include <QListView>
#include <QCryptographicHash>
//obj
#include "custom_title.h"
#include "custom_messagebox.h"
#include"csexpression/CSExpression.h"
#include "util.h"
void QComboBox::wheelEvent(QWheelEvent *e) {
  //啥也不干，屏蔽鼠标滚动
}

namespace behavior::util {


QPushButton *AddBtn2Table(QTableWidget *table, int row, int col,
                          std::function<void(int row, int col)> slot) {
  if (table && row < table->rowCount() && col < table->columnCount()) {
    auto blocked = table->signalsBlocked();
    table->blockSignals(true);
    auto item = table->item(row, col);
    if (item == nullptr) {
      item = new QTableWidgetItem();
      table->setItem(row, col, item);
    }
    auto cell = table->cellWidget(row, col);
    QFrame *frame = nullptr;
    if (cell == nullptr) {
      frame = new QFrame();
      auto layout = new QHBoxLayout(frame);
      layout->setMargin(0);
      layout->addStretch();
      frame->setLayout(layout);
      layout->setSpacing(4);
    } else {
      frame = dynamic_cast<QFrame *>(cell);
    }
    if (frame == nullptr) {
      table->blockSignals(blocked);
      return nullptr;
    }
    QPushButton *btn = new QPushButton(frame);

    QObject::connect(btn, &QPushButton::clicked, [=]() {
      if (slot) {
        slot(table->row(item), table->column(item));
      }
    });
    frame->layout()->addWidget(btn);
    auto h = dynamic_cast<QHBoxLayout *>(frame->layout());
    if (h) {
      h->addStretch();
    }
    table->setCellWidget(row, col, frame);
    table->blockSignals(blocked);
    return btn;
  }
  return nullptr;
}


QComboBox * AddComboBoxTable(QTableWidget *table, int row,
                             int col, std::function<void(int row, int col, QString str)> slot) {
  if (table) {
    QComboBox *com = new QComboBox(table);
    //com->setView(new QListView);
    QTableWidgetItem *item = new QTableWidgetItem();
    table->setItem(row, col, item);
    QObject::connect(com, &QComboBox::currentTextChanged, [=](QString str) {
      if (slot) {
        slot(table->row(item), table->column(item), str);
      }
    });
    table->setCellWidget(row, col, com);
    com->setEditable(true);
    com->lineEdit()->setAlignment(Qt::AlignCenter);
    com->lineEdit()->setStyleSheet(R"(
	border: 0px solid #FFFFFF;
)");
    com->lineEdit()->setReadOnly(true);
	com->setContextMenuPolicy(Qt::NoContextMenu);
	auto view = new QListView();
	com->setView(view);
    com->lineEdit()->setReadOnly(true);
    //static_cast<QStandardItemModel*>(com->model())->item(pos)->setTextAlignment(
    //  Qt::AlignCenter);
    return com;
  }
  return nullptr;
}

bool IsIllegalNameCN(const std::string &name) {
  QString pat("^[\u4e00-\u9fa5_a-zA-Z0-9\(\)]+$");
  QRegExp rx(pat);
  auto rv = rx.exactMatch(QString::fromStdString(name));
  if (!rv) {
    std::string text = R"(名称必须非空且只能包含:
 - 中文汉字
 - 英文大小写字母
 - 数字
 - ( ) _

请输入有效名称.
)";
			ShowMessageBox(u8"提示", text.c_str(), { QMessageBox::YesRole }, 220, 150);
		}
		return !rv;
	}
	bool IsIllegalNameEN(const std::string &name)
	{
		QString pat("^[_a-zA-Z0-9\(\)]+$");
		QRegExp rx(pat);
		auto rv = rx.exactMatch(QString::fromStdString(name));
		if (!rv) {
			std::string text = R"(名称必须非空且只能包含:
 - 英文大小写字母
 - 数字
 - ( ) _

请输入有效名称.
)";
			ShowMessageBox(u8"提示", text.c_str(), { QMessageBox::YesRole }, 220, 150);
		}
		return !rv;
	}

	std::vector<std::unordered_map<std::string, const Param *const>>
		ArrangeOrder(std::unordered_map<std::string, Param> &origin) {
		std::vector<std::unordered_map<std::string, const  Param *const>> groups = { {},{},{},{} };
		for (auto &[param_name, param] : origin) {
			bool input = false;
			bool output = false;
			for (const auto&usage : param.usage) {
				if (usage == DataDirection::Input) {
					input = true;
				}
				else if (usage == DataDirection::Output) {
					output = true;
				}
			}
			int index = 0;
			if (input && output) {
				index = 1;
			}
			else if (input) {
				index = 0;
			}
			else if (output) {
				index = 2;
			}
			else {
				index = 3;
			}
			groups[index].emplace(param.name, &param);
		}
		return std::move(groups);
	}

	void GetParamDisplayValueString(const std::string & parent_name, const ParamValue &value, std::string &display_value_str) {
		const auto &current_name = parent_name + value.name;
		if (!value.value.empty()) {
			display_value_str += current_name;
			display_value_str += "=";
			display_value_str += value.display_value;
			display_value_str += ";\n";
		}
		for (const auto &[_, p] : value.struct_param)
		{
			GetParamDisplayValueString(current_name + ".", p, display_value_str);
		}
		for (const auto & p : value.array_param)
		{
			GetParamDisplayValueString(current_name, p, display_value_str);
		}
	}

	void GenerateConsequentsStr(AtomicRule & atomic_rule) {
		atomic_rule.consequents.clear();
		for (auto &[param_name, value] : atomic_rule.output.params) {
			GetParamDisplayValueString("", value, atomic_rule.consequents);
		}
		if (atomic_rule.consequents.size() >= 1)
		{
			//去除最后一个换行
			atomic_rule.consequents = atomic_rule.consequents.substr(0, atomic_rule.consequents.size() - 1);
		}
	}
	void GenerateConsequentsStr(AtomicRule & atomic_rule, const std::unordered_map<std::string, Param>& params)
	{
		atomic_rule.consequents.clear();
		for (const auto &[param_name, value] : atomic_rule.output.params.to_vector()) {
			GetParamDisplayValueString("",value, atomic_rule.consequents);
		}
		if (atomic_rule.consequents.size() >= 1)
		{
			//去除最后一个换行
			atomic_rule.consequents = atomic_rule.consequents.substr(0, atomic_rule.consequents.size() - 1);
		}
	}

	std::string file_contents(const std::string &filename)
	{
		std::ifstream ifs(string2wstring(filename), std::ios_base::binary);
		if (!ifs) {
			return "";
		}
		return std::string((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
	}

	std::set<std::string> QStringListToStdStringSet(const QStringList &list)
	{
		std::set<std::string> temp;
		for (auto s : list)
		{
			temp.emplace(s.toStdString());
		}
		return std::move(temp);
	}

	QStringList StdStringSetToQStringList(const std::set<std::string> &list)
	{
		QStringList temp;
		for (auto s : list)
		{
			temp.push_back(QString::fromStdString(s));
		}
		return std::move(temp);
	}
	QString BtnRoleToString(QMessageBox::ButtonRole role) {
		QString role_str;
		switch (role)
		{
		case QMessageBox::InvalidRole:
			role_str = "无效";
			break;
		case QMessageBox::AcceptRole:
			role_str = "接受";
			break;
		case QMessageBox::RejectRole:
			role_str = "拒绝";
			break;
			// 	case QMessageBox::DestructiveRole:
			// 		role_str = "毁坏";
			// 		break;
			// 	case QMessageBox::ActionRole:
			// 		role_str = "动作";
			// 		break;
		case QMessageBox::HelpRole:
			role_str = "帮助";
			break;
		case QMessageBox::YesRole:
			role_str = "确认";
			break;
		case QMessageBox::NoRole:
			role_str = "取消";
			break;
		case QMessageBox::ResetRole:
			role_str = "重置";
			break;
		case QMessageBox::ApplyRole:
			role_str = "应用";
			break;
		case QMessageBox::NRoles:
			role_str = "无";
			break;
		default:
			break;
		}
		return role_str;
	}
	int ShowMessageBox(const QString &title, const QString &text,
		const std::vector<int> &roles, int width, int hight)
	{

		const auto &id= typeid(QMessageBox::ButtonRole);
		const auto &cid = typeid(char);
		{
			auto c_box = new CustomMessageBox(title);
			c_box->SetText(text);
			for (const auto &role : roles)
			{
				c_box->AddButton(BtnRoleToString(QMessageBox::ButtonRole(role)), QMessageBox::ButtonRole(role));
			}
			return c_box->exec();
		}


		auto msg_box = new QMessageBox;
// 		{
// 
// 			auto grid_layout = dynamic_cast<QGridLayout *>(msg_box->layout());
// 			if (grid_layout)
// 			{
// 				auto sub_frame = new QFrame(msg_box);
// 				auto c_title = new CustomTitle(title);
// 				c_title->SetControlWidget(msg_box);
// 				c_title->setBackgroundColor("#1F334A");
// 
// 				auto label = new QLabel();
// 				label->setText(text);
// 				auto sub_vlayout = new QVBoxLayout();
// 				sub_frame->setLayout(sub_vlayout);
// 
// 				sub_vlayout->addWidget(c_title);
// 				sub_vlayout->addWidget(label);
// 
// 				auto vlayout = new QVBoxLayout(msg_box);
// 				vlayout->addWidget(sub_frame);
// 				//vlayout->addLayout(sub_vlayout);
// 				msg_box->setLayout(vlayout);
// 
// 				//grid_layout->addLayout(vlayout, 0, 0);
// 			}
// 		}
		msg_box->setWindowTitle(title);
		msg_box->setText(text);
		for (const auto &role : roles)
		{
			msg_box->addButton(BtnRoleToString(QMessageBox::ButtonRole(role)), QMessageBox::ButtonRole(role));
		}
		//msg_box->setIcon(":/behavior_creator/platform.ico");
		//msg_box->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		//msg_box->setAttribute(Qt::WA_TranslucentBackground, true);
		QString sheet = R"(
	*{
	background-color:#10131A;
	color:#FFFFFF;
	}
	QPushButton{
		width: 90px;
		height: 20px;
		border: 1px solid rgba(15, 187, 213, 1);	
		border-radius: 3px;
		color:rgba(16, 19, 26, 1);
		background-color:qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 183, 208, 1),stop:1 rgba(0, 224, 255, 1));
	}
)";
		if (width > 0 && hight > 0)
		{
			sheet += "QLabel{ min-width: " + QString::number(width) + "; min-height:" + QString::number(150) + ";}";
		}
		msg_box->setStyleSheet(sheet);
		msg_box->show();
		return msg_box->exec();
	}

	BEHAVIOR_UTIL_EXPORT int ShowMessageBox(const QString &title,const QString &text,const QStringList &btn_str)
	{
		auto c_box = new CustomMessageBox(title);
		c_box->SetText(text);
		for (const auto &role : btn_str)
		{
			c_box->AddButton(role, QMessageBox::NoRole);
		}
		return c_box->exec();
	}

	std::string GenerateUUid() {
		return std::move(QUuid::createUuid().toString(
			QUuid::WithoutBraces).toStdString());
	}

	void UpdateTableIndex(QTableWidget *table) {
		for (int row = 0; row < table->rowCount(); row++) {
			auto item = table->item(row, 0);
			if (item) {
				item->setText(QString::number(row + 1));
			}
		}
	}
	QString JsGetExpression() {
		return "QtGetExpression();";
	}

	enum class DataType {
		NUMERIC,
		STRING,
		NUMERIC_VECTOR,
		UNSUPPORTED

	};

	//参数类型映射到表达式支持的数据类型
	DataType GetExpressionTypeEnum(const std::string &type) {
		if (type == "bool" || type == "int8" || type == "uint8"
			|| type == "int16" || type == "uint16" || type == "int32"
			|| type == "uint32" || type == "int64" || type == "uint64"
			|| type == "float32" || type == "float64" || type == "float128") {
			return DataType::NUMERIC;

		}
		else if (type == "string") {
			return DataType::STRING;
		}
		else if (type == "bool[]" || type == "int8[]" || type == "uint8[]"
			|| type == "int16[]" || type == "uint16[]" || type == "int32[]"
			|| type == "uint32[]" || type == "int64[]" || type == "uint64[]"
			|| type == "float32[]" || type == "float64[]" || type == "float128[]") {
			return DataType::NUMERIC_VECTOR;
		}

		return DataType::UNSUPPORTED;
	}
	std::vector<std::string> ExpressionValidation(std::string expr,
		std::map<std::string, std::string> param_table) {
		std::map<std::string, double> numeric_table;
		std::map<std::string, std::string> string_table;
		std::map<std::string, std::vector<double>> num_vec_table;
		std::vector<std::string> result;
		for (auto &[name, type] : param_table) {
			switch (GetExpressionTypeEnum(type)) {
			case DataType::NUMERIC:
				numeric_table[name];
				break;
			case DataType::STRING:
				string_table[name];
				break;
			case DataType::NUMERIC_VECTOR:
				num_vec_table[name] = std::vector<double>(1);
				break;
			default:
				break;
			}
		}
		CSExpression expression;
		expression.SetExpString(expr);
		for (auto&[name, val] : numeric_table) {
			expression.AddVariable(name, val);
		}
		for (auto&[name, val] : string_table) {
			expression.AddString(name, val);
		}
		for (auto&[name, val] : num_vec_table) {
			expression.AddVector(name, val);
		}
		if (expression.Compile()) {
			auto x = expression.GetResultValue();
			qDebug()<<u8"表达式计算值:"<<x;
			result.push_back(u8"表达式校验通过！");
			return result;
		}
		else {
			auto errors = expression.GetCompileError();
			result.push_back(u8"表达式校验失败！是否继续保存?");
			for (auto & error : errors) {
				std::string temp;
				temp += u8"序号：" + std::to_string(error.index)
					+ u8",位置：" + std::to_string(error.pos)
					+ u8",类型：" + error.type
					+ u8" 详细信息：" + error.msg;
				result.push_back(temp);
			}
			return result;
		}
		return result;
	}

	QMenu *AddMenu()
	{
		QMenu *menu = new QMenu;
		menu->setAttribute(Qt::WA_TranslucentBackground);
		menu->setStyleSheet(R"(
QMenu{
	background-color: rgba(31, 51, 74, 1);
	border: 1px solid rgba(31, 51, 74, 1);	
	border-radius:2px;
	padding-top:4px;
	padding-bottom:4px;
}
QMenu::item{
	color:rgba(255, 255, 255, 1);
	font-size:14px;
	padding:8px 24px;
}
QMenu::item:hover{
  background-color:#354c66;
}
QMenu::item:selected{
  background-color:#354c66;
}


)");
		return menu;
	}

	

	QJsonObject StringToQJson(const QString &str)
	{
		QJsonParseError result;
		QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &result);
		if (result.error == QJsonParseError::NoError)
		{
			if (doc.isObject())
			{
				return doc.object();
			}
		}
		else
		{
			qDebug() << result.error;
		}
		return QJsonObject();
	}
	QJsonObject CJsonToQJson(const nlohmann::json &json)
	{
		return StringToQJson(json.dump().c_str());
	}
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

	QString QJsonToString(const QJsonObject& json)
	{
		return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
	}

	void SetAutoToolTip(QTableWidget *table)
	{
		QObject::connect(table, &QTableWidget::itemChanged, [=](QTableWidgetItem *item) {
			if (item) {
				auto blocked = table->signalsBlocked();
				table->blockSignals(true);
				item->setToolTip(item->text());
				table->blockSignals(blocked);
			}
		});
	}

	void SetAutoToolTip(QLineEdit *line)
	{
		QObject::connect(line, &QLineEdit::textChanged, [=](QString text) {
			if (line) {
				auto blocked = line->signalsBlocked();
				line->blockSignals(true);
				line->setToolTip(text);
				line->blockSignals(blocked);
			}
		});
	}

	void SetAutoToolTip(QComboBox *cbox)
	{
		QObject::connect(cbox, &QComboBox::currentTextChanged, [=](QString text) {
			if (cbox) {
				auto blocked = cbox->signalsBlocked();
				cbox->blockSignals(true);
				cbox->setToolTip(text);
				cbox->blockSignals(blocked);
			}
		});
	}

	void SetAutoToolTip(QTreeWidget *tree)
	{
		QObject::connect(tree, &QTreeWidget::itemChanged, [=](QTreeWidgetItem *item,int col) {
			if (tree) {
				auto blocked = tree->signalsBlocked();
				tree->blockSignals(true);
				item->setToolTip(col,item->text(col));
				tree->blockSignals(blocked);
			}
		});
	}

	void LoadCanvas(QWebEngineView *view, const std::string &url) {
		LoadCanvas(view, QUrl(url.c_str()));
	}

	void LoadCanvas(QWebEngineView *view, const QUrl &url) {
		view->load(url);
		view->show();
		view->setAttribute(Qt::WA_DeleteOnClose);
		view->setStyleSheet("background-color:rgba(11, 18, 40, 1);");
	}

	void AddWebPageShortCut(QWebEngineView *view)
	{
		AddShortCut(view, Qt::Key_F11, [view]() {
			auto map = new QWebEngineView;
			view->page()->setDevToolsPage(map->page());
			view->page()->triggerAction(QWebEnginePage::InspectElement);
			//map->setAttribute(Qt::WA_DeleteOnClose, true);

			auto debug = new QWidget();
			//static std::shared_ptr<QWidget> debug;
			//debug = std::make_shared<QWidget>();
			debug->setWindowTitle(view->page()->url().toString());
			debug->setLayout(new QHBoxLayout);
			debug->layout()->addWidget(map);
			debug->setAttribute(Qt::WA_DeleteOnClose, true);
			debug->show();
		});
		AddShortCut(view, Qt::Key_F5, [view]() {
			view->reload();
		});
		AddShortCut(view, Qt::CTRL + Qt::Key_F5, [view]() {
			auto old = view->url();
			auto profile = view->page()->profile();
			profile->clearHttpCache();
			//auto cookie = profile->cookieStore();
			//cookie->deleteAllCookies();
			//QString cachePath = profile->cachePath();
			//QDir cachePathDir(cachePath);
			//if (cachePathDir.exists())
			//{
			//	bool rlt = cachePathDir.rmdir(cachePath); //删除缓存目录
			//	if (!rlt)
			//	{
			//		qDebug() << "删除缓存目录失败！";
			//	}
			//}
			qDebug() << "重新加载链接:" << old;
			LoadCanvas(view, old);
		});
	}
	void ShowToolMessage(const QString &msg, QWidget *parent)
	{
		if (parent)
		{
			//static int tool_id = 0;
			auto tool = new QFrame(parent);
			auto background_widget = new QFrame(tool);
			auto label = new QLabel(background_widget);
			background_widget->setContentsMargins(0, 0, 0, 0);
			auto b_layout = new QHBoxLayout(background_widget);
			background_widget->setLayout(b_layout);
			b_layout->addWidget(label);
			b_layout->setContentsMargins(0, 0, 0, 0);
			QString obj_name = "tool_background_frame"/* + QString::number(tool_id++)*/;
			background_widget->setObjectName(obj_name);
			//  设置背景
			tool->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
			tool->setAttribute(Qt::WA_TranslucentBackground);
			tool->setAttribute(Qt::WA_QuitOnClose);
			tool->setAttribute(Qt::WA_DeleteOnClose);
			label->setAlignment(Qt::AlignCenter);
			//label->setAlignment(Qt::AlignVCenter);
			//label->setAlignment(Qt::AlignHCenter);
			label->setText(msg);
			label->adjustSize();
			background_widget->setStyleSheet("#"+obj_name+R"({
 	background-color: rgba(31, 51, 74, 1);
 	border: 1px solid rgba(31, 51, 74, 1);	
 	border-radius:4px;
 	padding:0px 0px 0px 0px;/**/
 	font-size:14px;
 	color:rgba(195, 248, 255, 1);
 }
 )");

    auto time = new QTimer(tool);
// 			time->singleShot()
// 	QObject::connect(time,&QTimer::sing)
    time->singleShot(1000, [tool]() {
      QPropertyAnimation *animation =
        new QPropertyAnimation(tool, "windowOpacity", tool);
      animation->setDuration(1000);
      animation->setStartValue(1);
      animation->setEndValue(0);
      animation->start();
      QObject::connect(animation, &QPropertyAnimation::finished, [tool]() {
        tool->close();
      });
    });

    auto msg_size = background_widget->size();
    background_widget->resize(label->width()+32, background_widget->height());
    tool->resize(background_widget->width(), background_widget->height() );

    //计算相对父窗口位置
    double x_rate = 0.5;
    double y_rate = 0.65;
    auto parent_rect = parent->geometry();
    auto x = (parent_rect.width() - tool->geometry().width()) * x_rate;
    auto y = parent_rect.height()*y_rate;
    tool->move(x, y);
    tool->show();
  }
}




void SetComboboxAlignment(QComboBox *com) {
  if (com) {
    auto model = static_cast<QStandardItemModel*> (com->model());
    if (model) {
      for (int i = 0; i < model->rowCount(); i++) {
        if (model->item(i))
          model->item(i)->setTextAlignment(Qt::AlignCenter);
      }
    }
  }
}

QJsonArray StdMapToJsonArray(const
                             std::unordered_map<std::string, behavior::Param> &map) {
  QJsonArray temp;
  for (const auto &[k, v] : map) {
    temp.push_back(v.display_name.c_str());
  }
  return std::move(temp);
}


QString DisplayToExpression(const QVariant & data,
                            std::unordered_map<std::string, behavior::Param>& params) {
  auto list = data.toList();
  QString expr;
  bool flag;
  for (const auto & d : list) {
    auto qp = d.toString().split(".");
    if (qp.empty()) {
      continue;
    }
    auto p = qp[0].toStdString();
    std::string sub_p;
    for (int i=1; i<qp.size(); ++i) {
      sub_p += "." + qp[i].toStdString();
    }
    flag = false;
    for (auto&[name, param] : params) {
      if (param.display_name == p) {
        expr += QString(name.c_str());
        if (!sub_p.empty()) {
          expr = expr + sub_p.c_str();
        }
        flag = true;
        break;
      }
    }
    if (!flag) {
      expr += d.toString();
    }
  }
  return expr;
}

QString DisplayExpression(const QVariant & data) {
  auto list = data.toList();
  QString expr(u8"");
  for (const auto & d : list) {
    auto qp = d.toString();
    expr += qp;
  }
  return expr;
}

void SetTextAndToolTip(QLabel *label, const QString &text) {
  label->setText(text);
  label->setToolTip(text);
}

BEHAVIOR_UTIL_EXPORT void SetTextAndToolTip(QLineEdit *line,
    const QString &text) {
  line->setText(text);
  line->setToolTip(text);
}

BEHAVIOR_UTIL_EXPORT void SetTextAndToolTip(QTreeWidgetItem *item, int col,
    const QString &text) {
  item->setText(col,text);
  item->setToolTip(col, text);
  item->setTextAlignment(col, Qt::AlignHCenter | Qt::AlignVCenter);
}

BEHAVIOR_UTIL_EXPORT std::wstring string2wstring(const std::string &str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(str);
}

BEHAVIOR_UTIL_EXPORT std::string wstring2string(const std::wstring &str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(str);
}

BEHAVIOR_UTIL_EXPORT QStringList StdStringVectorToQStringList(
  const std::vector<std::string> &vec) {
  QStringList list;
  for (const auto &v:vec) {
    list.push_back(QString::fromStdString(v));
  }
  return list;
}

BEHAVIOR_UTIL_EXPORT std::vector<std::string> QStringListToStdStringVector(
  const QStringList &list) {
  std::vector<std::string> vec;
  for (const auto &v : list) {
    vec.push_back(v.toStdString());
  }
  return vec;
}

BEHAVIOR_UTIL_EXPORT std::vector<std::string>
QStringListToStdStringUnRepetVector(
  const QStringList &list) {
  std::vector<std::string> vec;
  std::unordered_set<std::string> set;
  for (const auto &v : list) {
    if (set.emplace(v.toStdString()).second) {
      vec.push_back(v.toStdString());
    }
  }
  return vec;
}

BEHAVIOR_UTIL_EXPORT std::string GetBasicType(const std::string &type) {
  auto basic_type = type;
  if (auto it = type.rfind("[]"); it != type.npos) {
    basic_type = type.substr(0, it);
  }
  return basic_type;
}

BEHAVIOR_UTIL_EXPORT bool BigStringCompare(const QString &l, const QString &r,
    bool precise) {
  if (l.size() != r.size()) {
    return false;
  }
  if (precise == true) {
    if (QCryptographicHash::hash(l.toUtf8(), QCryptographicHash::Md5)
        != QCryptographicHash::hash(r.toUtf8(), QCryptographicHash::Md5)) {
      return false;
    }
    return l == r;
  } else {
    return QCryptographicHash::hash(l.toUtf8(), QCryptographicHash::Md5)
           == QCryptographicHash::hash(r.toUtf8(), QCryptographicHash::Md5);
  }
}

};
