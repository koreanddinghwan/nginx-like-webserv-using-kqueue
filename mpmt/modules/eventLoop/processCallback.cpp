#include "EventLoop.hpp"
#include <sys/wait.h>

void EventLoop::e_processCallback(struct kevent *e)
{
	std::cerr<<"e_processCallback"<<std::endl;
	Event *e_udata = static_cast<Event *>(e->udata);

	if (e->fflags & NOTE_EXIT)
	{
		int status;
        // 자식 프로세스의 종료를 기다림
        waitpid(e_udata->childPid, &status, WNOHANG);

        if (WIFEXITED(status)) {
            // 자식 프로세스가 정상적으로 종료한 경우
			dup2(e_udata->oldStdin, STDIN_FILENO);
			close(STDIN_FILENO);

			/* dup2(e_udata->oldStdOut, STDOUT_FILENO); */
			/* close(e_udata->oldStdOut); */
            int exit_status = WEXITSTATUS(status);
			std::cout<<static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().path<<std::endl;
            printf("Child process exited with status: %d\n", exit_status);
			std::cerr<<"cgi process exit"<<std::endl;
			registerTmpFileReadEvent(e_udata);
        }
	}
}
