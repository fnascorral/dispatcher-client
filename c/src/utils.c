/*+
 * Copyright 2015 iXsystems, Inc.
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define _WITH_GETLINE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define LINEMAX 1024

void *
xmalloc(size_t nbytes)
{
	void *ptr = malloc(nbytes);
	memset(ptr, 0, nbytes);
	return ptr;
}

ssize_t
xread(int fd, void *buf, size_t nbytes)
{
	ssize_t ret, done = 0;

	while (done < nbytes) {
		ret = read(fd, (void *)(buf + done), nbytes - done);
		if (ret == 0)
			return (-1);

		if (ret < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;

			return (-1);
		}

		done += ret;
	}

	return (done);
}

ssize_t
xwrite(int fd, void *buf, size_t nbytes)
{
	ssize_t ret, done = 0;

	while (done < nbytes) {
		ret = write(fd, (void *)(buf + done), nbytes - done);
		if (ret == 0)
			return (-1);

		if (ret < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;

			return (-1);
		}

		done += ret;
	}

	return (done);
}

ssize_t
xrecvmsg(int fd, struct msghdr *msg, int flags)
{

	return (recvmsg(fd, msg, flags | MSG_WAITALL));
}

ssize_t
xsendmsg(int fd, struct msghdr *msg, int flags)
{
	ssize_t ret, done = 0;
	ssize_t nbytes = (ssize_t)msg->msg_iov[0].iov_len;

	while (done < nbytes) {
		ret = sendmsg(fd, msg, flags);
		if (ret < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;

			return (-1);
		}

		done += ret;
		msg->msg_iov[0].iov_base += ret;
		msg->msg_iov[0].iov_len -= ret;
		msg->msg_control = NULL;
		msg->msg_controllen = 0;
	}

	return (done);
}

char *xfgetln(FILE *f)
{
	size_t nbytes = LINEMAX;
	char *buf = xmalloc(nbytes + 1);
	getline(&buf, &nbytes, f);
	return buf;
}
