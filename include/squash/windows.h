/*
 * Copyright (c) 2016-2017 Minqi Pan and Shengyuan Liu
 *
 * This file is part of libsquash, distributed under the MIT License
 * For full terms see the included LICENSE file
 */

#ifndef WINDOWS_H_A80B5674
#define WINDOWS_H_A80B5674

#include <Windows.h>
#include <stdint.h>

typedef unsigned short sqfs_mode_t;
typedef uint32_t sqfs_id_t; /* Internal uids/gids are 32-bits */

typedef SSIZE_T ssize_t;
typedef DWORD64 sqfs_off_t;
typedef uint32_t uid_t;		/* [???] user IDs */

#define	S_IFIFO		0010000		/* [XSI] named pipe (fifo) */
#define	S_IFBLK		0060000		/* [XSI] block special */
#define	S_IFLNK		0120000		/* [XSI] symbolic link */
#define	S_IFSOCK	0140000		/* [XSI] socket */

#define	S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)	/* block special */
#define	S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)	/* char special */
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)	/* directory */
#define	S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)	/* fifo or socket */
#define	S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)	/* regular file */
#define	S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)	/* symbolic link */
#define	S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)	/* socket */

#define	makedev(x,y)	((dev_t)(((x) << 24) | (y)))

/* structure describing an open directory. */
typedef struct {
	int	__dd_fd;	/* file descriptor associated with directory */
	long	__dd_loc;	/* offset in current buffer */
	long	__dd_size;	/* amount of data returned */
	char	*__dd_buf;	/* data buffer */
	int	__dd_len;	/* size of data buffer */
	long	__dd_seek;	/* magic cookie returned */
	long	__dd_rewind;	/* magic cookie for rewinding */
	int	__dd_flags;	/* flags for readdir */
	struct _telldir *__dd_td; /* telldir position recording */
} DIR;

#endif /* end of include guard: WINDOWS_H_A80B5674 */
