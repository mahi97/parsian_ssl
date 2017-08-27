cmake_minimum_required(VERSION 2.8.3)
project(parsian_util)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
        geometry_msgs
        parsian_msgs
        roscpp
        rospy
        std_msgs
        )

find_package(Protobuf REQUIRED)

set(CMAKE_CXX_FLAGS "\
--std=c++14 ${CMAKE_CXX_FLAGS} \
-Wall \
-Wno-unused-function \
-Wsign-compare \
-g \
"
        )

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a run_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   geometry_msgs#   parsian_msgs#   std_msgs
# )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
#catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES parsian_util
#  CATKIN_DEPENDS geometry_msgs parsian_msgs roscpp rospy std_msgs
#  DEPENDS system_lib
#)

###########
## Build ##
###########


## Declare a C++ library
# add_library(${PROJECT_NAME}
#   src/${PROJECT_NAME}/parsian_util.cpp
# )

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
# add_executable(${PROJECT_NAME}_node src/parsian_util_node.cpp)

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node
#   ${catkin_LIBRARIES}
# )



#####################################
## Protobuf conversion & compiling ##
#####################################

set(proto_dir ${PROJECT_SOURCE_DIR}/proto)
set(proto_files
        ${proto_dir}/grSim_Replacement.proto
        ${proto_dir}/grSim_Commands.proto
        ${proto_dir}/grSim_Packet.proto
        ${proto_dir}/messages_robocup_ssl_detection.proto
        ${proto_dir}/messages_robocup_ssl_geometry.proto
        ${proto_dir}/messages_robocup_ssl_refbox_log.proto
        ${proto_dir}/messages_robocup_ssl_wrapper.proto
        ${proto_dir}/multi_team_communication.proto
        ${proto_dir}/radio_protocol_command.proto
        ${proto_dir}/radio_protocol_wrapper.proto
        ${proto_dir}/referee.proto
        ${proto_dir}/robot_status.proto
        )

# Set up destination directories
catkin_destinations()
set(proto_gen_cpp_dir ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_INCLUDE_DESTINATION})
set(proto_gen_python_dir ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_PYTHON_DESTINATION})
file(MAKE_DIRECTORY ${proto_gen_cpp_dir})
file(MAKE_DIRECTORY ${proto_gen_python_dir})
set(protogen_include_dirs ${proto_gen_cpp_dir}/../ ${proto_gen_python_dir})

# catkin_python_setup()

catkin_package(
        INCLUDE_DIRS
        include
        ${protogen_include_dirs}
        LIBRARIES
        parsian_util
        #    ${PROJECT_NAME}_proto
)



include_directories(
        ${catkin_INCLUDE_DIRS}
        include
        ${PROTOBUF_INCLUDE_DIR}
        ${PROTOBUF_INCLUDE_DIR}/../../
)

# Create lists of files to be generated.
set(proto_gen_cpp_files "")
set(proto_gen_python_files "")
foreach(proto_file ${proto_files})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    list(APPEND proto_gen_cpp_files ${proto_gen_cpp_dir}/${proto_name}.pb.h ${proto_gen_cpp_dir}/${proto_name}.pb.cc)
    list(APPEND proto_gen_python_files ${proto_gen_python_dir}/${proto_name}_pb2.py)
endforeach(proto_file ${proto_files})

# Run protoc and generate language-specific headers.
add_custom_command(
        OUTPUT ${proto_gen_cpp_files} ${proto_gen_python_files}
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --proto_path=${proto_dir} --cpp_out=${proto_gen_cpp_dir} --python_out=${proto_gen_python_dir} ${proto_files}

        DEPENDS ${PROTOBUF_PROTOC_EXECUTABLE} ${proto_files}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set_source_files_properties(${proto_gen_cpp_files} PROPERTIES GENERATED TRUE)
set_source_files_properties(${proto_gen_python_files} PROPERTIES GENERATED TRUE)

# Make the python message folder a python package.
file(APPEND ${proto_gen_python_dir}/__init__.py "")

add_custom_target(${PROJECT_NAME}_generate_headers
        DEPENDS ${proto_gen_cpp_files} ${proto_gen_python_files}
        )

# Create proto library for linking.
#add_library(${PROJECT_NAME}_proto ${proto_gen_cpp_files})
#target_link_libraries(${PROJECT_NAME}_proto ${PROTOBUF_LIBRARY})
#add_dependencies(${PROJECT_NAME}_proto ${PROJECT_NAME}_generate_headers)
add_library(${PROJECT_NAME} ${proto_gen_cpp_files})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARY} ${PROTOBUF_LIBRARIES})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_generate_headers)


install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        )

install(DIRECTORY ${proto_gen_cpp_dir}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
        FILES_MATCHING PATTERN "*.h"
        )

install(DIRECTORY ${proto_gen_python_dir}/
        DESTINATION ${CATKIN_PACKAGE_PYTHON_DESTINATION}
        FILES_MATCHING PATTERN "*.py"
        )

#add_library(parsian_util
#)
#add_dependencies(parsian_util
#    ${${PROJECT_NAME}_EXPORTED_TARGETS}
#    ${catkin_EXPORTED_TARGETS}
#)

#target_link_libraries(parsian_util
#    ${catkin_LIBRARIES}
#    ${PROJECT_NAME}_proto
#)


#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

# add_library(parsian_util_proto ${PROTO_CPP} ${PROTO_H} ${PROTO_PY})
# add_dependencies(parsian_util_proto ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
# target_link_libraries(parsian_util_proto ${catkin_LIBRARIES} ${PROTOBUF_LIBRARIES} ${PROTOBUF_LIBRARY})
#

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_parsian_util.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
