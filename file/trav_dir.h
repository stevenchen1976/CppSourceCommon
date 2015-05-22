/*****************************************
* @file     trav_dir.h
* @brief    出处：http://www.leewei.org/?p=2400
* @details  想找跨平台遍历目录的方法，所以只能使用条件编译把各平台代码分开
            添加了递归遍历指定类型文件的例子
* @author   phata,wqvbjhc@gmail.com
* @date     2013-7-24
* @mod      2014-07-27 phata 添加RecurTraverDir接口
******************************************/
#ifndef TRAV_DIR_H
#define TRAV_DIR_H
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#define	PATH_SEP_CHAR	'\\'
#elif defined __linux__
#include <dirent.h>
#define MAX_PATH 1024
#define	PATH_SEP_CHAR	'/'
#endif

struct traverse_dir_context {
    char			basedir[MAX_PATH+1];
#ifdef _WIN32
    WIN32_FIND_DATAA	wfd;
    HANDLE			hFind;
#elif defined __linux__
    DIR*			dir;
    struct dirent*		pentry;
#else
#error "Platform unsupported."
#endif
};

int	trav_dir_begin(const char* dir, struct traverse_dir_context *ctx);	  /* 开始遍历，dir自动判断\结尾,支持相对路径 */
int	trav_dir_next(struct traverse_dir_context *ctx);			/* 遍历一项 */
int	trav_dir_end(struct traverse_dir_context *ctx);				/* 结束遍历 */

const char* trav_entry_name(struct traverse_dir_context *ctx);			/* 文件/目录名(无PATH_SEP_CHAR结尾) */
int	trav_entry_path(struct traverse_dir_context *ctx, char* buf, int len);	/* 完整路径名(无PATH_SEP_CHAR结尾) */

int	trav_entry_is_dot(struct traverse_dir_context *ctx);			/* 该项是.(当前目录) */
int	trav_entry_is_dotdot(struct traverse_dir_context *ctx);			/* 该项是..(上级目录) */
int	trav_entry_is_dir(struct traverse_dir_context *ctx);			/* 该项类型是目录 */
int	trav_entry_is_regular(struct traverse_dir_context *ctx);		/* 该项类型是普通文件 */

/*自己加的接口--递归查找目录下的指定文件
  dir    --要查找到目录，不一定需要\/结尾，程序自动判断添加
  ext    --要查找到文件后缀，可多个,如"jpg;png;bmp"(其delim为;), "jpg"(其中delim为NULL)
  delim  --ext中的分割符，为空表明ext无分割符
  return --返回包含完整路径的文件名列表
*/
std::vector<std::string> RecurTraverDir(const char *dir, const char *ext, const char *delim);
#endif//TRAV_DIR_H

////////////调用例子
////#include <stdio.h>
//#include <stdio.h>
//#include <vector>
//#include <string>
//#include<algorithm>
//#include "trav_dir.h"
//
//void TraverDir(char *dir)
//{
//	char buf[MAX_PATH+1];
//	struct traverse_dir_context ctx;
//	printf ("Contents of %s:\n", dir);
//	if (trav_dir_begin(dir, &ctx)) {
//		do {
//			if (trav_entry_is_dot(&ctx) || trav_entry_is_dotdot(&ctx)) {	//忽略"."和".."
//				continue;
//			} else if (trav_entry_is_dir(&ctx)) {			//目录
//				trav_entry_path(&ctx, buf, MAX_PATH);
//				printf("Directory entrypath：%s\n", buf);//完整路径
//				//const char *p = trav_entry_name(&ctx);
//				//printf("Directory entryname：%s\n", p);//名称
//
//				//进入目录
//				TraverDir(buf);
//			} else if (trav_entry_is_regular(&ctx)) {			//普通文件
//				trav_entry_path(&ctx, buf, MAX_PATH);
//				printf("Regular entrypath：%s\n", buf);//完整路径
//				const char *p = trav_entry_name(&ctx);
//				printf("Regular entryname：%s\n", p);//名称
//			} else {							//特殊文件
//				const char *p = trav_entry_name(&ctx);
//				printf("Special：%s\n", p);
//			}
//		} while(trav_dir_next(&ctx));
//	}
//	trav_dir_end(&ctx);
//	printf("\n");
//}
//
///* implementation of strcmp that ingnores cases */
//int ic_strcmp(const char *s1, const char *s2)
//{
//	int index;
//	for (index = 0; s1[index] && s2[index]; ++index) {
//		/* If characters are same or inverting the 6th bit makes them same */
//		if (s1[index] == s2[index] || (s1[index] ^ 32) == s2[index]) {
//			continue;
//		} else {
//			break;
//		}
//	}
//	/* Compare the last (or first mismatching in case of not same) characters */
//	if (s1[index] == s2[index]) {
//		return 0;
//	}
//	if ((s1[index]|32) < (s2[index]|32)) { //Set the 6th bit in both, then compare
//		return -1;
//	}
//	return 1;
//}
//
//int stringCompareIgnoreCase(const std::string& lhs, const std::string& rhs)
//{
//	return ic_strcmp(lhs.c_str(),rhs.c_str());
//}
//
////递归遍历dir目录下所有符合ext的文件
////dir--在遍历的根目录
////ext--在查找的文件后缀，若为多种，分割符包含于delim。如"jpg","doc;xls";为NULL表明返回所有文件
////delim--ext中的分割符，可以多个.为NULL表明ext只所有内容都为一个后缀名
////返回值：包含完整路径的文件名列表
//std::vector<std::string> RecurTraverDir(const char *dir, const char *ext, const char *delim)
//{
//	std::vector<std::string> retvec;
//	if (NULL == dir) {
//		return std::move(retvec);
//	}
//	std::vector<std::string> splitvec;
//	char buf[MAX_PATH+1];
//	struct traverse_dir_context ctx;
//	if (NULL != ext) {//有指定类型
//		std::string str_ext=ext;
//		const char* p=NULL;
//		if (NULL == delim) {
//			splitvec.push_back(str_ext);//无分割符，全部都是
//		} else {
//			for (p = strtok((char*)str_ext.c_str(),delim );  p;  p = strtok( NULL,  delim )) {
//				splitvec.push_back(p);//分割得到所有后缀
//			}
//		}
//		auto cit=splitvec.cbegin(), citend=splitvec.cend();
//		if (trav_dir_begin(dir, &ctx)) {
//			do {
//				if (trav_entry_is_dot(&ctx) || trav_entry_is_dotdot(&ctx)) {	//忽略"."和".."
//					continue;
//				} else if (trav_entry_is_dir(&ctx)) {//目录，进行递归
//					trav_entry_path(&ctx, buf, MAX_PATH);
//					auto dirvec=RecurTraverDir(buf,ext,delim);
//					retvec.insert(retvec.end(),dirvec.begin(),dirvec.end());
//				} else {//文件
//					trav_entry_path(&ctx, buf, MAX_PATH);
//					std::string filename = buf;
//					int index=filename.find_last_of(".");
//					std::string fileext=filename.substr(index+1);//后缀名
//					for (cit=splitvec.cbegin(); cit != citend; ++cit) {
//						stringCompareIgnoreCase(fileext,*cit) == 0 ? retvec.push_back(filename) : 0;
//					}
//				}
//			} while(trav_dir_next(&ctx));
//		}
//	} else {//所有文件
//		if (trav_dir_begin(dir, &ctx)) {
//			do {
//				if (trav_entry_is_dot(&ctx) || trav_entry_is_dotdot(&ctx)) {	//忽略"."和".."
//					continue;
//				} else if (trav_entry_is_dir(&ctx)) {//目录，进行递归
//					trav_entry_path(&ctx, buf, MAX_PATH);
//					auto dirvec=RecurTraverDir(buf,ext,delim);
//					retvec.insert(retvec.end(),dirvec.begin(),dirvec.end());
//				} else {//文件
//					trav_entry_path(&ctx, buf, MAX_PATH);
//					retvec.push_back(buf);
//				}
//			} while(trav_dir_next(&ctx));
//		}
//	}
//	trav_dir_end(&ctx);
//	return std::move(retvec);
//}
//int   main(int   argc,   char*   argv[])
//{
//	char *dir;
//#ifdef __linux__
//	dir = "/tmp";
//#else
//	dir = "D:\\My Documents\\My Pictures";
//#endif
//	//TraverDir(dir);
//
//	std::string ext="jpg";
//	std::string delim=";";
//	auto retvec=RecurTraverDir(dir,ext.c_str(),NULL);
//	for (auto it=retvec.cbegin(); it!=retvec.cend(); ++it) {
//		printf("%s\n",it->c_str());
//	}
//	printf("found %d item\n",retvec.size());
//	system("pause");
//	return   0;
//}
