/*
 * Copyright (c) 2017 Minqi Pan <pmq2001@gmail.com>
 *                    Shengyuan Liu <sounder.liu@gmail.com>
 *
 * This file is part of libsquash, distributed under the MIT License
 * For full terms see the included LICENSE file
 */

#include "squash.h"
#include <time.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
static SQUASH_OS_PATH squash_tmpdir()
{
	const int squash_win32_buf_sz = 32767;
	wchar_t squash_win32_buf[squash_win32_buf_sz + 1];
	DWORD length;

	length = GetEnvironmentVariable(L"TEMP", squash_win32_buf, squash_win32_buf_sz);
	if (length) {
		goto out;
	}
	length = GetEnvironmentVariable(L"TMP", squash_win32_buf, squash_win32_buf_sz);
	if (length) {
		goto out;
	}
	length = GetEnvironmentVariable(L"SystemRoot", squash_win32_buf, squash_win32_buf_sz);
	if (!length) {
		length = GetEnvironmentVariable(L"windir", squash_win32_buf, squash_win32_buf_sz);
	}
	if (length) {
		if (length + 5 >= squash_win32_buf_sz) {
			return NULL;
		}
		squash_win32_buf[length] = L'\\';
		squash_win32_buf[length + 1] = L't';
		squash_win32_buf[length + 2] = L'e';
		squash_win32_buf[length + 3] = L'm';
		squash_win32_buf[length + 4] = L'p';
		squash_win32_buf[length + 5] = 0;
		length += 5;
		goto out;
	}
	return NULL;
out:
	if (length >= 2 && L'\\' == squash_win32_buf[length - 1] && L':' != squash_win32_buf[length - 2]) {
		squash_win32_buf[length - 1] = 0;
		length -= 1;
	}
	return wcsdup(squash_win32_buf);
}
static SQUASH_OS_PATH squash_tmpf(SQUASH_OS_PATH tmpdir)
{
	const int squash_win32_buf_sz = 32767;
	wchar_t squash_win32_buf[squash_win32_buf_sz + 1];
	int ret, try = 0;
	srand(time(NULL));
	while (try < 3) {
		ret = swprintf(squash_win32_buf, squash_win32_buf_sz, L"%s\\nodec-runtime-%d", tmpdir, rand());
		if (-1 == ret) {
			return NULL;
		}
		if (!PathFileExistsW(squash_win32_buf)) {
			return wcsdup(squash_win32_buf);
		}
		++try;
	}
	return NULL;
}
#else // _WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static SQUASH_OS_PATH squash_tmpdir()
{
	char *try;
	size_t length;
	try = getenv("TMPDIR");
	if (try) {
		goto out;
	}
	try = getenv("TMP");
	if (try) {
		goto out;
	}
	try = getenv("TEMP");
	if (try) {
		goto out;
	}
	try = "/tmp";
out:
	try = strdup(try);
	length = strlen(try);
	if (length >= 2 && '/' == try[length - 1]) {
		try[length - 1] = 0;
	}
	return try;
}
static SQUASH_OS_PATH squash_tmpf(SQUASH_OS_PATH tmpdir)
{
	const int squash_buf_sz = 32767;
	char squash_buf[squash_buf_sz + 1];
	int ret, try = 0;
    struct stat statbuf;

	srand(time(NULL));
	while (try < 3) {
		ret = snprintf(squash_buf, squash_buf_sz, "%s/nodec-runtime-%d", tmpdir, rand());
		if (-1 == ret) {
			return NULL;
		}
		if (-1 == stat(squash_buf, &statbuf)) {
			return strdup(squash_buf);
		}
		++try;
	}
	return NULL;
}
#endif // _WIN32

struct SquashExtractEntry {
	sqfs *fs;
	SQUASH_OS_PATH path;
	SQUASH_OS_PATH ret;
	struct SquashExtractEntry *next;
};

static SquashExtractEntry* squash_extract_cache = NULL;

static const struct SquashExtractEntry* squash_extract_cache_find(sqfs *fs, SQUASH_OS_PATH path)
{
	struct SquashExtractEntry* ptr = squash_extract_cache;
	while (NULL != ptr) {
		if (fs == ptr->fs && 0 == strcmp(path, ptr->path)) {
			return ptr;
		}
		ptr = ptr->next;
	}
	return ptr;
}
static void squash_extract_cache_insert(sqfs *fs, SQUASH_OS_PATH path, SQUASH_OS_PATH ret)
{
	struct SquashExtractEntry* ptr = malloc(sizeof(struct SquashExtractEntry));
	if (NULL == ptr) {
		return;
	}
	ptr->fs = fs;
	ptr->path = strdup(path);
	if (NULL == ptr->path) {
		free(ptr);
		return;
	}
	ptr->ret = ret;
	ptr->next = squash_extract_cache;
	squash_extract_cache = ptr;
}

static SQUASH_OS_PATH squash_uncached_extract(sqfs *fs, SQUASH_OS_PATH path)
{
	SQUASH_OS_PATH tmpdir, tmpf, ret;

	tmpdir = squash_tmpdir();
	if (NULL == tmpdir) {
		return NULL;
	}
	tmpf = squash_tmpf(tmpdir);
	if (NULL == tmpf) {
		free(tmpdir);
		return NULL;
	}
	// TODO
out:
	free(tmpdir);
	return ret;
}

SQUASH_OS_PATH squash_extract(sqfs *fs, SQUASH_OS_PATH path)
{
	SQUASH_OS_PATH ret;
	static SquashExtractEntry* found;

	found = squash_extract_cache_find(fs, path);
	if (NULL != found) {
		return found->ret;
	}
	ret = squash_uncached_extract(fs, path);
	if (NULL != ret) {
		squash_extract_cache_insert(fs, path, ret);
	}
	return ret;
}
