

# Provides an option that the user can optionally select.
# Can accept condition to control when option is available for user.
# Usage:
#   option(<option_variable>
#          "help string describing the option"
#          <initial value or boolean expression>
#          [VISIBLE_IF <condition>]
#          [VERIFY <condition>])
macro(PG_OPTION variable description value)
  set(__value ${value})
  set(__condition "")
  set(__verification)
  set(__varname "__value")
  foreach(arg ${ARGN})
    if(arg STREQUAL "IF" OR arg STREQUAL "if" OR arg STREQUAL "VISIBLE_IF")
      set(__varname "__condition")
    elseif(arg STREQUAL "VERIFY")
      set(__varname "__verification")
    else()
      list(APPEND ${__varname} ${arg})
    endif()
  endforeach()
  unset(__varname)
  if(__condition STREQUAL "")
    set(__condition 2 GREATER 1)
  endif()

  if(${__condition})
    if(__value MATCHES ";")
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    elseif(DEFINED ${__value})
      if(${__value})
        option(${variable} "${description}" ON)
      else()
        option(${variable} "${description}" OFF)
      endif()
    else()
      option(${variable} "${description}" ${__value})
    endif()
  else()
    if(DEFINED ${variable} AND "${${variable}}"  # emit warnings about turned ON options only.
        AND NOT (OPENCV_HIDE_WARNING_UNSUPPORTED_OPTION OR "$ENV{OPENCV_HIDE_WARNING_UNSUPPORTED_OPTION}")
    )
      message(WARNING "Unexpected option: ${variable} (=${${variable}})\nCondition: IF (${__condition})")
    endif()
    if(OPENCV_UNSET_UNSUPPORTED_OPTION)
      unset(${variable} CACHE)
    endif()
  endif()
  if(__verification)
    set(OPENCV_VERIFY_${variable} "${__verification}") # variable containing condition to verify
    list(APPEND OPENCV_VERIFICATIONS "${variable}") # list of variable names (WITH_XXX;WITH_YYY;...)
  endif()
  unset(__condition)
  unset(__value)
endmacro()

# execute_process(COMMAND conan install . -r=xf_conan -u
#   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
# )
# include(conanbuildinfo.cmake)
# conan_basic_setup()
