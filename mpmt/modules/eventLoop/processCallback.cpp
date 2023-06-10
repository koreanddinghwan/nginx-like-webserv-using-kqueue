#include "EventLoop.hpp"
#include <sys/wait.h>

void EventLoop::e_processCallback(struct kevent *e)
{
	std::cerr<<"e_processCallback"<<std::endl;
	Event *e_udata = static_cast<Event *>(e->udata);

	if (e->fflags & NOTE_EXIT)
	{
        int exit_status;
		int status;
        // 자식 프로세스의 종료를 기다림
        waitpid(e_udata->childPid, &status, WNOHANG);

        if (WIFEXITED(status)) {
            // 자식 프로세스가 정상적으로 종료한 경우
			// 현재 STDIN, STDOUT에는 원래의 값이 아닌 다른 값이 들어가 있으므로 원래의 값으로 복구
            printf("Child process exited with status: %d\n", exit_status);
			printf("%s\n", static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().path.c_str());
			printf("%d, %d, %d, %d", e_udata->oldStdOut, e_udata->oldStdin, STDIN_FILENO, STDOUT_FILENO);
			printf("%d, %d %d %d", e_udata->tmpInFile, e_udata->tmpOutFile, e_udata->file_fd, e_udata->getClientFd());
			/* close(e_udata->tmpInFile + 1); */
			/* close(STDIN_FILENO); */
			/* dup2(e_udata->oldStdin, STDIN_FILENO); */
			
			/* close(STDOUT_FILENO); */
			/* dup2(e_udata->oldStdin, STDOUT_FILENO); */
			/* close(e_udata->oldStdin); */

			/* close(e_udata->oldStdOut); */
			/* dup2(e_udata->oldStdOut, STDOUT_FILENO); */
			exit_status = WEXITSTATUS(status);
			std::cout<<static_cast<HttpreqHandler *>(e_udata->getRequestHandler())->getRequestInfo().path<<std::endl;
			registerTmpFileReadEvent(e_udata);
			/* close(e_udata->oldStdin); */
        }
	}
}
