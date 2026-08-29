# Found on stack overflow Set colour string values
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold "${Esc}[1m")
  set(Red "${Esc}[31m")
  set(Green "${Esc}[32m")
  set(Yellow "${Esc}[33m")
  set(Blue "${Esc}[34m")
  set(Magenta "${Esc}[35m")
  set(Cyan "${Esc}[36m")
  set(White "${Esc}[37m")
  set(BoldRed "${Esc}[1;31m")
  set(BoldGreen "${Esc}[1;32m")
  set(BoldYellow "${Esc}[1;33m")
  set(BoldBlue "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan "${Esc}[1;36m")
  set(BoldWhite "${Esc}[1;37m")
endif()

# Overload the message function
function(message)
  if(ARGC EQUAL 0)
    _message()
    return()
  endif()

  list(GET ARGV 0 MessageType)

  # Only these modes get colour. Everything else (NOTICE, VERBOSE, DEBUG,
  # TRACE, CHECK_START/CHECK_PASS/CHECK_FAIL, or a plain message with no
  # mode keyword at all) is forwarded to the real message() untouched, so
  # native behaviors - like CHECK_START/CHECK_PASS's indentation - still
  # work. Passing "${ARGV}" (quoted) here would flatten a multi-arg call
  # into one semicolon-joined string, breaking exactly that.
  if(MessageType STREQUAL FATAL_ERROR OR MessageType STREQUAL SEND_ERROR)
    list(REMOVE_AT ARGV 0)
    _message(${MessageType} "${BoldRed}" ${ARGV} "${ColourReset}")
  elseif(MessageType STREQUAL WARNING)
    list(REMOVE_AT ARGV 0)
    _message(${MessageType} "${BoldYellow}" ${ARGV} "${ColourReset}")
  elseif(MessageType STREQUAL AUTHOR_WARNING)
    list(REMOVE_AT ARGV 0)
    _message(${MessageType} "${BoldCyan}" ${ARGV} "${ColourReset}")
  elseif(MessageType STREQUAL STATUS)
    list(REMOVE_AT ARGV 0)
    _message(${MessageType} "${Green}" ${ARGV} "${ColourReset}")
  else()
    _message(${ARGV})
  endif()
endfunction(message)
