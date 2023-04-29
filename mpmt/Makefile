NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -g3 -fsanitize=address
INC = .


LIBSRCS = strSplit.cpp FileGuard.cpp
LIBOBJS = $(addprefix ./lib/, $(LIBSRCS:.cpp=.o))

MAINSRCS = main.cpp
MAINOBJS = $(addprefix ./, $(MAINSRCS:.cpp=.o))

OBJS = $(LIBOBJS) $(MAINOBJS)

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
