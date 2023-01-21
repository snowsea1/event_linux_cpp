#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "base_common.h"


#define COMMENT_CHAR    '#'
#define PATH_CONFIG     "./config.ini"




class Config {

  public:
    Config();
    ~Config();
    int config_init();
    string config_read(string key);
    int config_write(string key, string value);

  private:
    map<string, string> map_config;
    bool is_space(char c);
    void remove_space(string &str);
    bool analyse_line(const string &line, string &key, string &value);
    bool config_get(const string &filename, map<string, string> &map);
    bool config_set(const string &filename, map<string, string> &map, const string map_key);
    void config_print(const map<string, string> &map_data);
};













#endif //_CONFIG_H_