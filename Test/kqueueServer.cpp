#include <sys/_types/_iovec_t.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

	const char *addr = "127.0.0.1";

    struct sockaddr_in address;
    address.sin_family = AF_INET;
	if (!inet_aton(addr, &address.sin_addr))
	{	
        std::cerr << "Failed to set address"<<addr<<std::endl;
		return 1;
	}
    address.sin_port = htons(8000);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        std::cerr << "Failed to bind to port 8080\n";
        return 1;
    }

    if (listen(server_fd, 5) == -1) {
        std::cerr << "Failed to listen on socket\n";
        return 1;
    }

    int kq_fd = kqueue();
    if (kq_fd == -1) {
        std::cerr << "Failed to create kqueue\n";
        return 1;
    }

    struct kevent kev;
    EV_SET(&kev, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) == -1) {
        std::cerr << "Failed to register server socket with kqueue\n";
        return 1;
    }

    std::cout << "Server started on port 8000\n";

    while (true) {
        struct kevent events[16];
        int nevents = kevent(kq_fd, NULL, 0, events, 16, NULL);
        if (nevents == -1) {
            std::cerr << "Failed to wait for events\n";
            return 1;
        }

        for (int i = 0; i < nevents; ++i) 
		{
            if (events[i].ident == server_fd) 
			{
                struct sockaddr_in client_address;
                socklen_t client_len = sizeof(client_address);
                int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
                if (client_fd == -1) {
                    std::cerr << "Failed to accept client connection\n";
                    continue;
                }

                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                EV_SET(&kev, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) == -1) {
                    std::cerr << "Failed to register client socket with kqueue\n";
                    return 1;
                }

                std::cout << "Client connected\n";
            }
            else 
			{
				int client = events[i].ident;
				struct sockaddr_in client_address;
				socklen_t client_len = sizeof(client_address);
				int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
				char buffer[1024];

				ssize_t n = recv(client_fd, buffer, sizeof(buffer), O_NONBLOCK);
				if (n == -1) 
				{
					std::cerr << "Failed to read from client socket\n";
					continue;
				}
				else if (n == 0) 
				{
					std::cout << "Client disconnected\n";
					close(client_fd);
					EV_SET(&kev, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) == -1) {
						std::cerr << "Failed to unregister client socket with kqueue\n";
						return 1;
					}
				}
				else 
				{
					std::cout << "Received " << n << " bytes from client\n";
					std::cout<<buffer<<std::endl;

					// Process incoming data according to the HTTP protocol
					// Generate a response based on the incoming request
					// Write the response to the client socket

					// Example response
					// content length안지키면 client측에서 block되는 문제있음. 주의
					std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 5100\r\n\r\naaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaaaaaaaaaaaaaaaaAaasaaaaaaaaaaaa";
					fcntl(client_fd, F_SETFL, O_NONBLOCK);
					write(client_fd, response.c_str(), response.length());
					close(client_fd);
				}
			}
		}
	}
	return 0;
}
