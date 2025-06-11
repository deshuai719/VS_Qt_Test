#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

namespace LPWZ{

class LogProcessor {
    typedef struct{
        std::string Res;
        std::vector<std::string> chipData;
    } ChipRes;
public:
    // 处理日志文件
    void processLog(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream inFile(inputFile);
        std::ofstream outFile(outputFile);
        
        if (!inFile.is_open() || !outFile.is_open()) {
            std::cerr << "Error opening files!" << std::endl;
            return;
        }

        std::string line;
        bool inParameterBlock = false;
        // std::map<std::string, std::vector<std::string>> chipData; // 按芯片编号存储数据
        std::map<std::string, ChipRes> chipData; // 按芯片编号存储数据
        std::string currentTimestamp;

        while (std::getline(inFile, line)) {
            if (line.find("[本组参数下发开始:") != std::string::npos) {
                inParameterBlock = true;
                outFile << line << "\n\n";
                continue;
            }

            if (line.find("[本组参数下发结束:") != std::string::npos) {
                inParameterBlock = false;
                
                // 输出排序后的芯片数据
                for (const auto& pair : chipData) {
                    outFile << pair.first << ", "; // 芯片编号
                    outFile << "Res:" << pair.second.Res << "\n"; // 芯片结果
                    for (const auto& data : pair.second.chipData) {
                        outFile << data << "\n";
                    }
                }
                
                outFile << "\n" << line << "\n";
                chipData.clear();
                continue;
            }

            if (inParameterBlock) {
                if (line.find("测试次数：") != std::string::npos) {
                    // 提取时间戳
                    size_t pos = line.find("时标:");
                    if (pos != std::string::npos) {
                        currentTimestamp = line.substr(pos + 7);
                    }
                    continue;
                }

                if (line.find("芯片编号：") != std::string::npos) {
                    // 提取芯片编号作为key
                    std::string chipId = line.substr(line.find("芯片编号："));
                    
                    // 读取接下来的三行数据并添加时间戳
                    // std::vector<std::string> chipLines;
                    ChipRes chipRes;
                    bool isRes = true;
                    for (int i = 0; i < 3; i++) {
                        if (std::getline(inFile, line)) {
                            // 添加时间戳
                            size_t bracketPos = line.find('[');
                            if (bracketPos != std::string::npos) {
                                line.insert(bracketPos + 1, "Timstamp:" + currentTimestamp + ", ");
                            }
                            size_t resultPos = line.find("FALSE");
                            if(resultPos != std::string::npos) {
                                isRes = false;
                            }
                            // chipLines.push_back(line);
                            chipRes.chipData.push_back(line);
                        }
                    }
                    
                    // 保存到map中
                    // chipData[chipId].insert(chipData[chipId].end(), chipLines.begin(), chipLines.end());
                    chipData[chipId].Res = (isRes ? "成功" : "失败");
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