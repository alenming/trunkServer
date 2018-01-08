#ifndef __CONF_MUSIC_H__
#define __CONF_MUSIC_H__

#include "ConfManager.h"

class BgMusicItem
{
public:
    int BgMusicID;           // 背景音乐ID
    int FadeInTime;          // 淡入时间(毫秒)
    int FadeOutTime;         // 淡出时间(毫秒)
    int IsRepeate;           // 是否循环
    std::string FileName;    // 文件名
};

class BgMusicSetting
{
public:
    int UIID;                // 场景标识ID
    int BgMusicID;           // 对应的背景音乐ID
    int EffectID;            // 打开UI播放音效
    std::vector<std::string> MoodEffect; // 打开UI播放氛围音效
};

class AudioItem
{
public:
    int         AudioID;                 // 音效id
    std::string AudioPath;               // 音效路径
    bool        isRuleAffect;            // 是否受到场景音效播放规则影响
};

///////////////////////////////// 读取 /////////////////////////////////////////
class CConfBgMusic : public CConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

class CConfBgMusicSetting : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

class CConfUIButtonEffect : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    int getButtonEffectId(std::string btnName);

protected:
    std::map<std::string, int> m_ButtonEffect;
};

class CConfUISoundEffect : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
    bool getEffectPath(int effectID, std::string& path);

protected:
    std::map<int, std::string> m_EffectPath;
};

class CConfAudio : public CConfBase
{
public:
    virtual bool LoadCSV(const std::string& str);
};

////////////////////////////////// 查询 ////////////////////////////////////////
inline const BgMusicItem* queryConfBgMusic(int bmgID)
{
    CConfBgMusic *conf = dynamic_cast<CConfBgMusic*>(
        CConfManager::getInstance()->getConf(CONF_BG_MUSIC));
    return static_cast<BgMusicItem*>(conf->getData(bmgID));
}

inline const BgMusicSetting* queryConfBgMusicSetting(int uiID)
{
    CConfBgMusicSetting *conf = dynamic_cast<CConfBgMusicSetting*>(
        CConfManager::getInstance()->getConf(CONF_BG_MUSIC_SETTING));
    return static_cast<BgMusicSetting*>(conf->getData(uiID));
}

inline const AudioItem* queryConfAudio(int musicId)
{
    CConfAudio *conf = dynamic_cast<CConfAudio*>(
        CConfManager::getInstance()->getConf(CONF_AUDIO));
    return static_cast<AudioItem*>(conf->getData(musicId));
}

#endif