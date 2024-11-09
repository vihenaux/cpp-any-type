#include "any.hpp"
#include <sstream>
#include <set>
#include <ranges>

namespace any_type
{

    Any::Any() : type_(NONE)
    {

    }

    Any::Any(ANY_TYPE type) : type_(type)
    {

    }

    Any::Any(const std::string & value) : type_(STRING), str_(value)
    {

    }

    Any::Any(int value) : type_(INT), int_(value)
    {

    }

    Any::Any(double value) : type_(FLOAT), dbl_(value)
    {

    }

    Any::Any(bool value) : type_(BOOL), bool_(value)
    {

    }

    Any::Any(const std::map<std::string,Any> & value) : type_(DICT), dict_(value)
    {

    }

    Any::Any(const std::vector<Any> & value) : type_(ARRAY), array_(value)
    {

    }

    Any::Any(const ANY_OBJECT_STATUS & status) : status_(status)
    {

    }

    ANY_TYPE Any::getType() const
    {
        return type_;
    }

    ANY_OBJECT_STATUS Any::getStatus() const
    {
        return status_;
    }

    void Any::setStatus(const ANY_OBJECT_STATUS & status)
    {
        status_ = status;
    }

    std::string Any::to_string() const
    {
        switch(type_)
        {
            case STRING:
                return str_;
            case INT:
                return std::to_string(int_);
            case FLOAT: {
                std::stringstream ss;
                ss << dbl_;
                return ss.str();
            }
            case BOOL:
                return (bool_) ? "true" : "false";
            case DICT:
            case ARRAY:
            case NONE:
                return "";
        }
    }

    std::string Any::getStr() const
    {
        return (type_ == STRING) ? str_ : "";
    }

    int Any::getInt() const
    {
        return (type_ == INT) ? int_ : 0;
    }

    double Any::getFlt() const
    {
        return (type_ == FLOAT) ? dbl_ : 0.;
    }

    bool Any::getBool() const
    {
        return type_ == BOOL && bool_;
    }

    Any Any::operator[](const std::string & key) const
    {
        return (type_ == DICT) ? Any(dict_.at(key)) : Any();
    }

    Any Any::operator[](unsigned int index) const
    {
        return (type_ == ARRAY) ? array_[index] : Any();
    }

    bool Any::contains(const std::string & key) const
    {
        return type_ == DICT && dict_.contains(key);
    }

    std::vector<std::string> Any::getKeys() const
    {
        auto key_view = std::views::keys(dict_);
        std::vector<std::string> keys{ key_view.begin(), key_view.end() };
        return keys;
    }

    unsigned int Any::size() const
    {
        switch(type_)
        {
            case DICT:
                return static_cast<unsigned int>(dict_.size());
            case ARRAY:
                return static_cast<unsigned int>(array_.size());
            case INT:
            case FLOAT:
            case BOOL:
            case STRING:
            case NONE:
                return 0;
        }
    }

    void Any::add(const std::string & key, const Any & value)
    {
        if(type_ == DICT)
        {
            dict_[key] = value;
        }
    }

    void Any::add(const Any & value)
    {
        if(type_ == ARRAY)
        {
            array_.push_back(value);
        }
    }

    void Any::debug() const
    {
        switch(type_)
        {
            case STRING:
                std::cout << "STR: " << str_ << std::endl;
                break;
            case INT:
                std::cout << "INT: " << int_ << std::endl;
                break;
            case FLOAT:
                std::cout << "FLT: " << dbl_ << std::endl;
                break;
            case BOOL:
                std::cout << "BOOL: " << bool_<< std::endl;
                break;
            case DICT:
                std::cout << "DICT(" << dict_.size() << "): {";
                for(auto const& x : dict_)
                {
                    std::cout << "\"" << x.first << "\" -> ";
                    x.second.debug();
                }
                std::cout << "}" << std::endl;
                break;
            case ARRAY:
                std::cout << "ARRAY: " << array_.size() << std::endl;
                break;
            case NONE:
                std::cout << "NONE" << std::endl;
                break;
        }
    }

    Any readJson(const std::string & path)
    {
        std::ifstream jsonFile(path);

        if(!jsonFile.is_open())
        {
            return Any(ANY_OBJECT_STATUS::IMPORT_FAIL);
        }

        std::string content = "";

        for(std::string line; std::getline(jsonFile, line);)
        {
            content += line;
        }

        return readJsonStr(content);
    }

    Any readJsonStr(const std::string & json)
    {
        unsigned int cursor = 0;
        return readJson(json, cursor);
    }

    Any readJson(const std::string & json, unsigned int & cursor)
    {
        ANY_TYPE this_type = ANY_TYPE::NONE;
        std::string this_content = "";
        Any tmp_array(ANY_TYPE::ARRAY);
        Any tmp_dict(ANY_TYPE::DICT);
        std::set<char> block_start({'{','"','t','f','0','1','2','3','4','5','6','7','8','9','.'});
        bool key_found = false;

        while(cursor < json.size())
        {
            if(this_type == ANY_TYPE::NONE)
            {
                switch(json[cursor])
                {
                    case '{':
                        this_type = ANY_TYPE::DICT;
                        break;
                    case '[':
                        this_type = ANY_TYPE::ARRAY;
                        break;
                    case '"':
                        this_type = ANY_TYPE::STRING;
                        break;
                    case 't':
                    case'f':
                        this_type = ANY_TYPE::BOOL;
                        this_content = json[cursor];
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                    case '.':
                        this_type = ANY_TYPE::FLOAT;
                        this_content = json[cursor];
                        break;
                    default:
                        break;
                }
            }
            else if(this_type == ANY_TYPE::BOOL)
            {
                if(json[cursor] == ',')
                {
                    std::cerr << "Json parsing error : unexpected character : " << json[cursor] << " after " << this_content << std::endl;
                }

                this_content += json[cursor];

                if(this_content.size() > 5)
                {
                    std::cerr << "Json parsing error : unexpected token : " << this_content << std::endl;
                }

                if(this_content == "true")
                {
                    return Any(true);
                }
                else if(this_content == "false")
                {
                    return  Any(false);
                }
            }
            else if(this_type == ANY_TYPE::STRING)
            {
                if(json[cursor] == '"')
                {
                    if(this_content.back() == '\\')
                    {
                        this_content.back() = '"';
                    }
                    else
                    {
                        return Any(this_content);
                    }
                }
                else
                {
                    this_content += json[cursor];
                }
            }
            else if(this_type == ANY_TYPE::FLOAT)
            {
                if(json[cursor] != '.' && !(json[cursor] >= '0' && json[cursor] <= '9'))
                {
                    if(this_content.find('.') != std::string::npos)
                    {
                        return Any(std::stod(this_content));
                    }
                    else
                    {
                        return Any(std::stoi(this_content));
                    }
                }
                this_content += json[cursor];
            }
            else if(this_type == ANY_TYPE::ARRAY)
            {
                if(json[cursor] == ']')
                {
                    return tmp_array;
                }
                if(block_start.count(json[cursor]) != 0)
                {
                    tmp_array.add(readJson(json, cursor));
                }
            }
            else /* if(this_type == ANY_TYPE::DICT) */
            {
                if(json[cursor] == '"' && json[cursor-1] != '\\')
                {
                    key_found = !key_found;
                    if(key_found)
                    {
                        this_content = "";
                    }
                }
                else if(key_found)
                {
                    if(json[cursor] == '"')
                    {
                        this_content.back() = json[cursor];
                    }
                    else
                    {
                        this_content += json[cursor];
                    }
                }
                else if(json[cursor] == '}')
                {
                    return tmp_dict;
                }
                else if(json[cursor] == ':')
                {
                    tmp_dict.add(this_content, readJson(json,cursor));
                }
            }

            ++cursor;
        }

        switch(this_type)
        {
            case ANY_TYPE::DICT:
                std::cerr << "Json parsing error: expected }, got end of file instead" << std::endl;
                break;
            case ANY_TYPE::ARRAY:
                std::cerr << "Json parsing error: expected ], got end of file instead" << std::endl;
                break;
            case ANY_TYPE::STRING:
                std::cerr << "Json parsing error: expected \", got end of file instead" << std::endl;
                break;
            case ANY_TYPE::INT:
            case ANY_TYPE::FLOAT:
            case ANY_TYPE::BOOL:
                std::cerr << "Json parsing error: json format do not allow to end with : " << this_content << std::endl;
                break;
            case ANY_TYPE::NONE:
                std::cerr << "Json parsing error: json format not detected" << std::endl;
                break;
        }
        return Any();
    }

    ANY_TYPE intToType(int type)
    {
        switch(type)
        {
            case 1:
                return STRING;
            case 2:
                return INT;
            case 3:
                return FLOAT;
            case 4:
                return BOOL;
            case 5:
                return DICT;
            case 6:
                return ARRAY;
            default:
                return NONE;
        }
    }

    int typeToInt(ANY_TYPE type)
    {
        switch(type)
        {
            case STRING:
                return 1;
            case INT:
                return 2;
            case FLOAT:
                return 3;
            case BOOL:
                return 4;
            case DICT:
                return 5;
            case ARRAY:
                return 6;
            case NONE:
                return 0;
        }
    }

    std::string readString(std::ifstream & in)
    {
        while(!in.eof())
        {
            char c;
            in.get(c);
            if(c == '"')
            {
                break;
            }
        }
        std::string tmp;
        std::getline(in, tmp, '"');
        return tmp;
    }

} // any_type
