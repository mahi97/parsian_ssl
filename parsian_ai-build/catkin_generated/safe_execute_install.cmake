execute_process(COMMAND "/home/hamid/Desktop/parsian_ws/src/parsian_ssl/parsian_ai-build/catkin_generated/python_distutils_install.sh" RESULT_VARIABLE res)

if(NOT res EQUAL 0)
  message(FATAL_ERROR "execute_process(/home/hamid/Desktop/parsian_ws/src/parsian_ssl/parsian_ai-build/catkin_generated/python_distutils_install.sh) returned error code ")
endif()
