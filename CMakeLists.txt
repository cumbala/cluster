cmake_minimum_required(VERSION 3.23)
project(cluster C)

set(CMAKE_C_STANDARD 99)

add_compile_options(-Wall -Wextra -Werror -DNDEBUG -g)

add_executable(cluster cluster.c)
target_link_libraries(cluster m)