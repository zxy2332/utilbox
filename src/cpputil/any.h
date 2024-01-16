#pragma once

#include <typeindex>
#include <memory>
#include <unordered_map>



namespace zxy::util {
	class any {
	private:
		class BaseObj {
		public:			
			virtual ~BaseObj() {}
			virtual std::unique_ptr<BaseObj> Clone()const = 0;
		};

		template<typename T>
		class TypeObj :public BaseObj {
		public:
			TypeObj() :v(T()), type(typeid(T)) {

			}
			TypeObj(T t) :v(std::forward<T>(t)), type(typeid(T)) {

			}
			T GetValue() { 
				return v;
			}

			virtual std::unique_ptr<BaseObj> Clone() const override {
				return std::unique_ptr<BaseObj>(new TypeObj<T>(v));
			}
		private:
			std::type_index type;
			T v;
		};
	public:
        any() :base_ptr() {}
		//拷贝构造函数
        any(const any &t) :base_ptr(t.Clone()) {
		}
        any(any &t) :base_ptr(t.Clone()) {
		}
		//移动构造函数
        any(any &&v) :base_ptr(std::move(v.base_ptr)) {
		}
		//自身"="重载
        any& operator=(const any &t) {
			base_ptr = t.Clone();
			return *this;
		}

		//创建智能指针时，对于一般的类型，通过std::decay来移除引用和cv符，从而获取原始类型
		//template<typename ValueType, class = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, Any>::value, ValueType>::type>
		//Any(ValueType  &&value) : base_ptr(new TypeObj <typename std::decay<ValueType>::type>(std::forward<ValueType>(value))) {
		//
		//}
		//template<typename ValueType, class = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, Any>::value, ValueType>::type>
		//Any(ValueType  &&value) : base_ptr(std::unique_ptr<BaseObj>(new TypeObj<ValueType>(value))) {
		//
		//}


		template<typename ValueType, class = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, any>::value, ValueType>::type>
        any(ValueType  &&value) : base_ptr(new TypeObj <typename std::decay<ValueType>::type>(std::forward<ValueType>(value))) {

		}
		// 其他类型"="的重载
		template<typename ValueType>
        any& operator=(const ValueType &t) {
			*this = AnyValue(t);
			return *this;
		}
		//获取指定类型的值
		template<typename ValueType>
		static ValueType any_cast(any value) {
			if (value.base_ptr) {
				auto value_ptr = dynamic_cast<TypeObj<ValueType> *>(value.base_ptr.get());
				if (value_ptr) {
					return value_ptr->GetValue();
				}
			}
			return ValueType();
		}
		//获取指定类型的值
		template<typename ValueType>
		void GetTo(ValueType &to) {
			if (this->base_ptr) {
				auto value_ptr = dynamic_cast<TypeObj<ValueType> *>(this->base_ptr.get());
				if (value_ptr) {
					to = value_ptr->GetValue();
				}
			}
			to = ValueType();
		}

		bool IsEmpty() {
			return !base_ptr.get();
		}
	private:
		std::unique_ptr<BaseObj> Clone() const
		{
			if (base_ptr) {
				return base_ptr->Clone();
			}
			//return std::unique_ptr<BaseObj>();
			return nullptr;
		}

	private:
		std::unique_ptr<BaseObj> base_ptr;

	};
};
