/*
* 通用工具方法头文件
*
* 2015-2-5 By 宝爷
*/
#ifndef __COMM_TOOLS_H__
#define __COMM_TOOLS_H__

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <sstream>

#include "KxCSComm.h"

#ifndef RunningInServer
#endif 

inline void splitStringToVec(const std::string& str, const std::string& split, std::vector<std::string>& out)
{
    if (str.empty() || split.empty())
    {
        return;
    }

    size_t pos = 0;
    size_t offset = 0;
    while (true)
    {
        pos = str.find(split, offset);
        if (pos == std::string::npos)
        {
            out.push_back(str.substr(offset, str.length() - offset));
            break;
        }
        else
        {
            out.push_back(str.substr(offset, pos - offset));
            offset = split.length() + pos;
        }
    }
}

inline std::vector<std::string> splitStringToVec(const std::string& str, const std::string& split)
{
    std::vector<std::string> ret;
    splitStringToVec(str, split, ret);
    return ret;
}

template<typename T>
inline void releaseAndClearVec(std::vector<T>& l)
{
    for (typename std::vector<T>::iterator iter = l.begin();
        iter != l.end(); ++iter)
    {
        SAFE_RELEASE((*iter));
    }
    l.clear();
}

template<typename T>
inline void deleteAndClearVec(std::vector<T>& v)
{
	for (typename std::vector<T>::iterator iter = v.begin();
        iter != v.end(); ++iter)
    {
        SAFE_DELETE((*iter));
    }
    v.clear();
}

template<typename K, typename V>
inline void releaseAndClearMap(std::map<K, V>& m)
{
	for (typename std::map<K, V>::iterator iter = m.begin();
        iter != m.end(); ++iter)
    {
        SAFE_RELEASE((iter->second));
    }
    m.clear();
}

template<typename K, typename V>
inline void deleteAndClearMap(std::map<K, V>& m)
{
	for (typename std::map<K, V>::iterator iter = m.begin();
        iter != m.end(); ++iter)
    {
        SAFE_DELETE((iter->second));
    }
    m.clear();
}

template<typename T>
inline void releaseAndClearList(std::list<T>& l)
{
	for (typename std::list<T>::iterator iter = l.begin();
        iter != l.end(); ++iter)
    {
        SAFE_RELEASE((*iter));
    }
    l.clear();
}

template<typename T>
inline void deleteAndClearList(std::list<T>& l)
{
    for (typename std::list<T>::iterator iter = l.begin();
        iter != l.end(); ++iter)
    {
        SAFE_DELETE((*iter));
    }
    l.clear();
}

template<typename T>
inline bool eraseObjectInList(std::list<T>& l, T obj)
{
	for (typename std::list<T>::iterator iter = l.begin();
        iter != l.end(); ++iter)
    {
        if (*iter == obj)
        {
            l.erase(iter);
            return true;
        }
    }

    return false;
}

template<typename T>
inline bool eraseObjectInVec(std::vector<T>& l, T obj)
{
    for (typename std::vector<T>::iterator iter = l.begin();
        iter != l.end(); ++iter)
    {
        if (*iter == obj)
        {
            l.erase(iter);
            return true;
        }
    }

    return false;
}

template<typename T>
inline T toolAbs(T num)
{
    return num > 0 ? num : -num;
}

template<typename T>
inline std::string toolToStr(T var)
{
    std::string ret;
    std::stringstream st;
    st << var;
    st >> ret;
    return ret;
}

template<class T>
inline std::string toolToStr(T num, int width, char fill)
{
    std::string ret;
    std::stringstream st;
    st.width(width);
    st.fill(fill);
    st << num;
    st >> ret;
    return ret;
}


#define RadianToDegree	57.2957795130821975611438762422f

// 快速求出 1 / sqrt(x)，精确到小数点后3位
inline float fastInvSqrt(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;		        // get bits for floating VALUE
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;		        // convert bits BACK to float
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    return x;
}

// 快速求出 sqrt，精确到小数点后3位
inline float fastSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;              // get bits for floating VALUE         
    i = 0x5f3759df - (i >> 1);      // gives initial guess y0      
    x = *(float*)&i;                // convert bits BACK to float    
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    return (1 / x);
}

// 快速求出 1 / sqrt(x)，精确到小数点后10位
inline float preciseInvSqrt(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;		        // get bits for floating VALUE
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;		        // convert bits BACK to float
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    return x;
}

// 快速求出 sqrt，精确到小数点后10位
inline float preciseSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;              // get bits for floating VALUE         
    i = 0x5f3759df - (i >> 1);      // gives initial guess y0      
    x = *(float*)&i;                // convert bits BACK to float    
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    return (1 / x);
}

// 求两个向量的叉积 P1 X P2 = x1y2 - x2y1
inline float crossProduct(float x1, float y1, float x2, float y2)
{
    return x1*y2 - x2*y1;
}

// 求两条连续线段的方向 (P2 - P0) X (P1 - P0)
// 为正表示线段 P0P2 在线段 P0P1 的顺时针方向，为负表示线段 P0P2 在线段 P0P1 逆时针
inline float direction(float x0, float y0, float x1, float y1, float x2, float y2)
{
    return crossProduct(x2 - x0, y2 - y0, x1 - x0, y1 - y0);
}

// 求两条线段 P0P1 和 P2P3 是否相交
inline bool lineIntersect(float x0, float y0, float x1, float y1,
    float x2, float y2, float x3, float y3)
{
    float d1 = direction(x0, y0, x2, y2, x3, y3);
    float d2 = direction(x1, y1, x2, y2, x3, y3);
    float d3 = direction(x2, y2, x0, y0, x1, y1);
    float d4 = direction(x3, y3, x0, y0, x1, y1);

    //只考虑相交不考虑垂直
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0))
        && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
    {
        return true;
    }

    return false;
}

//检查两条线段是否相交
inline bool lineIntersect(const Vec2& p0, const Vec2& p1,
    const Vec2& p2, const Vec2& p3)
{
    float d1 = direction(p0.x, p0.y, p2.x, p2.y, p3.x, p3.y);
    float d2 = direction(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
    float d3 = direction(p2.x, p2.y, p0.x, p0.y, p1.x, p1.y);
    float d4 = direction(p3.x, p3.y, p0.x, p0.y, p1.x, p1.y);

    //只考虑相交不考虑垂直
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0))
        && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
    {
        return true;
    }

    return false;
}

#ifndef RunningInServer
//检查矩形和线段是否相交
inline bool rectIntersect(const Rect& rect,
    const Vec2& p0, const Vec2& p1)
{
    if (lineIntersect(p0.x, p0.y, p1.x, p1.y, rect.origin.x, rect.origin.y,
        rect.origin.x, rect.origin.y + rect.size.height))
    {
        return true;
    }
    else if (lineIntersect(p0.x, p0.y, p1.x, p1.y, rect.origin.x, rect.origin.y,
        rect.origin.x + rect.size.width, rect.origin.y))
    {
        return true;
    }
    else if (lineIntersect(p0.x, p0.y, p1.x, p1.y, rect.origin.x + rect.size.width, rect.origin.y,
        rect.origin.x + rect.size.width, rect.origin.y + rect.size.height))
    {
        return true;
    }
    else if (lineIntersect(p0.x, p0.y, p1.x, p1.y, rect.origin.x, rect.origin.y + rect.size.height,
        rect.origin.x + rect.size.width, rect.origin.y + rect.size.height))
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif 

// 数值上下限
template<typename T>
void limitValue(T& v, const T& min, const T& max)
{
    if (v < min)
    {
        v = min;
    }
    else if (v > max)
    {
        v = max;
    }
}

static float g_nQuickSin[360] = {
	0.000000f, 0.841471f, 0.909297f, 0.141120f, -0.756802f, -0.958924f, -0.279415f, 0.656987f, 0.989358f, 0.412118f,
	-0.544021f, -0.999990f, -0.536573f, 0.420167f, 0.990607f, 0.650288f, -0.287903f, -0.961397f, -0.750987f, 0.149877f,
	0.912945f, 0.836656f, -0.008851f, -0.846220f, -0.905578f, -0.132352f, 0.762558f, 0.956376f, 0.270906f, -0.663634f,
	-0.988032f, -0.404038f, 0.551427f, 0.999912f, 0.529083f, -0.428183f, -0.991779f, -0.643538f, 0.296369f, 0.963795f, 
	0.745113f, -0.158623f, -0.916522f, -0.831775f, 0.017702f, 0.850904f, 0.901788f, 0.123573f, -0.768255f, -0.953753f, 
	-0.262375f, 0.670229f, 0.986628f, 0.395925f, -0.558789f, -0.999755f, -0.521551f, 0.436165f, 0.992873f, 0.636738f, 
	-0.304811f, -0.966118f, -0.739181f, 0.167356f, 0.920026f, 0.826829f, -0.026551f, -0.855520f, -0.897928f, -0.114785f,
	0.773891f, 0.951055f, 0.253823f, -0.676772f, -0.985146f, -0.387782f, 0.566108f, 0.999520f, 0.513978f, -0.444113f, 
	-0.993889f, -0.629888f, 0.313229f, 0.968364f, 0.733190f, -0.176076f, -0.923458f, -0.821818f, 0.035398f, 0.860069f, 
	0.893997f, 0.105988f, -0.779466f, -0.948282f, -0.245252f, 0.683262f, 0.983588f, 0.379608f, -0.573382f, -0.999207f, 
	-0.506366f, 0.452026f, 0.994827f, 0.622989f, -0.321622f, -0.970535f, -0.727143f, 0.184782f, 0.926818f, 0.816743f, 
	-0.044243f, -0.864551f, -0.889996f, -0.097182f, 0.784980f, 0.945435f, 0.236661f, -0.689698f, -0.981952f, -0.371404f, 
	0.580611f, 0.998815f, 0.498713f, -0.459903f, -0.995687f, -0.616040f, 0.329991f, 0.972630f, 0.721038f, -0.193473f,
	-0.930106f, -0.811603f, 0.053084f, 0.868966f, 0.885925f, 0.088369f, -0.790433f, -0.942514f, -0.228052f, 0.696080f, 
	0.980240f, 0.363171f, -0.587795f, -0.998345f, -0.491022f, 0.467745f, 0.996469f, 0.609044f, -0.338333f, -0.974649f, 
	-0.714876f, 0.202150f, 0.933321f, 0.806401f, -0.061920f, -0.873312f, -0.881785f, -0.079549f, 0.795824f, 0.939520f, 
	0.219425f, -0.702408f, -0.978450f, -0.354910f, 0.594933f, 0.997797f, 0.483292f, -0.475550f, -0.997173f, -0.602000f,
	0.346649f, 0.976591f, 0.708659f, -0.210811f, -0.936462f, -0.801135f, 0.070752f, 0.877590f, 0.877575f, 0.070722f,
	-0.801153f, -0.936451f, -0.210781f, 0.708680f, 0.976584f, 0.346621f, -0.602024f, -0.997171f, -0.475524f, 0.483318f, 
	0.997799f, 0.594909f, -0.354938f, -0.978457f, -0.702386f, 0.219455f, 0.939530f, 0.795806f, -0.079579f, -0.881799f, 
	-0.873297f, -0.061890f, 0.806418f, 0.933310f, 0.202120f, -0.714898f, -0.974642f, -0.338305f, 0.609068f, 0.996467f,
	0.467719f, -0.491048f, -0.998347f, -0.587771f, 0.363199f, 0.980246f, 0.696059f, -0.228082f, -0.942525f, -0.790415f, 
	0.088399f, 0.885939f, 0.868951f, 0.053053f, -0.811621f, -0.930095f, -0.193444f, 0.721059f, 0.972623f, 0.329962f,
	-0.616064f, -0.995684f, -0.459877f, 0.498739f, 0.998817f, 0.580587f, -0.371432f, -0.981958f, -0.689676f, 0.236691f,
	0.945445f, 0.784962f, -0.097212f, -0.890009f, -0.864536f, -0.044213f, 0.816760f, 0.926807f, 0.184752f, -0.727163f, 
	-0.970528f, -0.321594f, 0.623012f, 0.994824f, 0.451999f, -0.506392f, -0.999208f, -0.573357f, 0.379636f, 0.983593f, 
	0.683240f, -0.245281f, -0.948292f, -0.779447f, 0.106017f, 0.894010f, 0.860054f, 0.035368f, -0.821835f, -0.923447f, 
	-0.176046f, 0.733211f, 0.968357f, 0.313200f, -0.629911f, -0.993885f, -0.444086f, 0.514004f, 0.999521f, 0.566083f, 
	-0.387809f, -0.985151f, -0.676750f, 0.253853f, 0.951064f, 0.773872f, -0.114815f, -0.897941f, -0.855504f, -0.026521f, 
	0.826846f, 0.920014f, 0.167326f, -0.739201f, -0.966110f, -0.304782f, 0.636761f, 0.992869f, 0.436138f, -0.521577f, 
	-0.999756f, -0.558764f, 0.395953f, 0.986633f, 0.670207f, -0.262404f, -0.953762f, -0.768235f, 0.123603f, 0.901801f, 
	0.850888f, 0.017672f, -0.831791f, -0.916510f, -0.158593f, 0.745133f, 0.963787f, 0.296340f, -0.643561f, -0.991775f, 
	-0.428155f, 0.529108f, 0.999912f, 0.551402f, -0.404065f, -0.988036f, -0.663611f, 0.270935f, 0.956385f, 0.762539f, 
	-0.132382f, -0.905591f, -0.846204f, -0.008821f, 0.836672f, 0.912933f, 0.149847f, -0.751007f, -0.961389f, -0.287874f, 
	0.650311f, 0.990603f, 0.420140f, -0.536598f, -0.999990f, -0.543996f, 0.412146f, 0.989363f, 0.656964f, -0.279444f, 
	-0.958933f, -0.756783f, 0.141150f, 0.909310f, 0.841455f, -0.000030f, -0.841487f, -0.909285f, -0.141090f, 0.756822f
};

static float g_nQuickCos[360] = {

	- 1.000000f, 0.540302f, -0.416147f, -0.989992f, -0.653644f, 0.283662f, 0.960170f, 0.753902f, -0.145500f, -0.911130f, 
	-0.839072f, 0.004426f, 0.843854f, 0.907447f, 0.136737f, -0.759688f, -0.957659f, -0.275163f, 0.660317f, 0.988705f,
	0.408082f, -0.547729f, -0.999961f, -0.532833f, 0.424179f, 0.991203f, 0.646919f, -0.292139f, -0.962606f, -0.748058f, 
	0.154251f, 0.914742f, 0.834223f, -0.013277f, -0.848570f, -0.903692f, -0.127964f, 0.765414f, 0.955074f, 0.266643f, 
	-0.666938f, -0.987339f, -0.399985f, 0.555113f, 0.999843f, 0.525322f, -0.432178f, -0.992335f, -0.640144f, 0.300593f, 
	0.964966f, 0.742154f, -0.162991f, -0.918283f, -0.829310f, 0.022127f, 0.853220f, 0.899867f, 0.119180f, -0.771080f, 
	-0.952413f, -0.258102f, 0.673507f, 0.985897f, 0.391857f, -0.562454f, -0.999647f, -0.517770f, 0.440143f, 0.993390f, 
	0.633319f, -0.309023f, -0.967251f, -0.736193f, 0.171717f, 0.921751f, 0.824331f, -0.030975f, -0.857803f, -0.895971f, 
	-0.110387f, 0.776686f, 0.949678f, 0.249540f, -0.680023f, -0.984377f, -0.383698f, 0.569750f, 0.999373f, 0.510177f, 
	-0.448074f, -0.994367f, -0.626444f, 0.317429f, 0.969459f, 0.730174f, -0.180430f, -0.925148f, -0.819288f, 0.039821f, 
	0.862319f, 0.892005f, 0.101586f, -0.782231f, -0.946868f, -0.240959f, 0.686487f, 0.982780f, 0.375510f, -0.577002f,
	-0.999021f, -0.502544f, 0.455969f, 0.995267f, 0.619521f, -0.325810f, -0.971592f, -0.724097f, 0.189129f, 0.928471f,
	0.814181f, -0.048664f, -0.866767f, -0.887969f, -0.092776f, 0.787715f, 0.943984f, 0.232359f, -0.692896f, -0.981106f,
	-0.367291f, 0.584209f, 0.998590f, 0.494872f, -0.463829f, -0.996088f, -0.612548f, 0.334165f, 0.973649f, 0.717964f,
	-0.197814f, -0.931722f, -0.809010f, 0.057503f, 0.871147f, 0.883863f, 0.083959f, -0.793136f, -0.941026f, -0.223741f, 
	0.699251f, 0.979355f, 0.359044f, -0.591370f, -0.998081f, -0.487161f, 0.471652f, 0.996831f, 0.605528f, -0.342495f,
	-0.975629f, -0.711775f, 0.206482f, 0.934900f, 0.803775f, -0.066337f, -0.875459f, -0.879689f, -0.075136f, 0.798496f, 
	0.937995f, 0.215105f, -0.705551f, -0.977527f, -0.350769f, 0.598484f, 0.997494f, 0.479412f, -0.479439f, -0.997496f, 
	-0.598460f, 0.350797f, 0.977533f, 0.705530f, -0.215135f, -0.938005f, -0.798478f, 0.075166f, 0.879703f, 0.875445f, 
	0.066307f, -0.803793f, -0.934890f, -0.206453f, 0.711796f, 0.975623f, 0.342466f, -0.605552f, -0.996829f, -0.471626f, 
	0.487188f, 0.998083f, 0.591345f, -0.359072f, -0.979361f, -0.699229f, 0.223770f, 0.941037f, 0.793118f, -0.083989f, 
	-0.883877f, -0.871133f, -0.057472f, 0.809028f, 0.931711f, 0.197784f, -0.717985f, -0.973642f, -0.334137f, 0.612572f, 
	0.996085f, 0.463802f, -0.494898f, -0.998592f, -0.584184f, 0.367319f, 0.981111f, 0.692874f, -0.232388f, -0.943994f,
	-0.787696f, 0.092806f, 0.887983f, 0.866752f, 0.048634f, -0.814198f, -0.928460f, -0.189100f, 0.724118f, 0.971585f, 
	0.325781f, -0.619544f, -0.995264f, -0.455942f, 0.502570f, 0.999022f, 0.576978f, -0.375538f, -0.982785f, -0.686465f,
	0.240988f, 0.946878f, 0.782212f, -0.101616f, -0.892018f, -0.862304f, -0.039791f, 0.819306f, 0.925136f, 0.180401f, 
	-0.730194f, -0.969452f, -0.317400f, 0.626468f, 0.994364f, 0.448047f, -0.510203f, -0.999374f, -0.569726f, 0.383726f, 
	0.984382f, 0.680001f, -0.249569f, -0.949687f, -0.776667f, 0.110417f, 0.895984f, 0.857788f, 0.030945f, -0.824348f, 
	-0.921740f, -0.171688f, 0.736213f, 0.967243f, 0.308994f, -0.633343f, -0.993387f, -0.440116f, 0.517796f, 0.999648f, 
	0.562429f, -0.391885f, -0.985902f, -0.673485f, 0.258131f, 0.952422f, 0.771061f, -0.119210f, -0.899880f, -0.853204f, 
	-0.022097f, 0.829327f, 0.918271f, 0.162961f, -0.742174f, -0.964958f, -0.300564f, 0.640167f, 0.992332f, 0.432151f,
	-0.525348f, -0.999844f, -0.555088f, 0.400013f, 0.987344f, 0.666916f, -0.266672f, -0.955083f, -0.765395f, 0.127994f,
	0.903705f, 0.848554f, 0.013247f, -0.834240f, -0.914730f, -0.154222f, 0.748078f, 0.962598f, 0.292110f, -0.646942f, 
	-0.991199f, -0.424152f, 0.532859f, 0.999961f, 0.547704f, -0.408110f, -0.988709f, -0.660294f, 0.275192f, 0.957668f, 
	0.759668f, -0.136767f, -0.907459f, -0.843838f, -0.004396f, 0.839088f, 0.911118f, 0.145470f, -0.753922f, -0.960162f,
	-0.283633f, 0.653666f, 0.989988f, 0.416119f, -0.540328f, -1.000000f, -0.540277f, 0.416174f, 0.989997f, 0.653621f
};
template<typename T>
inline float quickSin(T & v)
{
	return g_nQuickSin[(int)v % 360];
}
template<typename T>
inline float quickCos(T & v)
{
	return g_nQuickCos[(int)v % 360];
}


#endif
