#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>

class JsonValue;

const int max_depth = 200;

class Json final
{
public:
    enum Type
    {
        NUL,
        NUMBER,
        BOOL,
        STRING,
        ARRAY,
        OBJECT
    };
    typedef std::vector<Json> array;
    typedef std::map<std::string, Json> object;
    Json();
    Json(std::nullptr_t);
    Json(double JsonValue);
    Json(int JsonValue);
    Json(bool JsonValue);

    Json(const char *JsonValue);
    Json(const std::string &JsonValue);
    Json(std::string &&JsonValue);

    Json(const array &JsonValue);
    Json(array &&JsonValue);

    Json(const object &JsonValue);
    Json(object &&JsonValue);

    Json(void *) = delete;

    Type type() const;
    bool is_null() const { return Type() == NUL; }
    bool is_number() const { return Type() == NUMBER; }
    bool is_bool() const { return Type() == BOOL; }
    bool is_string() const { return Type() == STRING; }
    bool is_array() const { return Type() == ARRAY; }
    bool is_object() const { return Type() == OBJECT; }

    double number_value() const;
    int int_value() const;
    bool bool_value() const;
    const std::string &string_value() const;
    const array &array_items() const;
    const object &object_items() const;

    const Json &operator[](size_t i) const;
    const Json &operator[](const std::string &key) const;

    void format(std::string &out) const;
    std::string format() const
    {
        std::string out;
        format(out);
        return out;
    }

    bool operator==(const Json &rhs) const;
    bool operator<(const Json &rhs) const;
    bool operator!=(const Json &rhs) const { return !(*this == rhs); }
    bool operator<=(const Json &rhs) const { return !(rhs < *this); }
    bool operator>(const Json &rhs) const { return (rhs < *this); }
    bool operator>=(const Json &rhs) const { return !(*this < rhs); }

    ~Json() {}

private:
    std::shared_ptr<JsonValue> m_ptr;
};

class JsonValue
{

protected:
    friend Json;

    JsonValue(Json::Type type) : m_type(type) {}

    virtual Json::Type type() const = 0;
    virtual void format(std::string &out) const = 0;

    virtual double number_value() const;
    virtual int int_value() const;
    virtual bool bool_value() const;
    virtual const std::string &string_value() const;

    virtual const Json::array &array_items() const;
    virtual const Json &operator[](size_t i) const;

    virtual const Json::object &object_items() const;
    virtual const Json &operator[](const std::string &key) const;

    //暂且不做比较
    virtual bool equals(const JsonValue *other) const { return false; }
    virtual bool less(const JsonValue *other) const { return false; }

    virtual ~JsonValue(){};

    Json::Type m_type;
};

class JsonParser
{
public:
    JsonParser(const std::string &s) : str(s),i(0){};
    ~JsonParser() {}
    void get_next_pos();
    std::string parse_string();
    Json parse_number();
    Json parse_json(int depth=0);
public:
    const std::string &str;
    size_t i;
};
