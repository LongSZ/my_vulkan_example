#ifndef __FileUtil_H__
#define __FileUtil_H__

#include <string>

using namespace std;//指定使用的命名空间
class FileUtil {
public:
    static string loadAssetStr(string fname);//加载Assets文件夹下的指定文本性质文件内容作为字符串返回
};

#endif