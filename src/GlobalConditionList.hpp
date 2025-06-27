#pragma once
#include <QVector>
#include <utility>
#include "TestCondition.hpp"
#include "DataConstruct.hpp" // 假设 ARG_RTC_GENERATE 定义在这里

extern QVector<DCWZ::ARG_RTC_GENERATE> g_ParamList; // 新增：全局参数序列
extern QVector<std::pair<TCOND::TestCondition, TCOND::TestCondition>> g_ConditionList;