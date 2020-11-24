add_library(coord2b SHARED fsm.c)

install(
  TARGETS coord2b
  EXPORT coord2b-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# Forward exported libraries
set(EXPORTED_LIBRARIES coord2b PARENT_SCOPE)
