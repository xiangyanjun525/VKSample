#pragma once

#include <string>

class FileUtil
{
public:
	static std::string loadAssetStr(const std::string& fname);//加载Assets文件夹下的指定文本性质文件内容作为字符串返回
};