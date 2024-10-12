#ifndef GCG_ANY_HPP
#define GCG_ANY_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>

namespace gcg {

    enum ANY_TYPE {
        STRING  = 1,
        INT     = 2,
        FLOAT   = 3,
        BOOL    = 4,
        DICT    = 5,
        ARRAY   = 6,
        NONE    = 0
    };

    class Any {
        public:

        Any();
        Any(const Any & a) = default;
        Any(const std::string & value);
        Any(int value);
        Any(double value);
        Any(bool value);
        Any(const std::map<std::string,Any> & value);
        Any(const std::vector<Any> & value);

        ANY_TYPE getType() const;

        std::string to_string() const;
        std::string getStr() const;
        int getInt() const;
        double getFlt() const;
        bool getBool() const;
        Any operator[](const std::string & key) const;
        Any operator[](unsigned int index) const;
        unsigned int size() const;
        bool contains(const std::string & key) const;

        void add(const std::string & key, const Any & value);
        void add(const Any & value);

        void debug() const;

        friend Any readJson(const std::string & json, unsigned int & cursor);

        private:

        Any(ANY_TYPE type);

        ANY_TYPE type_{NONE};

        std::string str_{""};
        int int_{0};
        double dbl_{0.};
        bool bool_{true};
        std::map<std::string,Any> dict_{};
        std::vector<Any> array_{};
    };

    Any readJson(const std::string & path);
    Any readJsonStr(const std::string & json);
    Any readJson(const std::string & json, unsigned int & cursor);
    ANY_TYPE intToType(int type);
    int typeToInt(ANY_TYPE type);
    std::string readString(std::ifstream & in);

} // gcg

#endif //GCG_ANY_HPP
