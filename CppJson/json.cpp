#include "json.hpp"

//供全局使用的静态nulljson
static const Json &static_null()
{
    static const Json jsonnull;
    return jsonnull;
}

//null
class JsonNull final : public JsonValue
{
public:
    JsonNull() : JsonValue(Json::NUL) {}
    Json::Type type() const override { return Json::NUL; }
    void format(std::string &out) const override
    {
        out += "null";
        //std::cout<<out<<std::endl;
    }
};

//double
class JsonDouble final : public JsonValue
{
public:
    explicit JsonDouble(const double value) : m_val(value), JsonValue(Json::NUMBER) {}
    Json::Type type() const override { return Json::NUMBER; }
    void format(std::string &out) const override
    {
        out += std::to_string(m_val);
        //std::cout<<out<<std::endl;
    }
    double number_value() const override { return m_val; }
    int int_value() const override { return static_cast<int>(m_val); }

private:
    double m_val;
};

//int
class JsonInt final : public JsonValue
{
public:
    explicit JsonInt(const int value) : m_val(value), JsonValue(Json::NUMBER) {}
    Json::Type type() const override { return Json::NUMBER; }
    void format(std::string &out) const override
    {
        out += std::to_string(m_val);
        //std::cout<<out<<std::endl;
    }
    double number_value() const override { return m_val; }
    int int_value() const override { return m_val; }

private:
    int m_val;
};

//bool
class JsonBool final : public JsonValue
{
public:
    explicit JsonBool(const bool value) : m_val(value), JsonValue(Json::BOOL) {}
    Json::Type type() const override { return Json::BOOL; }
    void format(std::string &out) const override
    {
        out += m_val ? "true" : "false";
        //std::cout<<out<<std::endl;
    }
    bool bool_value() const override { return m_val; }

private:
    bool m_val;
};

//string
class JsonString final : public JsonValue
{
public:
    explicit JsonString(const std::string &value) : m_val(value), JsonValue(Json::STRING) {}
    explicit JsonString(std::string &&value) : m_val(std::move(value)), JsonValue(Json::STRING) {}
    Json::Type type() const override { return Json::STRING; }

    void format(std::string &out) const override
    {
        out += '"';
        for (size_t i = 0; i < m_val.length(); i++)
        {
            const char ch = m_val[i];
            if (ch == '\\')
            {
                out += "\\\\";
            }
            else if (ch == '"')
            {
                out += "\\\"";
            }
            else if (ch == '\b')
            {
                out += "\\b";
            }
            else if (ch == '\f')
            {
                out += "\\f";
            }
            else if (ch == '\n')
            {
                out += "\\n";
            }
            else if (ch == '\r')
            {
                out += "\\r";
            }
            else if (ch == '\t')
            {
                out += "\\t";
            }
            else
            {
                out += ch;
            }
        }
        out += '"';
        //std::cout<<out<<std::endl;
    }
    const std::string &string_value() const override { return m_val; }

private:
    std::string m_val;
};
//array
class JsonArray final : public JsonValue
{
public:
    JsonArray(const Json::array &value) : m_val(value), JsonValue(Json::ARRAY) {}
    JsonArray(Json::array &&value) : m_val(std::move(value)), JsonValue(Json::ARRAY) {}
    const Json &operator[](size_t i) const override
    {
        if (i < 0 || i >= m_val.size())
            return static_null();
        return m_val[i];
    }
    Json::Type type() const override { return Json::ARRAY; }
    void format(std::string &out) const override
    {
        bool first = true;
        out += "[";
        for (const auto &value : m_val)
        {
            if (!first)
                out += ", ";
            value.format(out);
            first = false;
        }
        out += "]";
    }
    const Json::array &array_items() const override { return m_val; }

private:
    Json::array m_val;
};
//object
class JsonObject final : public JsonValue
{
public:
    JsonObject(const Json::object &value) : m_val(value), JsonValue(Json::ARRAY) {}
    JsonObject(Json::object &&value) : m_val(std::move(value)), JsonValue(Json::ARRAY) {}
    const Json &operator[](const std::string &key) const override
    {
        auto it = m_val.find(key);
        return it == m_val.end() ? static_null() : it->second;
    }
    Json::Type type() const override { return Json::ARRAY; }
    void format(std::string &out) const override
    {
        bool first = true;
        out += "{";
        for (const auto &value : m_val)
        {
            if (!first)
                out += ", ";
            out += value.first;
            out += ':';
            value.second.format(out);
            first = false;
        }
        out += "}";
    }
    const Json::object &object_items() const override { return m_val; }

private:
    Json::object m_val;
};

//构造函数
Json::Json() :                              m_ptr(std::make_shared<JsonNull>()) {}
Json::Json(std::nullptr_t) :                m_ptr(std::make_shared<JsonNull>()) {}
Json::Json(double value) :                  m_ptr(std::make_shared<JsonDouble>(value)) {}
Json::Json(int value) :                     m_ptr(std::make_shared<JsonInt>(value)) {}
Json::Json(bool value) :                    m_ptr(std::make_shared<JsonBool>(value)) {}
Json::Json(const std::string &value) :      m_ptr(std::make_shared<JsonString>(value)) {}
Json::Json(std::string &&value) :           m_ptr(std::make_shared<JsonString>(move(value))) {}
Json::Json(const char *value) :             m_ptr(std::make_shared<JsonString>(value)) {}
Json::Json(const Json::array &values) :     m_ptr(std::make_shared<JsonArray>(values)) {}
Json::Json(Json::array &&values) :          m_ptr(std::make_shared<JsonArray>(move(values))) {}
Json::Json(const Json::object &values) :    m_ptr(std::make_shared<JsonObject>(values)) {}
Json::Json(Json::object &&values) :         m_ptr(std::make_shared<JsonObject>(move(values))) {}

//比较
bool Json::operator==(const Json &other) const
{
    if (m_ptr == other.m_ptr)
        return true;
    if (m_ptr->type() != other.m_ptr->type())
        return false;

    return m_ptr->equals(other.m_ptr.get());
}

bool Json::operator<(const Json &other) const
{
    if (m_ptr == other.m_ptr)
        return false;
    if (m_ptr->type() != other.m_ptr->type())
        return m_ptr->type() < other.m_ptr->type();

    return m_ptr->less(other.m_ptr.get());
}

//Json成员函数，调用包含value类型对应的成员函数
Json::Type Json::type()                                     const { return m_ptr->type(); }
void Json::format(std::string &out)                         const { m_ptr->format(out); }
double Json::number_value()                                 const { return m_ptr->number_value(); }
int Json::int_value()                                       const { return m_ptr->int_value(); }
bool Json::bool_value()                                     const { return m_ptr->bool_value(); }
const std::string &Json::string_value()                     const { return m_ptr->string_value(); }
const std::vector<Json> &Json::array_items()                const { return m_ptr->array_items(); }
const std::map<std::string, Json> &Json::object_items()     const { return m_ptr->object_items(); }
const Json &Json::operator[](size_t i)                      const { return (*m_ptr)[i]; }
const Json &Json::operator[](const std::string &key)        const { return (*m_ptr)[key]; }

//JsonValue 成员函数,在具体JsonValue中被重写
double JsonValue::number_value()                                const { return 0; }
int JsonValue::int_value()                                      const { return 0; }
bool JsonValue::bool_value()                                    const { return false; }
const std::string &JsonValue::string_value()                    const { return ""; }
const std::vector<Json> &JsonValue::array_items()               const { return std::vector<Json>(); }
const std::map<std::string, Json> &JsonValue::object_items()    const { return std::map<std::string, Json>(); }
const Json &JsonValue::operator[](size_t)                       const { return static_null(); }
const Json &JsonValue::operator[](const std::string &)          const { return static_null(); }

void JsonParser::get_next_pos()
{
    while (str[i] == '\n' || str[i] == ' ' || str[i] == '\t')
        i++;
}
std::string JsonParser::parse_string()
{
    std::string out;
    //long last_escaped_codepoint = -1;
    while (true)
    {
        // if (i == str.size())
        //     return fail("unexpected end of input in string", "");

        char ch = str[i++];
        //std::cout<<ch;
        if (ch == '"')
        {
            return out;
        }

        // if (in_range(ch, 0, 0x1f))
        //     return fail("unescaped " + esc(ch) + " in string", "");

        // The usual case: non-escaped characters
        if (ch != '\\')
        {
            // encode_utf8(last_escaped_codepoint, out);
            //last_escaped_codepoint = -1;
            out += ch;
            continue;
        }

        // Handle escapes
        // if (i == str.size())
        //     return fail("unexpected end of input in string", "");

        ch = str[i++];

        if (ch == 'b')
        {
            out += '\b';
        }
        else if (ch == 'f')
        {
            out += '\f';
        }
        else if (ch == 'n')
        {
            out += '\n';
        }
        else if (ch == 'r')
        {
            out += '\r';
        }
        else if (ch == 't')
        {
            out += '\t';
        }
        else if (ch == '"' || ch == '\\' || ch == '/')
        {
            out += ch;
        }
        // } else {
        //     return fail("invalid escape character " + esc(ch), "");
        // }
    }
    //std::cout<<out<<std::endl;
}

Json JsonParser::parse_number()
{
    size_t start_pos = i;

    if (str[i] == '-')
        i++;

    // Integer part
    if (str[i] == '0')
    {
        i++;
        //if (str[i] >= '0' && str[i] <= '9'))
        //return fail("leading 0s not permitted in numbers");
    }
    else if (str[i] >= '1' && str[i] <= '9')
    {
        i++;
        while (str[i] >= '1' && str[i] <= '9')
            i++;
    }

    if (str[i] != '.')
    {
        return std::atoi(str.c_str() + start_pos);
    }

    // Decimal part
    if (str[i] == '.')
    {
        i++;
        while (str[i] >= '0' && str[i] <= '9')
            i++;
    }
    return std::strtod(str.c_str() + start_pos, nullptr);
}

Json JsonParser::parse_json(int depth)
{
    if (depth > max_depth)
    {
        Json(nullptr);
    }

    get_next_pos();
    char ch = str[i++];

    if (ch == '-' || (ch >= '0' && ch <= '9'))
    {
        i--;
        return parse_number();
    }

    if (ch == 't')
    {
        i += 4;
        return Json(true);
    }
    if (ch == 'f')
    {
        i += 5;
        return Json(false);
    }
    if (ch == 'n')
    {
        i += 4;
        return Json(nullptr);
    }

    if (ch == '"')
        return parse_string();

    if (ch == '{')
    {
        std::map<std::string, Json> data;
       get_next_pos();
        ch = str[i++];
        if (ch == '}')
            return data;

        while (1)
        {
            // if (ch != '"')
            //     return fail("expected '\"' in object, got " + esc(ch));

            std::string key = parse_string();
            // if (failed)
            //     return Json();
            get_next_pos();
            ch = str[i++];
            // if (ch != ':')
            //     return fail("expected ':' in object, got " + esc(ch));

            data[std::move(key)] = parse_json(depth + 1);
            // if (failed)
            //     return Json();
            get_next_pos();
            ch = str[i++];
            if (ch == '}')
                break;
            // if (ch != ',')
            //     return fail("expected ',' in object, got " + esc(ch));
            get_next_pos();
            ch = str[i++];
        }
        return data;
    }

    if (ch == '[')
    {
        std::vector<Json> data;
       get_next_pos();
        ch = str[i++];
        if (ch == ']')
            return data;

        while (1)
        {
            i--;
            data.push_back(parse_json(depth + 1));
            // if (failed)
            //     return Json();
           get_next_pos();
            ch = str[i++];
            if (ch == ']')
                break;
            // if (ch != ',')
            //     return fail("expected ',' in list, got " + esc(ch));
            get_next_pos();
            ch = str[i++];
            //(void)ch;
        }
        return data;
    }

    return Json(nullptr);
}
