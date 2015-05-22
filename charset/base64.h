/***************************************
* @file     base64.h
* @brief    base64编解码函数,url编解码函数
* @details
* @author   phata, wqvbjhc@gmail.com
* @date     2014-4-2
****************************************/
#ifndef _BASE64_H_
#define _BASE64_H_

#include <string>

std::string Base64Encode(const unsigned char *bytes_to_encode, unsigned int in_len);
std::string Base64Decode(const unsigned char *encoded_string,  unsigned int in_len);
std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);

#endif