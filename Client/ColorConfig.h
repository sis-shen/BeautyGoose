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
    const QString main_color_light = "#FADD32";
    const QString light_grey = "#AAAAAA";
    const QString grey = "#555555";
    const QString background_color = "#F3F3F3";
    const QString background_color2 = "#F0F0F0";
    const QString hover_color = "#F4F4F4";
    const QString selected_color = "#E0E0E0";
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
