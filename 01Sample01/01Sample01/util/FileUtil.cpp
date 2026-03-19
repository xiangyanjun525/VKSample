#include "FileUtil.h"
#include<fstream>
#include<string>
#include<iostream>
#include <assert.h>

//加载Assets文件夹下的指定文本性质文件内容作为字符串返回
std::string FileUtil::loadAssetStr(const std::string& fname)
{
	std::ifstream infile;
	infile.open(fname.data());   //将文件流对象与文件连接起来 
	std::cout << "fname>" << fname << std::endl;
	assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 
	std::string ss;
	std::string s;
	while (getline(infile, s))
	{
		ss += s;
		ss += '\n';
	}
	infile.close();             //关闭文件输入流 
	/*cout << ss << endl;*/
	return ss;
}