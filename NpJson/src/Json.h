#ifndef __JSON_H__
#define __JSON_H__

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdexcept>

class Json // Implemented according to: http://www.json.org/
{
public:
	enum JsonType {
		TYPE_NULL, TYPE_BOOL, TYPE_NUMBER,
		TYPE_STRING, TYPE_ARRAY, TYPE_OBJECT
	};

	explicit Json(JsonType type = TYPE_NULL, const std::string& text = "");
	explicit Json(bool v, JsonType type = TYPE_BOOL);
	explicit Json(int8_t v, JsonType type = TYPE_NUMBER);
	explicit Json(int16_t v, JsonType type = TYPE_NUMBER);
	explicit Json(int32_t v, JsonType type = TYPE_NUMBER);
	explicit Json(int64_t v, JsonType type = TYPE_NUMBER);
	explicit Json(uint8_t v, JsonType type = TYPE_NUMBER);
	explicit Json(uint16_t v, JsonType type = TYPE_NUMBER);
	explicit Json(uint32_t v, JsonType type = TYPE_NUMBER);
	explicit Json(uint64_t v, JsonType type = TYPE_NUMBER);
	explicit Json(float v, JsonType type = TYPE_NUMBER);
	explicit Json(double v, JsonType type = TYPE_NUMBER);
	explicit Json(const char* v, JsonType type = TYPE_STRING);
	explicit Json(const std::string& v, JsonType type = TYPE_STRING);
	Json(const Json& j);

	static const Json& Null(); // enable to generate a 'const Json&' null object
	bool EqualTo(const Json& j) const;

	Json& Sub(size_t index);
	Json& Sub(const std::string& name);
	const Json& Sub(size_t index) const;
	const Json& Sub(const std::string& name) const;

	void Clone(const Json& j);
	void Clear(JsonType type = TYPE_NULL, const std::string& text = "");
	void Insert(const Json& value, size_t before = ~(size_t)0);
	void Insert(const std::string& key, const Json& value, size_t before = ~(size_t)0);
	void Move(size_t index, size_t before = ~(size_t)0);
	void Move(const std::string& key, size_t before = ~(size_t)0);
	void Erase(size_t pos);
	void Erase(const std::string& name);

	bool operator == (const Json& j) const { return EqualTo(j); }
	bool operator != (const Json& j) const { return !EqualTo(j); }

	Json& operator = (bool v);
	Json& operator = (int8_t v);
	Json& operator = (int16_t v);
	Json& operator = (int32_t v);
	Json& operator = (int64_t v);
	Json& operator = (uint8_t v);
	Json& operator = (uint16_t v);
	Json& operator = (uint32_t v);
	Json& operator = (uint64_t v);
	Json& operator = (float v);
	Json& operator = (double v);
	Json& operator = (const char* v);
	Json& operator = (const std::string& v);
	Json& operator = (const Json& j);

	Json& operator [] (size_t index) { return Sub(index); }
	Json& operator [] (const std::string& name) { return Sub(name); }
	const Json& operator [] (size_t index) const { return Sub(index); }
	const Json& operator [] (const std::string& name) const { return Sub(name); }

	template <typename T>
	Json& operator += (T v) { Json node; node = v; Insert(node); return *this; }

	JsonType Type() const { return type_; }

	bool        AsBool()   const;
	int8_t      AsInt8()   const { return static_cast<int8_t>(AsNumber<uint16_t>()); } /* To avoid getting ASCII */
	int16_t     AsInt16()  const { return AsNumber<int16_t>(); }
	int32_t     AsInt32()  const { return AsNumber<int32_t>(); }
	int64_t     AsInt64()  const { return AsNumber<int64_t>(); }
	uint8_t     AsUint8()  const { return static_cast<uint8_t>(AsNumber<uint16_t>()); } /* To avoid getting ASCII */
	uint16_t    AsUint16() const { return AsNumber<uint16_t>(); }
	uint32_t    AsUint32() const { return AsNumber<uint32_t>(); }
	uint64_t    AsUint64() const { return AsNumber<uint64_t>(); }
	float       AsFloat()  const { return AsNumber<float>(); }
	double      AsDouble() const { return AsNumber<double>(); }
	std::string AsString() const;
public:
	size_t Size() const;
	std::vector<std::string> Keys() const;
	bool Has(const std::string& name) const;
	bool HasAndNotEmpty(const std::string& name) const;
public:
	Json Query(const std::string& path) const;
public:
	bool Parse(const std::string& text, size_t *pos = NULL, bool strict = false);
	bool Load(const std::string& filename, bool strict = false);
	bool Save(const std::string& filename, bool autoCreateDirectory = false) const;
public:
	std::string Dump(size_t indent = 0, const std::string& sp = "", const std::string& eol = "", bool unicode = false, bool omitLongString = false) const;
	std::string DumpU(size_t indent = 0, const std::string& sp = "", const std::string& eol = "", bool omitLongString = false) const { return Dump(indent, sp, eol, true, omitLongString); }
	std::string Format(size_t indent = 0, const std::string& sp = "\t", const std::string& eol = "\n", bool unicode = false, bool omitLongString = true) const { return Dump(indent, sp, eol, unicode, omitLongString); }
	std::string FormatU(size_t indent = 0, const std::string& sp = "\t", const std::string& eol = "\n", bool omitLongString = true) const { return Format(indent, sp, eol, true, omitLongString); }
private:
	JsonType type_;
	std::string text_;
	std::vector<Json> array_;
	std::map<std::string, Json> object_;
public:
	class Iterator
	{
	public:
		Iterator();
		std::string Name() const;
		Json& operator * () const;
		Json* operator -> () const { return &(operator * ()); }
		bool operator == (const Iterator& it) const;
		bool operator != (const Iterator& it) const { return ! operator == (it); }
		Iterator& operator ++ ();
		Iterator operator ++ (int) { Iterator it(*this); operator ++(); return it; }
	private:
		Json* json_;
		size_t index_;
		friend class Json;
	};
	friend class Iterator;

	class ConstIterator
	{
	public:
		ConstIterator();
		ConstIterator(const Iterator& it);
		std::string Name() const;
		const Json& operator * () const;
		const Json* operator -> () const { return &(operator * ()); }
		bool operator == (const ConstIterator& it) const;
		bool operator != (const ConstIterator& it) const { return ! operator == (it); }
		ConstIterator& operator ++ ();
		ConstIterator operator ++ (int) { ConstIterator it(*this); operator ++(); return it; }
	private:
		const Json* json_;
		size_t index_;
		friend class Json;
		friend class Iterator;
	};
	friend class ConstIterator;

	Iterator Begin();
	Iterator End();
	ConstIterator Begin() const;
	ConstIterator End() const;

protected:
	template <typename T> T AsNumber() const;
	template <typename T> static std::string ToString(T v);
	template <typename T> static T ToNumber(const std::string& s);
};

inline std::ostream& operator << (std::ostream& os, const Json& json)
{
	return (os << json.Dump());
}

template <typename T>
T Json::AsNumber() const
{
	T v = 0;
	if (type_ == TYPE_NULL) {
		return 0;
	} else if (type_ == TYPE_BOOL) {
		return static_cast<T>(AsBool() ? 1 : 0);
	} else if (type_ == TYPE_STRING || type_ == TYPE_NUMBER) {
		std::stringstream ss(text_);
		ss >> v;
	}
	return v;
}

template <typename T>
std::string Json::ToString(T v)
{
	std::stringstream ss;
	ss << v;
	return ss.str();
}

template <typename T>
T Json::ToNumber(const std::string& s)
{
	T v = 0;
	std::stringstream ss(s);
	ss >> v;
	return v;
}

Json J(const std::string& s);

#endif
