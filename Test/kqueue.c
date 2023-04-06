#include <sys/event.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>

int main(int ac, char **av)
{
	struct kevent event; //monitor
	struct kevent tevent; //triggered
	int kq, fd, ret;

	if (ac != 2)
		err(EXIT_FAILURE, "Usage: %s path\n", av[0]);
	fd = open(av[1], O_RDONLY);
	if (fd == -1)
		err(EXIT_FAILURE, "Failed to open %s", av[1]);

	kq = kqueue();
	if (kq == -1)
		err(EXIT_FAILURE, "kqueue() failed");

	//kevent구조체 초기화
	EV_SET(&event, fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_WRITE, 0, NULL);

	//kqueue에 event부착
	ret = kevent(kq, &event,1, 0, 0, 0);
	if (ret == -1)
		err(EXIT_FAILURE, "kevent register");
	if (event.flags & EV_ERROR)
		errx(EXIT_FAILURE, "EVENT ERROR: %s", strerror(event.data));

	for (;;)
	{
		ret = kevent(kq, 0, 0, &tevent, 1, 0);
		if (ret == -1)
			err(EXIT_FAILURE, "kevent wait");
		else if (ret > 0)
			printf("Something was written in %s", av[1]);
	}
}
