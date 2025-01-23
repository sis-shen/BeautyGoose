#ifndef COLORCONFIG_H
#define COLORCONFIG_H

#include <QString>
class ColorConfig
{
private:
    //实现单例模式
    ColorConfig() =default;
    ColorConfig(const ColorConfig&) = delete;
    ColorConfig operator=(const ColorConfig&) = delete;
    static ColorConfig* _ins;
public:
    static const ColorConfig* getInstance();

    ~ColorConfig()=default;

    const QString main_color = "#F4C81C";
    const QString light_grey = "#AAAAAA";
    const QString grey = "#555555";
    const QString background_color = "#F3F3F3";
private:
};

inline const ColorConfig *ColorConfig::getInstance()
{
    if(_ins == nullptr)
    {
        _ins = new ColorConfig;
    }
    return _ins;
}

#endif // COLORCONFIG_H
