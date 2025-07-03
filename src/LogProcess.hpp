#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

#ifdef min
#undef min
#endif


namespace LPWZ{

class LogProcessor {
    struct ChipRes {
        std::string Res;
        std::vector<std::string> chipData;
        int validCount = 0; // 新增：有效包数
    } ;
public:
    // 处理日志文件
    void processLog(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream inFile(inputFile);
        std::ofstream outFile(outputFile);
        
        if (!inFile.is_open() || !outFile.is_open()) {
            std::cerr << "Error opening files!" << std::endl;
            return;
        }

        //增加参数与条件缓存
        std::vector<std::string> paramBlockLines;
        bool readingParamBlock = false;
        int paramBlockLineCount = 0;

        std::string line;                                                                          // 用于存储每次读取的日志行
        bool inParameterBlock = false;                                                             // 标记当前是否在“参数下发”块内
        // std::map<std::string, std::vector<std::string>> chipData;                               // 按芯片编号存储数据
        std::map<std::string, ChipRes> chipData;                                                   // 按芯片编号存储每个芯片的结果和数据
        std::string currentTimestamp;                                                              // 当前参数块的时标

        while (std::getline(inFile, line)) {                                                       // 逐行读取输入文件
            if (line.find("[本组参数下发开始:") != std::string::npos)                                      // 检查是否为参数块的开始
            {
                inParameterBlock = true;                                                           // 进入参数块
                outFile << line << "\n\n";                                                         // 原样输出该行并空一行
                //增加参数与条件缓存
                paramBlockLines.clear();
                readingParamBlock = false;
                paramBlockLineCount = 0;

                continue;                                                                          // 进入下一行处理

            }

            if (inParameterBlock) {
                // 2. 识别并缓存参数与条件内容
                if (!readingParamBlock && line.find("组参数与条件：") != std::string::npos) {
                    readingParamBlock = true;
                    paramBlockLineCount = 0;
                }
                if (readingParamBlock && paramBlockLineCount < 4) 
                {
                    paramBlockLines.push_back(line);
                    paramBlockLineCount++;
                    if (paramBlockLineCount == 4) {
                        readingParamBlock = false;
                    }
                   
                    
                    continue;
                }
            }

            if (line.find("[本组参数下发结束:") != std::string::npos) {
                inParameterBlock = false;
                // 新增：统计每个芯片编号的有效包数
                int currentDur = 1; // 默认值
                for (const auto& paramLine : paramBlockLines) {
                    size_t pos = paramLine.find("Dur=");
                    if (pos != std::string::npos) {
                        std::istringstream iss(paramLine.substr(pos + 4));
                        iss >> currentDur;
                        break;
                    }
                }
                for (auto& pair : chipData) {
                    int validCountCodec = 0;
                    int validCountAdPow = 0;
                    bool startCountingCodec = false;
                    bool startCountingAdPow = false;
                    const auto& dataVec = pair.second.chipData;

                    // 统计Codec Left + Codec Right（每2行一组）
                    for (size_t i = 0; i + 1 < dataVec.size(); i += 3) {
                        // 假设第i行为Codec Left，第i+1行为Codec Right
                        bool bothTrue = true;
                        for (int j = 0; j < 2; ++j) {
                            if (dataVec[i + j].find("Res: TRUE") == std::string::npos) {
                                bothTrue = false;
                                break;
                            }
                        }
                        if (bothTrue) {
                            if (!startCountingCodec) startCountingCodec = true;
                            if (startCountingCodec) ++validCountCodec;
                        }
                        else {
                            if (startCountingCodec) break;
                        }
                        // 跳到下一个三元组
                    }

                    // 统计AdPow（每3行的第3行为一组）
                    for (size_t i = 2; i < dataVec.size(); i += 3) {
                        if (dataVec[i].find("Res: TRUE") != std::string::npos) {
                            if (!startCountingAdPow) startCountingAdPow = true;
                            if (startCountingAdPow) ++validCountAdPow;
                        }
                        else {
                            if (startCountingAdPow) break;
                        }
                    }

                    int validCount = std::min(validCountCodec, validCountAdPow);
                    pair.second.validCount = validCount;
                    pair.second.Res = (validCount >= currentDur * 125 - 1) ? "成功" : "失败";
                }
                // 3. 输出每个芯片编号前先输出参数与条件
                for (const auto& pair : chipData) {
                    for (const auto& paramLine : paramBlockLines) {
                        outFile << paramLine << "\n";
                    }
                    outFile << pair.first << ", ";
                    outFile << "Res:" << pair.second.Res << "，";
                    outFile << " 有效包数：" << pair.second.validCount << "\n";
                    for (const auto& data : pair.second.chipData) {
                        outFile << data << "\n";
                    }
                }
                outFile << "\n" << line << "\n";
                chipData.clear();
                paramBlockLines.clear(); // 4. 清空
                continue;
            }

            if (inParameterBlock)                                                                  // 如果当前在参数块内
            {
                if (line.find("测试次数：") != std::string::npos)                                       // 查找“测试次数：”关键字
                {
                                                                                                   // 提取时间戳
                    size_t pos = line.find("时标:");                                                 // 查找“时标:”关键字
                    if (pos != std::string::npos)
                    {
                        currentTimestamp = line.substr(pos + 7);                                   // 提取时标后的内容作为当前时间戳
                    }
                    continue;                                                                      // 处理下一行
                }

                if (line.find("芯片编号：") != std::string::npos)                                       // 查找“芯片编号：”关键字
                {
                                                                                                   // 提取芯片编号作为key
                    std::string chipId = line.substr(line.find("芯片编号："));                          // 以“芯片编号：”及其后内容作为芯片编号


                                                                                                   // 读取接下来的三行数据并添加时间戳
                    ChipRes chipRes;                                                               // 新建一个芯片结果结构体
                    bool isRes = true;                                                             // 标记该芯片整体结果，初始为true
                    for (int i = 0; i < 3; i++)                                                    // 读取三行数据
                    {
                        if (std::getline(inFile, line))                                            // 读取一行
                        {
                                                                                                   // 添加时间戳
                            size_t bracketPos = line.find('[');                                    // 查找第一个'['
                            if (bracketPos != std::string::npos)
                            {
                                line.insert(bracketPos + 1, "Timstamp:" + currentTimestamp + ", ");// 在'['后插入时间戳
                            }
                            size_t resultPos = line.find("FALSE");                                 // 查找"FALSE"
                            if (resultPos != std::string::npos)                                    // 只要有一行出现"FALSE"，整体结果为失败
                            {
                                isRes = false;
                            }
                            chipRes.chipData.push_back(line);                                      // 保存该行到chipData

              
                        }
                    }
                    
                
                  
                    chipData[chipId].chipData.insert(chipData[chipId].chipData.end(), chipRes.chipData.begin(), chipRes.chipData.end());
                }
            } else {
                // 不在参数块中的内容直接写入
                outFile << line << "\n";
            }
        }

        inFile.close();
        outFile.close();
    }
};

// 使用示例
// int main() {
//     LogProcessor processor;
//     processor.processLog("LogUpRecord.log", "ProcessedLogUpRecord.log");
//     std::cout << "Log processing completed!" << std::endl;
//     return 0;
// }
};