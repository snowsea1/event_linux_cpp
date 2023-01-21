#include "config.h"
#include <fstream>


/**
 * @func: determine if a character is a space
 * @parm: c:character
 */
bool Config::is_space(char c)
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}

/**
 * @func: remove string space
 * @parm: str: string value
 */
void Config::remove_space(string &str)
{
    if (str.empty()) {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!is_space(str[i])) {
            break;
        }
    }
    if (i == str.size()) { // 全部是空白字符串
        str = "";
        return;
    }
    start_pos = i;
    for (i = str.size() - 1; i >= 0; --i) {
        if (!is_space(str[i])) {
            break;
        }
    }
    end_pos = i;
    str = str.substr(start_pos, end_pos - start_pos + 1);
}

/**
 * @func: analyse config line info
 * @parm: line: config line
 * @parm: key: line key
 * @parm: value: line value
 * @return: 0: on success
 */
bool Config::analyse_line(const string &line, string &key, string &value)
{
    /* invalid line data */
    if (line.empty()) {
        return false;
    }

    int start_pos = 0, end_pos = line.size() - 1, pos;

    /* handle commented info */
    if ((pos = line.find(COMMENT_CHAR)) != -1) {
        /* # is the first character */
        if (0 == pos) {
            return false;
        }
        end_pos = pos - 1;
    }

    /* delete commented info */
    string new_line = line.substr(start_pos, start_pos + 1 - end_pos);

    /* no "=" */
    if ((pos = new_line.find('=')) == -1) {
        return false; 
    }

    key = new_line.substr(0, pos);
    value = new_line.substr(pos + 1, end_pos + 1 - (pos + 1));

    /* remove spaces on both sides */
    remove_space(key);
    if (key.empty()) {
        return false;
    }
    remove_space(value);

    return true;
}

/**
 * @func: read config.ini 
 * @parm: filename:config name
 * @parm: map
 * @return: 0: on success
 */
bool Config::config_get(const string &filename, map<string, string> &map)
{
    /* clear map */
    map.clear();

    /* open file */
    ifstream infile(filename.c_str());
    if (!infile) {
        LOGE("file open error\n");
        return false;
    }

    /* get file data */
    string line, key, value;
    while (getline(infile, line)) {
        if (analyse_line(line, key, value)) {
            map[key] = value;
        }
    }
    infile.close();
    return true;
}

bool Config::config_set(const string &filename, map<string, string> &map, const string key_change)
{
    LOGE("lyh add -> config_set\n");

    /* open file */
    ifstream file(filename.c_str());
    if (!file) {
        LOGE("file open error\n");
        return false;
    }

    char filename_new[30];
    snprintf(filename_new, sizeof(filename_new),"%s%s", filename.c_str(), ".new");

    ofstream file_new(filename_new);
    if(!file_new) {
        LOGE("file new open error\n");
        return false;
    }

    /* get file data */
    string line, key, value;
    while (getline(file, line)) {
        cout << line << endl;

        if (analyse_line(line, key, value) == false) {
            file_new << line << endl;
            continue;
        }

        if (strncmp(key.c_str(), key_change.c_str(), strlen(key.c_str())) == 0) {
            LOGE("match key_change = %s\n", key.c_str());
            file_new << key_change << "=" << map_config[key_change] << endl;

        } else {
            file_new << line << endl;
        }
    }

    file.close();
    file_new.close();

    char filename_old[30];
    snprintf(filename_old, sizeof(filename_old),"%s%s", filename.c_str(), ".old");

    rename(filename.c_str(), filename_old);
    rename(filename_new, filename.c_str());
    

    return true;
}

/**
 * @func:  print map info
 * @parm: map
 * @return: 0: on success
 */
void Config::config_print(const map<string, string> &map_data)
{
    map<string, string>::const_iterator mite = map_data.begin();
    for (; mite != map_data.end(); ++mite) {
        cout << mite->first << "=" << mite->second << endl;

        /*// 把内容输出到新的文件里面去
        fout << mite->first << "=" << mite->second << endl;*/
    }
}

string Config::config_read(string key)
{
    return map_config[key];
}

int Config::config_write(string key, string value)
{
    map<string, string>::const_iterator mite = map_config.begin();
    for (; mite != map_config.end(); ++mite) {
        /* match key */
        if(strncmp(key.c_str(),mite->first.c_str(), strlen(mite->first.c_str())) == 0) {
            LOGE("match key = %s\n", key.c_str());

            /* match key-value */
            if(strncmp(value.c_str(), mite->second.c_str(), strlen(mite->second.c_str())) == 0) {
                LOGE("match key->value is same, ignore\n");
            }
            else {
                LOGE("match key->value is changed, %s->%s\n", mite->second.c_str(), value.c_str());
                map_config[key] = value;
                config_set(PATH_CONFIG, map_config, key);

            }
        }
    }

    return 0;

}

/**
 * @func: load config.ini 
 * @return: 0: on success
 */
int Config::config_init()
{    
    LOGE("enter config init, path = %s\n", PATH_CONFIG);

    string path_config = PATH_CONFIG;

    config_get(path_config, map_config);
    config_print(map_config);


    return 0;
}

Config::Config() {}
Config::~Config() {}