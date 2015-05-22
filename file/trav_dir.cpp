#include <stdio.h>
#include <string.h>
#include "trav_dir.h"

/* 打开目录并获取第一项 */
int trav_dir_begin(const char *dir, struct traverse_dir_context *ctx)
{
	const char* p;
	char buf[MAX_PATH];

	if (strlen(dir) > sizeof(ctx->basedir)-2)
		return 0;

	memset(ctx, 0, sizeof(*ctx));
	strcpy(ctx->basedir, dir);

	p = dir + strlen(dir) - 1;
	if (*p != PATH_SEP_CHAR)
		ctx->basedir[p-dir+1] = PATH_SEP_CHAR;

#ifdef _WIN32
	_snprintf(buf, MAX_PATH, "%s*.*", ctx->basedir);
	ctx->hFind = FindFirstFileA(buf, &ctx->wfd);
	if (ctx->hFind == INVALID_HANDLE_VALUE)
		return 0;
#elif defined __linux__
	ctx->dir = opendir(dir);
	if (ctx->dir == NULL)
		return 0;
	ctx->pentry = readdir(ctx->dir);
	if (ctx->pentry == NULL)
	{
		closedir(ctx->dir);
		return 0;
	}
#endif
	return 1;
}

/* 遍历目录中的下一项 */
int trav_dir_next(struct traverse_dir_context *ctx)
{
#ifdef _WIN32
	if (!FindNextFileA(ctx->hFind, &ctx->wfd))
		return 0;
#elif defined __linux__
	if ((ctx->pentry = readdir(ctx->dir)) == NULL)
		return 0;
#endif
	return 1;
}

/* 该项是否是.(当前目录) */
int trav_entry_is_dot(struct traverse_dir_context *ctx)
{
#ifdef _WIN32
	return !strcmp(ctx->wfd.cFileName, ".");
#elif defined __linux__
	return !strcmp(ctx->pentry->d_name, ".");
#endif
}

/* 该项是否是..(上级目录) */
int trav_entry_is_dotdot(struct traverse_dir_context *ctx)
{
#ifdef _WIN32
	return !strcmp(ctx->wfd.cFileName, "..");
#elif defined __linux__
	return !strcmp(ctx->pentry->d_name, "..");
#endif
}

/* 该项是否是目录 */
int trav_entry_is_dir(struct traverse_dir_context *ctx)
{
#ifdef _WIN32
	return ctx->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#elif defined __linux__
	return ctx->pentry->d_type == DT_DIR;
#endif
}

/* 普通文件的定义是：在Widnows系统上不是隐藏文件、系统文件等，
 * 在Linux系统上不是块设备、字符设备、FIFO等 */
int trav_entry_is_regular(struct traverse_dir_context *ctx)
{
#ifdef _WIN32
	if (ctx->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
	  || ctx->wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM
	  || ctx->wfd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE
	  || ctx->wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		return 0;
	return 1;
#elif defined __linux__
	return ctx->pentry->d_type == DT_REG;
#endif
}

/* 结束遍历 */
int trav_dir_end(struct traverse_dir_context *ctx)
{
#ifdef _WIN32
	FindClose(ctx->hFind);
#elif defined __linux__
	closedir(ctx->dir);
#endif
	return 1;
}

/* 获取此项路径名 */
const char* trav_entry_name(struct traverse_dir_context *ctx)
{
#ifdef _WIN32
	return ctx->wfd.cFileName;
#elif defined __linux__
	return ctx->pentry->d_name;
#endif
}

/* 获取此项路径名 */
int trav_entry_path(struct traverse_dir_context *ctx, char *buf, int len)
{
	int tlen;
#ifdef _WIN32
	tlen = strlen(ctx->basedir) + strlen(ctx->wfd.cFileName);
	if (tlen + 1 > len)
		return 0;
	memset(buf, 0, len);
	_snprintf(buf, len, "%s%s", ctx->basedir, ctx->wfd.cFileName);

#elif defined __linux__
	tlen = strlen(ctx->basedir) + strlen(ctx->pentry->d_name);
	if (tlen + 1 > len)
		return 0;
	memset(buf, 0, len);
	snprintf(buf, len, "%s%s", ctx->basedir, ctx->pentry->d_name);
#endif
	return 1;
}

///* implementation of strcmp that ingnores cases */
int ic_strcmp(const char *s1, const char *s2)
{
	int index;
	for (index = 0; s1[index] && s2[index]; ++index) {
		/* If characters are same or inverting the 6th bit makes them same */
		if (s1[index] == s2[index] || (s1[index] ^ 32) == s2[index]) {
			continue;
		} else {
			break;
		}
	}
	/* Compare the last (or first mismatching in case of not same) characters */
	if (s1[index] == s2[index]) {
		return 0;
	}
	if ((s1[index]|32) < (s2[index]|32)) { //Set the 6th bit in both, then compare
		return -1;
	}
	return 1;
}

int stringCompareIgnoreCase(const std::string& lhs, const std::string& rhs)
{
	return ic_strcmp(lhs.c_str(),rhs.c_str());
}

std::vector<std::string> RecurTraverDir(const char *dir, const char *ext, const char *delim)
{
	std::vector<std::string> retvec;
	if (NULL == dir) {
		return std::move(retvec);
	}
	std::vector<std::string> splitvec;
	char buf[MAX_PATH+1];
	struct traverse_dir_context ctx;
	if (NULL != ext) {//有指定类型
		std::string str_ext=ext;
		const char* p=NULL;
		if (NULL == delim) {
			splitvec.push_back(str_ext);//无分割符，全部都是
		} else {
			for (p = strtok((char*)str_ext.c_str(),delim );  p;  p = strtok( NULL,  delim )) {
				splitvec.push_back(p);//分割得到所有后缀
			}
		}
		auto cit=splitvec.cbegin(), citend=splitvec.cend();
		if (trav_dir_begin(dir, &ctx)) {
			do {
				if (trav_entry_is_dot(&ctx) || trav_entry_is_dotdot(&ctx)) {	//忽略"."和".."
					continue;
				} else if (trav_entry_is_dir(&ctx)) {//目录，进行递归
					trav_entry_path(&ctx, buf, MAX_PATH);
					auto dirvec=RecurTraverDir(buf,ext,delim);
					retvec.insert(retvec.end(),dirvec.begin(),dirvec.end());
				} else {//文件
					trav_entry_path(&ctx, buf, MAX_PATH);
					std::string filename = buf;
					int index=filename.find_last_of(".");
					std::string fileext=filename.substr(index+1);//后缀名
					for (cit=splitvec.cbegin(); cit != citend; ++cit) {
						stringCompareIgnoreCase(fileext,*cit) == 0 ? retvec.push_back(filename) : 0;
					}
				}
			} while(trav_dir_next(&ctx));
		}
	} else {//所有文件
		if (trav_dir_begin(dir, &ctx)) {
			do {
				if (trav_entry_is_dot(&ctx) || trav_entry_is_dotdot(&ctx)) {	//忽略"."和".."
					continue;
				} else if (trav_entry_is_dir(&ctx)) {//目录，进行递归
					trav_entry_path(&ctx, buf, MAX_PATH);
					auto dirvec=RecurTraverDir(buf,ext,delim);
					retvec.insert(retvec.end(),dirvec.begin(),dirvec.end());
				} else {//文件
					trav_entry_path(&ctx, buf, MAX_PATH);
					retvec.push_back(buf);
				}
			} while(trav_dir_next(&ctx));
		}
	}
	trav_dir_end(&ctx);
	return std::move(retvec);
}
