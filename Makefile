NAME = ft_lex
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror

SRC = src/main.cpp \
      src/tokenizer/Tokenizer.cpp \
      src/utils/Logger.cpp \
      src/utils/FileUtils.cpp \
      src/utils/StringUtils.cpp

OBJ = $(SRC:src/%.cpp=obj/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

obj/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Iinc -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re