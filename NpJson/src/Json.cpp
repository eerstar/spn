#include "Json.h"
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <libgen.h>
#include <sys/stat.h>

const size_t MAX_STRING_DISPLAY_SIZE = 1024;

static const char NULL_TEXT[] = "null";
static const char TRUE_TEXT[] = "true";
static const char FALSE_TEXT[] = "false";

const Json& Json::Null()
{
	static Json nullJson; // to make it available before main()
	return nullJson;
}

static inline std::string InitJsonText(Json::JsonType type, const std::string& text)
{
	if (type == Json::TYPE_NULL) {
		return NULL_TEXT;
	} else if (type == Json::TYPE_BOOL) {
		return (text == TRUE_TEXT ? TRUE_TEXT : FALSE_TEXT);
	} else if (type == Json::TYPE_ARRAY || type == Json::TYPE_OBJECT) {
		return "";
	} else {
		return text;
	}
}

Json::Json(Json::JsonType type, const std::string& text):
	type_(type), text_(InitJsonText(type, text))
{
}

Json::Json(bool v, Json::JsonType type):
	type_(type), text_(v ? TRUE_TEXT : FALSE_TEXT)
{
}

Json::Json(int8_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(int16_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(int32_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(int64_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(uint8_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(uint16_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(uint32_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(uint64_t v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(float v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(double v, Json::JsonType type):
	type_(type), text_(ToString(v))
{
}

Json::Json(const char* v, Json::JsonType type):
	type_(type), text_(v)
{
}

Json::Json(const std::string& v, Json::JsonType type):
	type_(type), text_(v)
{
}

Json::Json(const Json& j): type_(j.type_),
		text_(j.text_), array_(j.array_), object_(j.object_)
{
}

bool Json::EqualTo(const Json& j) const
{
	if (type_ != j.type_) {
		return false;
	}
	if (type_ == TYPE_ARRAY) {
		if (Size() != j.Size()) {
			return false;
		}
		for (size_t i = 0; i < Size(); ++i) {
			if (Sub(i) != j.Sub(i)) {
				return false;
			}
		}
	} else if (type_ == TYPE_OBJECT) {
		const std::vector<std::string> keys = Keys();
		if (keys != j.Keys()) {
			return false;
		}
		for (size_t i = 0; i < keys.size(); ++i) {
			if (Sub(keys[i]) != j.Sub(keys[i])) {
				return false;
			}
		}
	} else if (type_ != TYPE_NULL) {
		return text_ == j.text_;
	}
	return true;
}

Json& Json::Sub(size_t index)
{
	if (type_ != TYPE_ARRAY) {
		Clear(TYPE_ARRAY);
	}
	if (index >= array_.size()) {
		array_.resize(index + 1);
	}
	return array_[index];
}

Json& Json::Sub(const std::string& name)
{
	if (type_ != TYPE_OBJECT) {
		Clear(TYPE_OBJECT);
	}
	for (size_t i = 0; i < array_.size(); ++i) {
		if (array_[i].AsString() == name) {
			return object_[name];
		}
	}
	Json o;
	o = name;
	array_.push_back(o);
	return object_[name];
}

const Json& Json::Sub(size_t index) const
{
	if (type_ == TYPE_ARRAY && index < array_.size()) {
		return array_[index];
	}
	return Null();
}

const Json& Json::Sub(const std::string& name) const
{
	if (type_ == TYPE_OBJECT) {
		std::map<std::string, Json>::const_iterator it = object_.find(name);
		if (it != object_.end()) {
			return it->second;
		}
	}
	return Null();
}

void Json::Clone(const Json& j)
{
	Json copy(j); // considering j may be *this or part of *this
	type_ = copy.type_;
	text_ = copy.text_;
	array_ = copy.array_;
	object_ = copy.object_;
}

void Json::Clear(Json::JsonType type, const std::string& text)
{
	type_ = type;
	text_ = InitJsonText(type, text);
	array_.clear();
	object_.clear();
}

void Json::Insert(const Json& value, size_t before)
{
	Json copy(value); // consider value may be *this or part of *this
	if (type_ != TYPE_ARRAY) {
		Clear(TYPE_ARRAY);
	}
	if (before >= array_.size()) {
		array_.push_back(copy);
	} else {
		array_.insert(array_.begin() + before, copy);
	}
}

void Json::Insert(const std::string& key, const Json& value, size_t before)
{
	Json copy(value); // consider value may be *this or part of *this
	if (type_ != TYPE_OBJECT) {
		Clear(TYPE_OBJECT);
	}
	if (!Has(key)) {
		if (before < array_.size()) {
			array_.insert(array_.begin() + before, Json(key));
		} else {
			array_.push_back(Json(key));
		}
	}
	object_[key] = copy;
}

void Json::Move(size_t index, size_t before)
{
	if (index < array_.size()) {
		if (before < array_.size()) {
			if (index < before) {
				array_.insert(array_.begin() + before, array_[index]);
				array_.erase(array_.begin() + index);
			} else if (index > before) {
				array_.insert(array_.begin() + before, array_[index]);
				array_.erase(array_.begin() + index + 1);
			}
		} else {
			array_.insert(array_.end(), array_[index]);
			array_.erase(array_.begin() + index);
		}
	}
}

void Json::Move(const std::string& key, size_t before)
{
	for (size_t i = 0; i < array_.size(); ++i) {
		if (array_[i].AsString() == key) {
			Move(i, before);
			return;
		}
	}
}

void Json::Erase(size_t pos)
{
	if (type_ == TYPE_ARRAY && pos < array_.size()) {
		array_.erase(array_.begin() + pos);
	}
}

void Json::Erase(const std::string& name)
{
	if (type_ == TYPE_OBJECT) {
		std::map<std::string, Json>::iterator it = object_.find(name);
		if (it != object_.end()) {
			object_.erase(it);
			for (size_t i = 0; i < array_.size(); ++i) {
				if (array_[i].AsString() == name) {
					array_.erase(array_.begin() + i);
					break;
				}
			}
		}
	}
}

Json& Json::operator = (const Json& j)
{
	if (this != &j) {
		Clone(j);
	}
	return *this;
}

Json& Json::operator = (bool v)
{
	Clear(TYPE_BOOL, (v ? TRUE_TEXT : FALSE_TEXT));
	return *this;
}

Json& Json::operator = (int8_t v)
{
	std::ostringstream os;
	os << static_cast<int>(v);
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (int16_t v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (int32_t v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (int64_t v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (uint8_t v)
{
	std::ostringstream os;
	os << static_cast<unsigned int>(v);
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (uint16_t v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (uint32_t v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (uint64_t v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (float v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (double v)
{
	std::ostringstream os;
	os << v;
	Clear(TYPE_NUMBER, os.str());
	return *this;
}

Json& Json::operator = (const char* v)
{
	Clear(TYPE_STRING, v);
	return *this;
}

Json& Json::operator = (const std::string& v)
{
	Clear(TYPE_STRING, v);
	return *this;
}

bool Json::AsBool() const
{
	if (type_ == TYPE_ARRAY || type_ == TYPE_OBJECT) {
		return Size() > 0;
	} else if (type_ == TYPE_NULL) {
		return false;
	} else {
		return (text_ != "" && text_ != FALSE_TEXT && text_ != "0");
	}
}

std::string Json::AsString() const
{
	if (type_ == TYPE_NULL) {
		return "";
	} else if (type_ == TYPE_OBJECT || type_ == TYPE_ARRAY) {
		return Dump();
	} else {
		return text_;
	}
}

size_t Json::Size() const
{
	if (type_ == TYPE_ARRAY) {
		return array_.size();
	} else if (type_ == TYPE_OBJECT) {
		return object_.size();
	} else {
		return (type_ == TYPE_NULL ? 0 : 1);
	}
}

std::vector<std::string> Json::Keys() const
{
	std::vector<std::string> names;
	if (type_ == TYPE_OBJECT) {
		for (size_t i = 0; i < array_.size(); ++i) {
			names.push_back(array_[i].AsString());
		}
	}
	return names;
}

bool Json::Has(const std::string& name) const
{
	return (type_ == TYPE_OBJECT && object_.find(name) != object_.end());
}

bool Json::HasAndNotEmpty(const std::string& name) const
{
	return Has(name) && (*this)[name].AsString() != "";
}

static std::string EncodeString(const std::string& s, bool unicode)
{
	std::stringstream ss;
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] == '"') {
			ss << "\\\"";
		} else if (s[i] == '\\') {
			ss << "\\\\";
		} else if (s[i] == '/') {
			ss << "\\/";
		} else if (s[i] == '\b') {
			ss << "\\b";
		} else if (s[i] == '\f') {
			ss << "\\f";
		} else if (s[i] == '\n') {
			ss << "\\n";
		} else if (s[i] == '\r') {
			ss << "\\r";
		} else if (s[i] == '\t') {
			ss << "\\t";
		} else if (unicode) {
			unsigned int c = static_cast<unsigned char>(s[i]);
			if (c >= 0x7F) {
				ss << "\\u00"
					<< std::hex << std::uppercase
					<< static_cast<unsigned int>(c >> 4)
					<< static_cast<unsigned int>(c & 0x0F)
					<< std::dec << std::nouppercase;
			} else {
				ss << s[i];
			}
		} else {
			ss << s[i];
		}
	}
	return ss.str();
}

std::string Json::Dump(size_t indent, const std::string& sp, const std::string& eol, bool unicode, bool omitLongString) const
{
	std::string prefix;
	for (size_t i = 0; i < indent; ++i) {
		prefix += sp;
	}
	std::string suffix = eol;
	std::stringstream ss;
	if (type_ == TYPE_ARRAY) {
		ss << '[' << suffix;
		for (size_t i = 0; i < array_.size(); ++i) {
			ss << prefix << sp << array_[i].Dump(indent + 1, sp, eol, unicode, omitLongString);
			if (i + 1 < array_.size()) {
				ss << ",";
			}
			ss << eol;
		}
		ss << prefix << ']';
	} else if (type_ == TYPE_OBJECT) {
		ss << '{' << suffix;
		for (size_t i = 0; i < array_.size(); ++i) {
			std::string name = array_[i].AsString();
			ss << prefix << sp << '"' << EncodeString(name, unicode) << '"' << ":";
			std::map<std::string, Json>::const_iterator it = object_.find(name);
			if (it == object_.end()) {
				ss << NULL_TEXT;
			} else {
				ss << it->second.Dump(indent + 1, sp, eol, unicode, omitLongString);
			}
			if (i + 1 < array_.size()) {
				ss << ',';
			}
			ss << eol;
		}
		ss << prefix << '}';
	} else if (type_ == TYPE_STRING) {
		if (omitLongString && text_.size() > MAX_STRING_DISPLAY_SIZE) {
			ss << '"' << text_.substr(0, MAX_STRING_DISPLAY_SIZE) << "...\"(" << text_.size() << " bytes)";
		} else {
			ss << '"' << EncodeString(text_, unicode) << '"';
		}
	} else {
		ss << text_;
	}
	return ss.str();
}

static void SkipSpaces(const char *& s)
{
	while (*s) {
		if (isspace(*s)) {
			++s;
		} else if (*s == '/' && *(s + 1) == '*') { // Skip comments
			s += 2;
			while (*s) {
				if (*s == '*' && *(s + 1) == '/') {
					s += 2;
					break;
				}
				++s;
			}
		} else {
			break;
		}
	}
}

static unsigned int HexValue(int c)
{
	if (c >= '0' && c <= '9') {
		return (c - '0');
	} else if (c >= 'A' && c <= 'F') {
		return (c - 'A') + 10;
	} else if (c >= 'a' && c <= 'f') {
		return (c - 'a') + 10;
	} else {
		return c;
	}
}

static bool ParseString(const char *& s, Json& v, bool strict = true)
{
	SkipSpaces(s);
	if (strict) {
		if (*s != '"') return false; else ++s;
	}
	std::string text;
	while (*s) {
		if (strict) {
			if (*s == '"') break;
		} else {
			if (*s == ':' || *s == ',' || *s == '}' || *s == ']' || isspace(*s)) break;
		}
		if (*s == '\\') {
			switch (*(++s)) {
			case '"':
			case '\\':
			case '/': text += *s++; break;
			case 'b': text += '\b'; ++s; break;
			case 'f': text += '\f'; ++s; break;
			case 'n': text += '\n'; ++s; break;
			case 'r': text += '\r'; ++s; break;
			case 't': text += '\t'; ++s; break;
			case 'u':
				if (!isxdigit(s[1])) return false;
				if (!isxdigit(s[2])) return false;
				if (!isxdigit(s[3])) return false;
				if (!isxdigit(s[4])) return false;
				text.push_back(static_cast<char>(
						(HexValue(s[1]) << 12) +
						(HexValue(s[2]) << 8) +
						(HexValue(s[3]) << 4) +
						HexValue(s[4])));
				s += 5;
				break;
			default: return false;
			}
		} else {
			text += *s++;
		}
	}
	if (strict) {
		if (*s != '"') return false; else ++s;
	}
	v = text;
	return true;
}

static bool ParseNumber(const char *& s, Json& v)
{
	const char *s0 = s;
	SkipSpaces(s);
	if (*s == '-' || *s == '+') ++s;
	if (!isdigit(*s) && *s != '.') return false; else ++s;
	while (isdigit(*s)) ++s;
	if (*s == '.') {
		++s;
		while (isdigit(*s)) ++s;
	}
	if (*s == 'e' || *s == 'E') {
		++s;
		if (*s == '+' || *s == '-') ++s;
		if (!isdigit(*s)) return false; else ++s;
		while (isdigit(*s)) ++s;
	}
	v.Clear(Json::TYPE_NUMBER, std::string(s0, s));
	return true;
}

static bool ParseValue(const char *& s, Json& v, bool strict);

static bool ParseObject(const char *& s, Json& v, bool strict)
{
	SkipSpaces(s);
	if (*s != '{') return false; else ++s;
	SkipSpaces(s);
	v.Clear(Json::TYPE_OBJECT);
	if (*s != '}') {
		for (;;) {
			if (!strict) {
				SkipSpaces(s);
				if (*s == ',') {
					++s;
					continue;
				} else if (*s == '}') {
					break;
				}
			}
			std::string name;
			Json o;
			if (*s != '"' && !strict) {
				if (!ParseString(s, o, false)) return false;
			} else {
				if (!ParseString(s, o)) return false;
			}
			name = o.AsString();
			SkipSpaces(s);
			if (*s != ':') return false; else ++s;
			if (!ParseValue(s, o, strict)) return false;
			SkipSpaces(s);
			v[name] = o;
			if (*s == '}') break;
			if (*s == ',') {
				++s;
			} else {
				if (strict) return false;
			}
		}
	}
	++s;
	return true;
}

static bool ParseArray(const char *& s, Json& v, bool strict)
{
	SkipSpaces(s);
	if (*s != '[') return false; else ++s;
	SkipSpaces(s);
	v.Clear(Json::TYPE_ARRAY);
	if (*s != ']') {
		for (;;) {
			if (!strict) {
				SkipSpaces(s);
				if (*s == ',') {
					++s;
					continue;
				} else if (*s == ']') {
					break;
				}
			}
			Json o;
			if (!ParseValue(s, o, strict)) return false;
			SkipSpaces(s);
			v.Insert(o);
			if (*s == ']') break;
			if (*s == '}') return false;
			if (*s == ',') {
				++s;
			} else {
				if (strict) return false;
			}
		}
	}
	++s;
	return true;
}

static inline bool IsVaidSeparator(char c)
{
	return (c == '\0' || strchr(",]} \t\n\r", c) != NULL);
}

static inline bool MatchSymbol(const char *& s, const char *t, size_t size)
{
	if (memcmp(s, t, size) == 0 && IsVaidSeparator(s[size])) {
		s += size;
		return true;
	}
	return false;
}

static bool ParseValue(const char *& s, Json& v, bool strict)
{
	SkipSpaces(s);
	if (*s == '"') {
		return ParseString(s, v);
	}
	if (*s == '+' || *s == '-' || *s == '.' || isdigit(*s)) {
		const char *p = s;
		if (ParseNumber(p, v)) {
			if (strict) {
				s = p;
				return true;
			}
			if (IsVaidSeparator(*p)) {
				s = p;
				return true;
			}
		} else if (strict) {
			return false;
		}
		// otherwise, try to parse as string
	}
	if (*s == '{') {
		return ParseObject(s, v, strict);
	} else if (*s == '[') {
		return ParseArray(s, v, strict);
	} else if (MatchSymbol(s, NULL_TEXT, sizeof(NULL_TEXT) - 1)) {
		v.Clear();
		return true;
	} else if (MatchSymbol(s, TRUE_TEXT, sizeof(TRUE_TEXT) - 1)) {
		v = true;
		return true;
	} else if (MatchSymbol(s, FALSE_TEXT, sizeof(FALSE_TEXT) - 1)) {
		v = false;
		return true;
	} else if (!strict) {
		return ParseString(s, v, false);
	} else {
		return false;
	}
}

bool Json::Parse(const std::string& text, size_t *pos, bool strict)
{
	const char *p = text.c_str();
	if (!ParseValue(p, *this, strict)) {
		if (pos) *pos = p - text.c_str();
		return false;
	}
	SkipSpaces(p);
	if (pos) *pos = p - text.c_str();
	return (!*p);
}

bool Json::Load(const std::string& filename, bool strict)
{
	std::ifstream file(filename.c_str(), std::ios::in);
	if (!file.is_open()) {
		std::cerr << "cannot open input json file '" << filename << "'" << std::endl;
		return false;
	}
	std::string text;
	std::string line;
	while (std::getline(file, line)) {
		text += line;
	}
	size_t pos = 0;
	if (!Parse(text, &pos, strict)) {
		std::cerr << "invalid json format in file '" << filename << "'" << std::endl;
		return false;
	}
	if (pos != text.size()) {
		std::cerr << "unexpected character after json in file '" << filename << "'" << std::endl;
		return false;
	}
	return true;
}

bool Json::Save(const std::string& filename, bool autoCreateDirectory) const
{
	if (autoCreateDirectory) {
		char* p = strdup(filename.c_str());
		if (p) {
			mkdir(dirname(p), 0755);
			free(p);
		}
	}

	std::ofstream file(filename.c_str(), std::ios::out);
	if (!file.is_open()) {
		std::cerr << "cannot open output json file '" << filename << "'" << std::endl;
		return false;
	}
	file << Format(0, "\t", "\n", false, false) << std::endl; // avoid omitting long strings
	file.close();
	return true;
}

Json::Iterator::Iterator(): json_(NULL), index_(0)
{
}

std::string Json::Iterator::Name() const
{
	if (json_ && json_->Type() == Json::TYPE_OBJECT) {
		return json_->array_.at(index_).AsString();
	}
	return "";
}

Json& Json::Iterator::operator * () const
{
	if (json_ && json_->Type() == Json::TYPE_OBJECT) {
		return json_->object_.at(json_->array_.at(index_).AsString());
	} else if (json_ && json_->Type() == Json::TYPE_ARRAY) {
		return json_->array_.at(index_);
	} else {
		throw std::runtime_error("unexpected json type");
	}
}

bool Json::Iterator::operator == (const Iterator& it) const
{
	if (json_ != it.json_) {
		throw std::runtime_error("unmatch json objects");
	}
	return (index_ == it.index_);
}

Json::Iterator& Json::Iterator::operator ++ ()  // only support '++it', but no 'it++' at all.
{
	if (index_ < json_->array_.size()) {
		++index_;
	}
	return *this;
}


Json::ConstIterator::ConstIterator(): json_(NULL), index_(0)
{
}

Json::ConstIterator::ConstIterator(const Iterator& it): json_(it.json_), index_(it.index_)
{
}

std::string Json::ConstIterator::Name() const
{
	if (json_ && json_->Type() == Json::TYPE_OBJECT) {
		return json_->array_.at(index_).AsString();
	}
	return "";
}

const Json& Json::ConstIterator::operator * () const
{
	if (json_ && json_->Type() == Json::TYPE_OBJECT) {
		return json_->object_.at(json_->array_.at(index_).AsString());
	} else if (json_ && json_->Type() == Json::TYPE_ARRAY) {
		return json_->array_.at(index_);
	} else {
		return Null();
	}
}

bool Json::ConstIterator::operator == (const ConstIterator& it) const
{
	if (json_ != it.json_) {
		throw std::runtime_error("unmatch json objects");
	}
	return (index_ == it.index_);
}

Json::ConstIterator& Json::ConstIterator::operator ++ ()  // only support '++it', but no 'it++' at all.
{
	if (index_ < json_->array_.size()) {
		++index_;
	}
	return *this;
}

Json::Iterator Json::Begin()
{
	Iterator it;
	it.json_ = this;
	it.index_ = 0;
	return it;
}

Json::Iterator Json::End()
{
	Iterator it;
	it.json_ = this;
	it.index_ = array_.size();
	return it;
}

Json::ConstIterator Json::Begin() const
{
	ConstIterator it;
	it.json_ = this;
	it.index_ = 0;
	return it;
}

Json::ConstIterator Json::End() const
{
	ConstIterator it;
	it.json_ = this;
	it.index_ = array_.size();
	return it;
}

Json Json::Query(const std::string& path) const
{
	Json res;
	if (path.empty()) {
		res = *this;
	} else {
		std::string::size_type pos = path.find('/');
		std::string tag = path;
		std::string rest;
		if (pos != std::string::npos) {
			tag = path.substr(0, pos);
			rest = path.substr(pos + 1);
		}

		if (tag == "*") {
			for (Json::ConstIterator it = Begin(); it != End(); ++it) {
				Json sub = it->Query(rest);
				if (sub.Type() == Json::TYPE_ARRAY) {
					for (size_t i = 0; i < sub.Size(); ++i) {
						res += sub[i];
					}
				} else {
					res += sub;
				}
			}
		} else if (Type() == Json::TYPE_ARRAY) {
			size_t index = Json(tag).AsUint32();
			res = Sub(index).Query(rest);
		} else {
			res = Sub(tag).Query(rest);
		}
	}
	return res;
}

Json J(const std::string& s)
{
	Json res;
	if (!res.Parse(s)) {
		throw std::runtime_error("invalid json string");
	}
	return res;
}
