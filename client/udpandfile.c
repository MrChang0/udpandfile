#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

const int SEVUDPPORT = 9632;
const int CLIUDPPORT = 9631;
const int SEVTCPPORT = 9633;
const char* REQUEST = "request";
const char* RESPONSE = "response";
const char* SENDSTART = "sendstart";
const char* ERRORREV = "rev_error";

int
read_timeout(int fd, unsigned int wait_seconds)
{
	int	ret = 0;
	if (wait_seconds > 0) {
		fd_set read_fdset;
		struct timeval timeout;

		FD_ZERO(&read_fdset);
		FD_SET(fd, &read_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;

		do {
			ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		if (ret == 0) {
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}
	return ret;
}

ssize_t
writen(int fd, const void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nwrite;
	char *bufp = (char *)buf;
	while (nleft > 0) {
		if ((nwrite = write(fd, bufp, nleft)) < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nwrite == 0) {
			continue;
		}
		bufp += nwrite;
		nleft -= nwrite;
	}
	return count - nleft;
}

ssize_t
readn(int fd, void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char *bufp = (char *)buf;
	while (nleft > 0) {
		int ret = read_timeout(fd, 3);
		if (ret == -1) {
			return ret;
		}
		if ((nread = read(fd, bufp, nleft)) < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nread == 0) {
			return count - nleft;
		}
		bufp += nread;
		nleft -= nread;
	}
	return count - nleft;
}

ssize_t
recv_peek(int sockfd, void *buf, size_t len)
{
	while (1) {
		int ret = recv(sockfd, buf, len, MSG_PEEK);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

ssize_t
readline(int sockfd, void *buf, size_t maxline)
{
	int ret;
	int nread;
	char *bufp = (char *)buf;
	int nleft = maxline;
	while (1) {
		ret = recv_peek(sockfd, bufp, nleft);
		if (ret < 0)
			return ret;
		else if (ret == 0)
			return ret;

		nread = ret;
		int i;
		for (i = 0; i < nread; i++) {
			if (bufp[i] == '\n') {
				ret = readn(sockfd, bufp, i + 1);
				if (ret != i + 1)
					exit(EXIT_FAILURE);

				return ret;
			}
		}
		if (nread > nleft) {
			exit(EXIT_FAILURE);
		}
		nleft -= nread;
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
			exit(EXIT_FAILURE);
		bufp += nread;
	}

	return -1;
}

int
file_back(const char *filename)
{
	int filenamelen = strlen(filename);
	char filename_back[filenamelen + 5];

	strcpy(filename_back, filename);
	strcpy(filename_back + filenamelen, "_bak");

	if (rename(filename, filename_back) == -1)
		return -1;
	return 0;
}

int
file_rev(int sock, char *recvbuf, FILE *fp, int filesize)
{
	int bytesread = 0;
	int bytesnread = filesize;
	int nextnread = 4 * 1024;
	int ret;

	while (bytesnread) {
		if (nextnread > bytesnread)
			nextnread = bytesnread;
		ret = readn(sock, recvbuf, nextnread);
		if (ret == -1 && errno == ETIMEDOUT) {
			fprintf(stderr, "rev timeout\n");
			return -1;
		}
		int count = fwrite(recvbuf, sizeof(char), nextnread, fp);
		if (count != nextnread)
			ERR_EXIT("fwrite");
		bytesnread -= nextnread;
		bytesread += nextnread;
	}

	return 0;
}

int main()
{
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
		ERR_EXIT("socket");

	struct sockaddr_in cliaddr;
	memset(&cliaddr, 0, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(CLIUDPPORT);
	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
		ERR_EXIT("bind");
	char recvbuf[4 * 1024] = { 0 };
	int n;
	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);

	while (1) {
		n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&peeraddr, &peerlen);
		if (n == -1) {
			if (errno == EINTR)
				continue;
			ERR_EXIT("recvfrom");
		}
		else if (n > 0) {
			if (strncmp(recvbuf, REQUEST, strlen(REQUEST)) == 0) {
				sendto(sock, RESPONSE, sizeof(RESPONSE), 0, (struct sockaddr *)&peeraddr, peerlen);
			}
			else if (strncmp(recvbuf, SENDSTART, strlen(REQUEST)) == 0) {
				break;
			}
		}
		memset(recvbuf, 0, sizeof(recvbuf));
	}
	memset(recvbuf, 0, sizeof(recvbuf));

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		ERR_EXIT("socket");
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SEVTCPPORT);
	servaddr.sin_addr = peeraddr.sin_addr;

	if (connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("connect");

	readline(sock, recvbuf, sizeof(recvbuf));

	char filename[20] = { 0 };
	int filesize;
	sscanf(recvbuf, "%d %s", &filesize, filename);
	printf("size = %d ,name = %s\n", filesize, filename);
	fflush(stdout);
	memset(recvbuf, 0, strlen(recvbuf));

	FILE *fp;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		if (errno != ENOENT)
			ERR_EXIT("fopen");
	}
	else {
		if (file_back(filename) == -1)
			ERR_EXIT("file_back");
	}

	fp = fopen(filename, "wb");
	if (fp == NULL)
		ERR_EXIT("fopen");

	if (file_rev(sock, recvbuf, fp, filesize) == -1) {
		int ret = writen(sock, ERRORREV, strlen(ERRORREV));
		if (ret == -1)
			ERR_EXIT("readn");
		ERR_EXIT("file_rev");
	}

	return 0;
}