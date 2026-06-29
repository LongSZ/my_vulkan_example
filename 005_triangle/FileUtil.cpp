#include "FileUtil.h"
#include <assert.h>
#include <fstream>
#include <filesystem>
#include <stdexcept>

//加载Assets文件夹下的指定文本性质文件内容作为字符串返回
string FileUtil::loadAssetStr(string fname) 
{// 1. 判断文件是否存在
if (!std::filesystem::exists(fname) || !std::filesystem::is_regular_file(fname))
{
    return {};
}

// 2. 二进制模式打开，避免换行符转换干扰长度计算
std::ifstream in{ std::string{fname}, std::ios::in | std::ios::binary };
if (!in.is_open())
{
    return {};
}

// 3. 获取文件大小
const std::streamsize fileSize = std::filesystem::file_size(fname);
std::string content;
content.resize(static_cast<size_t>(fileSize));

// 4. 一次性整块读取（最高效）
in.read(content.data(), fileSize);

// 5. 校验实际读取字节数
if (!in && in.gcount() != fileSize)
{
    content.clear();
}

return content;
}

