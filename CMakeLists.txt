project(gmetricl)
cmake_minimum_required(VERSION 2.8.12)
find_package(Threads REQUIRED)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

#gcc -D_LARGEFILE64_SOURCE -D_REENTRANT -B static -D_GNU_SOURCE -DPATH_MAX=4096 \
#    -fno-strict-aliasing -g -std=c99 \
#    -o gmetricl csv.c ganglia.c gmetricl.c net.c \
#    -Wl,--export-dynamic \
#    -I/ncar/ssg/apr/current/include -I/ncar/ssg/apr/current/include/apr-1/ -L/ncar/ssg/apr/current/lib \
#    -I/ncar/ssg/ganglia/current-client/include -L/ncar/ssg/ganglia/current-client/lib64  \
#    -L/ncar/ssg/pcre/current/lib/ -L/ncar/ssg/expat/current/lib/ -L/ncar/ssg/libconfuse/current/lib \
#    -L/ncar/ssg/glib/current/lib -L/ncar/ssg/apr/current/lib -L/ncar/ssg/apr/current//lib64 \
#    -L/ncar/ssg/libconfuse/current//lib64 \
#    -Wl,-Bstatic -lganglia -lconfuse -lexpat -lpcre -lapr-1 \
#    -Wl,-Bdynamic -lresolv -ldl -lnsl -lrt -lcrypt -lpthread -pthread 


set(APR_PATH "/usr/include/apr-1.0" CACHE STRING "location of libapr-1 include files")
include_directories("${APR_PATH}")

if(CMAKE_C_COMPILER_ID MATCHES GNU)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pthread -fno-strict-aliasing -B static")
endif()

find_library(LIBGANGLIA ganglia)
find_library(LIBCONFUSE confuse)
find_library(LIBEXPAT expat)
find_library(LIBPCRE pcre)
find_library(LIBAPR1 apr-1)
find_library(LIBNSL nsl)
find_library(LIBRT rt)
find_library(LIBCRYPT crypt)
find_library(LIBPTHREAD pthread)

add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D_REENTRANT)
add_definitions(-D_GNU_SOURCE)
add_definitions(-DPATH_MAX=4096)
   
add_executable(gmetricl csv.c ganglia.c gmetricl.c net.c)
set_target_properties(gmetricl PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(gmetricl PROPERTIES LINK_SEARCH_END_STATIC 1)
target_link_libraries(gmetricl dl ${LIBGANGLIA} ${LIBCONFUSE} ${LIBEXPAT} ${LIBPCRE} ${LIBAPR1} ${LIBRT} ${LIBCYRPT} ${LIBPTHREAD} "${CMAKE_THREAD_LIBS_INIT}")

install(TARGETS gmetricl 
  RUNTIME DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE)
