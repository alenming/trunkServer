#include "ConfMusic.h"


bool CConfBgMusic::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    while (csvLoader.NextLine())
    {
        BgMusicItem *item = new BgMusicItem;
        item->BgMusicID = csvLoader.NextInt();
        item->FileName = csvLoader.NextStr();
        item->FadeInTime = csvLoader.NextInt();
        item->FadeOutTime = csvLoader.NextInt();
        item->IsRepeate = csvLoader.NextInt();

        m_Datas[item->BgMusicID] = item;
    }

    return true;
}

bool CConfBgMusicSetting::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    while (csvLoader.NextLine())
    {
        BgMusicSetting *item = new BgMusicSetting;
        item->UIID = csvLoader.NextInt();
        csvLoader.NextStr(); // 备注
        item->BgMusicID = csvLoader.NextInt();
        item->EffectID = csvLoader.NextInt();
        CConfAnalytic::ToJsonStr(csvLoader.NextStr(), item->MoodEffect);

        m_Datas[item->UIID] = item;
    }

    return true;
}

bool CConfUIButtonEffect::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    while (csvLoader.NextLine())
    {
        std::string btnName = csvLoader.NextStr();
        csvLoader.NextStr(); // 备注
        int nEffectID = csvLoader.NextInt();

        m_ButtonEffect[btnName] = nEffectID;
    }

    return true;
}

int CConfUIButtonEffect::getButtonEffectId(std::string btnName)
{
    std::map<std::string, int>::iterator iter = m_ButtonEffect.find(btnName);
    if (iter != m_ButtonEffect.end())
    {
        return iter->second;
    }

    return 0;
}

bool CConfUISoundEffect::LoadCSV(const std::string& str)
{
    CCsvLoader csvLoader;
    CHECK_RETURN(loadCsv(csvLoader, str.c_str(), 3));

    while (csvLoader.NextLine())
    {
        int nEffectID = csvLoader.NextInt();
        csvLoader.NextStr(); // 备注
        std::string effectPath = csvLoader.NextStr();

        m_EffectPath[nEffectID] = effectPath;
    }

    return true;
}

bool CConfUISoundEffect::getEffectPath(int effectID, std::string& path)
{
    std::map<int, std::string>::iterator iter = m_EffectPath.find(effectID);
    if (iter != m_EffectPath.end())
    {
        path = iter->second;
        return true;
    }

    return false;
}

bool CConfAudio::LoadCSV(const std::string& str)
{
    CCsvLoader pLoader;
    CHECK_RETURN(pLoader.LoadCSV(str.c_str()));
    while (pLoader.NextLine())
    {
        AudioItem *item = new AudioItem;
        item->AudioID = pLoader.NextInt();
        // 跳过备注
        pLoader.NextStr();
        item->AudioPath = pLoader.NextStr();
        item->isRuleAffect = pLoader.NextInt() == 1 ? true : false;

        m_Datas[item->AudioID] = item;
    }
    return true;
}