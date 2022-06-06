NAME						= webserv
CXX							= g++
RM							= rm -rf
# CXXFLAGS					= -Wall -Werror -Wextra -std=c++98 -g3 -fsanitize=address
# CXXFLAGS					= -std=c++98 -g3 -fsanitize=address
CXXFLAGS					= -Wall -Werror -Wextra -std=c++98
# CXXFLAGS					= -std=c++98

INCLUDE						= include


SRCDIR						= src
SRC							= $(wildcard $(SRCDIR)/*.cpp)

OBJDIR						= objects
OBJ							= $(SRC:%.cpp=$(OBJDIR)/%.o)

OBJ							= $(subst $(SRCDIR), $(OBJDIR), $(SRC:.cpp=.o))


# ${OBJ}:
# 	echo ${OBJDIR}/%.o




${OBJDIR}/%.o: $(SRCDIR)/%.cpp
			@mkdir -p ${@D}
			${CXX} ${CXXFLAGS} -c $< -I$(INCLUDE) -o $@

$(NAME): $(OBJ)
			${CXX} ${CXXFLAGS} ${OBJ} -o ${NAME}

all: $(NAME)


run:		${NAME}
			@echo "\033[0;34mRunning ${NAME}...\033[0;0m"
			@./${NAME}

clean:
			@${RM} ${OBJDIR}
			@printf "\033[0;35mPROJECT\033[0;0m:      \033[0;32mCleaned\033[0;0m\n"

fclean:		clean
			@${RM} ${NAME}

re:			fclean all

silent:
			@:

.PHONY:		all clean fclean re run

