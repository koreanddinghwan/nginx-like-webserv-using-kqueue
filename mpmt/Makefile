NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -g3 -fsanitize=address
INC = .

HTTPSRCS = HttpServer.cpp HttpreqHandler.cpp HttpResHandler.cpp response.cpp
HTTPOBJS = $(addprefix ./modules/http/, $(HTTPSRCS:.cpp=.o))

HTTPDATASRCS = HttpConfigData.cpp HttpData.cpp HttpLocationData.cpp HttpServerData.cpp
HTTPDATAOBJS = $(addprefix ./modules/config/data/, $(HTTPDATASRCS:.cpp=.o))

HTTPCONFIGSRCS = HttpBlock.conf.cpp HttpLocationBlock.conf.cpp HttpServerBlock.conf.cpp
HTTPCONFIGOBJS = $(addprefix ./modules/config/http/, $(HTTPCONFIGSRCS:.cpp=.o))

CONFIGSRCS = Config.cpp EventBlock.conf.cpp GeneralBlock.conf.cpp BlockParser.cpp
CONFIGOBJS = $(addprefix ./modules/config/, $(CONFIGSRCS:.cpp=.o))

WEBSERVSRCS = WebServ.cpp WebServManager.cpp
WEBSERVOBJS = $(addprefix ./modules/webserv/, $(WEBSERVSRCS:.cpp=.o))

EVENTLOOPSRCS = EventLoop.cpp Event.cpp readCallback.cpp writeCallback.cpp registerEvent.cpp
EVENTLOOPOBJS = $(addprefix ./modules/eventLoop/, $(EVENTLOOPSRCS:.cpp=.o))

EXCEPTIONSRCS = configParseException.cpp httpException.cpp
EXCEPTIONOBJS = $(addprefix ./exceptions/, $(EXCEPTIONSRCS:.cpp=.o))

LIBSRCS = strSplit.cpp FileGuard.cpp ft_split.cpp
LIBOBJS = $(addprefix ./lib/, $(LIBSRCS:.cpp=.o))

MAINSRCS = main.cpp
MAINOBJS = $(addprefix ./, $(MAINSRCS:.cpp=.o))

OBJS = $(LIBOBJS) $(MAINOBJS) $(EXCEPTIONOBJS) $(EVENTLOOPOBJS) $(HTTPCONFIGOBJS) $(CONFIGOBJS) $(HTTPDATAOBJS) $(WEBSERVOBJS) $(HTTPOBJS)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) -I$(INC) $^ -o $@

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -I$(INC) -c $^ -o $@

clean :
	rm -rf $(OBJS)

fclean :
	make clean;
	rm -rf $(NAME);

re :
	make fclean;
	make all;

.PHONY : all clean fclean re
