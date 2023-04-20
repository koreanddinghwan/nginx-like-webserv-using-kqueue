NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -g3 -fsanitize=address
INC = .


LIBSRCS = $(addprefix ./lib/, strSplit.cpp)
LIBOBJS = $(LIBSRCS:.cpp=.o)

MAINSRCS = $(addprefix ./, main.cpp)
MAINOBJS = $(MAINSRCS:.cpp=.o)

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
