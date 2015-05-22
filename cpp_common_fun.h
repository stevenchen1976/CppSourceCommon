/***************************************
* @file     cpp_common_fun.h
* @brief    C/C++常用功能函数。
* @details  实现了：
            浮点数的四舍五入
			分割字符串，替换字符串，去除前后空格
			得到当前时间，时间增减,ScopeExit
* @author   phata, wqvbjhc@gmail.com
* @date     2014-3-27
* @modify   2014-10-27  phata  添加GetCurTime,ScopeExit,去除string前后空格
            2015-03-23  phata  添加fast_log,fast_log10,fast_log2,fast_exp,InvSqrt函数
			2014-03-25  phata  DataCompare添加有bug. double相减保存于int,溢出会变为负，改为保存于double
****************************************/
#ifndef C_CPP_COMMON_FUNCTION_H
#define C_CPP_COMMON_FUNCTION_H
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#ifdef _MSC_VER
#pragma warning(disable:4996)// '...': This function or variable may be unsafe.
#endif // _MSC_VER
namespace Phata
{
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////字符相关操作/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/*****************************
* @brief   分割字符串，支持多个分割符
* @param   srcstr  -- 要分割的字符串
* @param   delimiter  -- 分割符，可多个。如":,|"
* @param   result  -- 返回的分割结果
* @return  返回结果保存在result中
******************************/
inline void split(const std::string& srcstr, const std::string& delimiter, std::vector<std::string>& result)
{
    result.clear();
    std::string _srccode = srcstr;//保存副本，对副本处理，会修改副本
    const char* p;
    for (p = strtok((char*)_srccode.c_str(), delimiter.c_str());  p;  p = strtok(NULL,  delimiter.c_str())) {
        result.push_back(p);
    }
}

/*****************************
* @brief   把strBase中的strSrc替换为strRep
* @param   strBase  -- 要被替换的字符串，此串最后保存替换后的结果
* @param   strSrc  -- 原始字符
* @param   strRep  -- 要替换后的字符
* @return  返回替换后的结果，等同于strBase
******************************/
inline std::string StringReplace(std::string& strBase, const std::string& strSrc, const std::string& strRep)
{
    std::string::size_type pos = 0;
    std::string::size_type srcLen = strSrc.size();
    std::string::size_type desLen = strRep.size();
    pos = strBase.find(strSrc, pos);
    while ((pos != std::string::npos)) {
        strBase.replace(pos, srcLen, strRep);
        pos = strBase.find(strSrc, (pos + desLen));
    }
    return strBase;
}

/*****************************
* @brief   去除字符串前后空格
* @param   strBase  -- 要被去除的字符串，此串最后保存去除后的结果
* @return  返回去除后的结果，等同于strBase
******************************/
inline std::string& trim(std::string& strBase)
{
    if (strBase.empty()) {
        return strBase;
    }
    strBase.erase(0, strBase.find_first_not_of(" \t\n\r"));
    strBase.erase(strBase.find_last_not_of(" \t\n\r") + 1);
    return strBase;
}

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////时间相关操作/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/*****************************
* @brief   把时间struct tm添加年、月、日
* @param   srctm  -- 在此时间上添加
* @param   year   -- 年份，为0不变，正数表增加，负数表减少
* @param   month  -- 月份，为0不变，正数表增加，负数表减少
* @param   day    -- 天数，为0不变，正数表增加，负数表减少
* @return  返回改变后的结果
******************************/
inline struct tm AddTimeInterval(struct tm srctm, int year, int month, int day)
{
    srctm.tm_year += year;
    srctm.tm_mon += month;
    srctm.tm_mday += day;
    mktime(&srctm);
    return srctm;
}

/*****************************
* @brief   得到当前时间，格式为YYYY-MM-DD hh:mm:ss(19个字符)
* @param   无
* @return  返回当前时间字符串
******************************/
inline std::string GetCurTime(void)
{
    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo = localtime(&rawtime);
    char curtime[20] = {0};
    strftime(curtime, sizeof(curtime), "%Y-%m-%d %H:%M:%S", timeinfo);
    return curtime;
}

/*****************************
* @brief   比较两个日期的大小，支持大日期比较，如9999-1-1 10:10 10
* @param   date1  -- 日期1
* @param   date2  -- 日期2
* @return  返回值：date1 0(相等) 1(大于) -1(小于) date2
******************************/
inline int DataCompare(const tm& date1, const tm& date2)
{
    char szDate1[32];
    char szDate2[32];
    sprintf(szDate1, "%04d%02d%02d%02d%02d%02d", date1.tm_year, date1.tm_mon, date1.tm_mday, date1.tm_hour, date1.tm_min, date1.tm_sec);
    sprintf(szDate2, "%04d%02d%02d%02d%02d%02d", date2.tm_year, date2.tm_mon, date2.tm_mday, date2.tm_hour, date2.tm_min, date2.tm_sec);
    double time1 = atof(szDate1);
    double time2 = atof(szDate2);
    double result = time1 - time2;
    if (result > 0)			return 1;
    else if (result < 0)	return -1;
    return 0;
}
/*未注册版本--添加时间判断
struct tm* local;
time_t t;
t = time(NULL);
local = localtime(&t);
local->tm_year += 1900;
local->tm_mon += 1;
struct tm deadline;
memset(&deadline, 0, sizeof(struct tm));
deadline.tm_year = 2020;
deadline.tm_mon = 03;
deadline.tm_mday = 17;
if (Phata::DataCompare(*local, deadline) == 1) {
    fprintf(stdout, "软件已过期，请联系作者\n");
    LOGE("软件已过期，请联系作者");
    continue;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////数值计算/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/*****************************
* @brief   double的四舍五入
* @param   dval  -- 原始数值
* @param   decDigits  -- 精确的位数
* @return  返回结果数值
******************************/
inline double dround(const double dval, int decDigits)
{
    double dRetval;
    double dMod = 0.0000001;
    if (dval < 0.0) dMod = -0.0000001;
    dRetval = dval;
    dRetval += (5.0 / pow(10.0, decDigits + 1.0));
    dRetval *= pow(10.0, decDigits);
    dRetval = floor(dRetval + dMod);
    dRetval /= pow(10.0, decDigits);
    return (dRetval);

    //if (decDigits<0) {
//       decDigits=0;
//   }
//   double result=1;
//   while(decDigits--) {
//       result*=10;
//   }
//   return int(dval*result+0.5)/result;
}

/*****************************
* @brief   double的四舍五入
* @param   dval  -- 原始数值
* @param   decDigits  -- 精确的位数
* @return  返回结果字符串
******************************/
inline std::string pround(const double dval, int decDigits)
{
    std::stringstream ss;
    ss << std::fixed;
    ss.precision(decDigits); // set # places after decimal
    ss << dval;
    return ss.str();
}

/*****************************
* @brief   快速对数函数ln(x)-log(x)函数:fast_log, fast_log10
    boost库对该函数的实现 http://www.boost.org/doc/libs/master/boost/intrusive/detail/math.hpp
    boost表明引用来源于    http://www.flipcode.com/archives/Fast_log_Function.shtml
    最后引用到文章   http://www.icsi.berkeley.edu/pubs/techreports/TR-07-002.pdf（原文见附件）
    http://stackoverflow.com/questions/4461506/im-in-need-for-a-floating-point-expert 解释快速原因如下：
    IEEE floats internally have an exponent E and a mantissa M, each represented as binary integers. The actual value is basically
    2^E * M
    Basic logarithmic math says:
    log2(2^E * M)
    = log2(2^E) + log2(M)
    = E + log2(M)
    The first part of your code separates E and M. The line commented (1) computes log2(M) by using a polynomial approximation. The final line adds E and the result of the approximation.
    The line (1) computes 1+log2(m), m ranging from 1 to 2. The proposed formula is a 3rd degree polynomial keeping first derivate continuity. Higher degree could be used for more accuracy. For faster results, one can remove this line, if accuracy is not the matter (it gives some linear interpolation between powers of 2).
    
	循环100次测试输出结果
	ln(x) on math.h
	mathlog Time-consuming  0.007697s
	ln(x) on fast_log
	fast_log Time-consuming 0.000962s--会快
	ln(0.005000) math -5.298317, fast_log -5.304517, diff 0.006199--精度有误差

	log10(x) on math.h
	mathlog10 Time-consuming  0.001283s
	log10(x) on fast_log
	fast_log10 Time-consuming 0.000641s--会快
	log10(0.005000) mathlog10 -2.301030, fast_log10 -2.303722, diff 0.002692--精度有误差

* @param   val  -- 要求的log(x)
* @return  返回log(x)
******************************/
inline float fast_log2(float val)
{
    union caster_t {
        unsigned x;
        float val;
    } caster;

    caster.val = val;
    unsigned x = caster.x;
    const int log_2 = int((x >> 23) & 255) - 128;
    x &= ~(unsigned(255u) << 23u);
    x += unsigned(127) << 23u;
    caster.x = x;
    val = caster.val;
    //1+log2(m), m ranging from 1 to 2
    //3rd degree polynomial keeping first derivate continuity.
    //For less precision the line can be commented out
    val = ((-1.f / 3.f) * val + 2.f) * val - (2.f / 3.f);
    return (val + log_2);
}

inline float fast_log(const float val)
{
    return (fast_log2(val) * 0.69314718f); //ln(x)=log2(x)*ln(2)=log2(x)0.69314718f
}

inline float fast_log10(const float val)
{
    //return fast_log2(val) / 3.312500f;//log10(x)=log2(x)/(ln(10)/ln(2))=log2(x)/3.32192809489(不知为何是3.312500f)=log2(x)*0.30102999566
    return fast_log2(val) * 0.30102999566;
}

/*****************************
* @brief   快速指数函数exp(x)函数:fast_exp(x)
    代码出处：https://github.com/thorbenk/saliencyfilters/tree/master/src/saliency

	循环100次测试输出结果(SSE版本未测试)
	exp(x) on math.h
	mathexp Time-consuming  0.001283s
	exp(x) on fast_exp
	fast_exp Time-consuming 0.000962s--会快
	exp(x) on fast_exp_sse
	exp(0.005000) mathexp 1.005013, fast_exp 1.005013, diff 0.000000--小精度一致
* @param   val  -- 要求的log(x)
* @return  返回log(x)
******************************/

inline float very_fast_exp(float x)
{
    return 1
           - x * (0.9999999995f
           - x * (0.4999999206f

           - x * (0.1666653019f
           - x * (0.0416573475f
           - x * (0.0083013598f

           - x * (0.0013298820f
           - x * (0.0001413161f)))))));
}

inline float fast_exp(float x)
{
    bool lessZero = true;
    if (x < 0) {
        lessZero = false;
        x = -x;
    }
    // This diry little trick only works because of the normalization and the fact that one element in the normalization is 1
    if (x > 20){
        return 0;
	}
    int mult = 0;

    while (x > 0.69 * 2 * 2 * 2) {
        mult += 3;
        x /= 8.0f;
    }

    while (x > 0.69 * 2 * 2) {
        mult += 2;
        x /= 4.0f;
    }
    while (x > 0.69) {
        mult++;
        x /= 2.0f;
    }

    x = very_fast_exp(x);
    while (mult) {
        mult--;
        x = x * x;
    }

    if (lessZero) {
        return 1 / x;
    } else {
        return x;
    }
}

#if (defined(__GNUC__) && defined(__SSE__)) || (defined(_MSC_VER) && _MSC_VER >=1500)//VC2008
#include <xmmintrin.h>
#include <mmintrin.h>
inline __m128 operator*( __m128 a, __m128 b ){
	return _mm_mul_ps( a, b );
}
inline __m128 operator/( __m128 a, __m128 b ){
	return _mm_div_ps( a, b );
}
inline __m128 operator+( __m128 a, __m128 b ){
	return _mm_add_ps( a, b );
}
inline __m128 operator-( __m128 a, __m128 b ){
	return _mm_sub_ps( a, b );
}
inline __m128 operator*=( __m128 &a, __m128 b ){
	return a=_mm_mul_ps( a, b );
}
inline __m128 operator/=( __m128 &a, __m128 b ){
	return a=_mm_div_ps( a, b );
}
inline __m128 operator+=( __m128 &a, __m128 b ){
	return a=_mm_add_ps( a, b );
}
inline __m128 operator-=( __m128 &a, __m128 b ){
	return a=_mm_sub_ps( a, b );
}
inline __m128 operator-( __m128 a ){
	return _mm_sub_ps( _mm_set1_ps(0), a );
}
inline __m128 very_fast_exp_sse(__m128 x)
{
    return _mm_set1_ps(1)
           - x * (_mm_set1_ps(0.9999999995)
           - x * (_mm_set1_ps(0.4999999206)

           - x * (_mm_set1_ps(0.1666653019)
           - x * (_mm_set1_ps(0.0416573475)
           - x * (_mm_set1_ps(0.0083013598)

           - x * (_mm_set1_ps(0.0013298820)
           - x * (_mm_set1_ps(0.0001413161))))))));
}
inline __m128 fast_exp_sse(__m128 x)
{
    const __m128 zero = _mm_set1_ps(0.0f);
    __m128 lessZero = _mm_cmplt_ps(zero, x);
    __m128 one      = _mm_set1_ps(1.0);
    // Invert x of less than 0
    x = _mm_add_ps(_mm_and_ps(lessZero, x), _mm_andnot_ps(lessZero, -x));

    // This diry little trick only works because of the normalization and then fact that one element in the normalization is 1
    __m128 set_zero = _mm_cmple_ps(x, _mm_set1_ps(20.0));

    // Bring it into range
    __m128 mult = zero;
    for (int i = 4, j = 2; i > 0; i /= 2, j--) {
        __m128 cmp = _mm_cmpgt_ps(x, _mm_set1_ps(0.69 * i));
        mult += _mm_and_ps(cmp, _mm_set1_ps(j + 1));
        x = x / (_mm_and_ps(cmp, _mm_set1_ps(2 * i - 1)) + one);
    }

    x = very_fast_exp_sse(x);

    while (1) {
        __m128 cmp = _mm_cmpgt_ps(mult, _mm_set1_ps(0.0));
        if (!_mm_movemask_ps(cmp)) break;
        mult -= _mm_and_ps(cmp, one);
        x *= (_mm_and_ps(cmp, x - one) + one);
    }

    x = _mm_add_ps(_mm_and_ps(lessZero, one / x), _mm_andnot_ps(lessZero, x));
    return _mm_and_ps(set_zero, x);
}
#endif
/*fast_log,fast_exp测试代码如下
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "opencv2/core/core.hpp"
#include "cpp_common_fun.h"
#include "cpp_common_fun.h"
using namespace cv;

int main(int argc, char** argv)
{
double testval = 0.005;

printf("ln(x) on math.h\n");
double exec_time = (double)getTickCount();
double dmathln;
for (int i = 0; i < 100; ++i) {
dmathln = log(testval);
}
exec_time = ((double)getTickCount() - exec_time) * 1000. / getTickFrequency();
printf("%s Time-consuming  %fs\n", "mathlog", exec_time);
printf("ln(x) on fast_log\n");
exec_time = (double)getTickCount();
double dfastln;
for (int i = 0; i < 100; ++i) {
dfastln = Phata::fast_log((float)testval);
}
exec_time = ((double)getTickCount() - exec_time) * 1000. / getTickFrequency();
printf("%s Time-consuming %fs\n", "fast_log", exec_time);
printf("ln(%f) math %f, fast_log %f, diff %f\n", testval, dmathln, dfastln, dmathln - dfastln);


printf("\nlog10(x) on math.h\n");
exec_time = (double)getTickCount();
for (int i = 0; i < 100; ++i) {
dmathln = log10(testval);
}
exec_time = ((double)getTickCount() - exec_time) * 1000. / getTickFrequency();
printf("%s Time-consuming  %fs\n", "mathlog10", exec_time);
printf("log10(x) on fast_log\n");
exec_time = (double)getTickCount();
for (int i = 0; i < 100; ++i) {
dfastln = Phata::fast_log10((float)testval);
}
exec_time = ((double)getTickCount() - exec_time) * 1000. / getTickFrequency();
printf("%s Time-consuming %fs\n", "fast_log10", exec_time);
printf("log10(%f) mathlog10 %f, fast_log10 %f, diff %f\n", testval, dmathln, dfastln, dmathln - dfastln);

printf("\nexp(x) on math.h\n");
exec_time = (double)getTickCount();
for (int i = 0; i < 100; ++i) {
dmathln = exp(testval);
}
exec_time = ((double)getTickCount() - exec_time) * 1000. / getTickFrequency();
printf("%s Time-consuming  %fs\n", "mathexp", exec_time);
printf("exp(x) on fast_exp\n");
exec_time = (double)getTickCount();
for (int i = 0; i < 100; ++i) {
dfastln = Phata::fast_exp((float)testval);
}
exec_time = ((double)getTickCount() - exec_time) * 1000. / getTickFrequency();
printf("%s Time-consuming %fs\n", "fast_exp", exec_time);
printf("exp(x) on fast_exp_sse\n");
exec_time = (double)getTickCount();
printf("exp(%f) mathexp %f, fast_exp %f, diff %f\n", testval, dmathln, dfastln, dmathln - dfastln);

return 0;
}
*/


/*****************************
* @brief   平方根倒数 1/sqrt(x)，比系统实现的还快
    wiki介绍 http://en.wikipedia.org/wiki/Fast_inverse_square_root
* @param   x  -- 输入的x
* @return  返回1/sqrt(x)
******************************/
inline float InvSqrt(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x; // get bits for floating VALUE
	i = 0x5f375a86 - (i >> 1); // gives initial guess y0
	x = *(float*)&i; // convert bits BACK to float
	x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
	//x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy --this can be removed
	return x;
}

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////其他/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/*****************************
ScopeExit,出处：http://the-witness.net/news/2012/11/scopeexit-in-c11/?utm_source=tuicool
作用：函数结束时自动调用某函数(释放资源的函数)，需要到C++11中lambda支持
******************************/
template <typename F>
struct ScopeExit {
    ScopeExit(F f) : f(f) {}
    ~ScopeExit()
    {
        f();
    }
    F f;
};

template <typename F>
ScopeExit<F> MakeScopeExit(F f)
{
    return ScopeExit<F>(f);
};

#define makestring2(x) #x
#define makestring(x) makestring2(x)
/*
#pragma message(__FILE__ "(" makestring(__LINE__) \
	") : _MSC_VER(" makestring(_MSC_VER) ") had defined")
*/

#define DO_STRING_JOIN2(arg1,arg2) arg1##arg2
#define STRING_JOIN2(arg1,arg2) DO_STRING_JOIN2(arg1,arg2)

/* = or & ? */
#define SCOPE_EXIT(code) \
	auto STRING_JOIN2(scope_exit_, __LINE__) = Phata::MakeScopeExit([&](){code;})
/* 用法
int * ip = new int[16];
SCOPE_EXIT(delete [] ip);

FILE * fp = fopen("test.out", "wb");
SCOPE_EXIT(fclose(fp));
*/

}
#endif//C_CPP_COMMON_FUNCTION_H