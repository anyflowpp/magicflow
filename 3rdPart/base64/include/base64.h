#pragma once
#ifndef __BASE64_H__
#define __BASE64_H__
//
//  base64 encoding and decoding with C++.
//  Version: 1.01.00
//
#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif