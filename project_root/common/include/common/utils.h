#pragma once
#include <vector>
#include <string>

// 데모용 간단 유틸
int add(int a, int b);

// 정수 벡터를 "1, 2, 3" 형태로 이어붙이기
std::string joinInts(const std::vector<int>& v, const std::string& sep);
