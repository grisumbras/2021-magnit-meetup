file(GLOB parts
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  part-*.adoc
)

file(GLOB pics
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  *.jpg *.png
)

file(GLOB styles
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  *.css
)

file(GLOB cpp_sources
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  ../test/*.cpp
)

file(GLOB fonts
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  *.otf
)

add_custom_command(
  OUTPUT presentation.html
  COMMAND bundler exec asciidoctor-revealjs
    -R "${CMAKE_CURRENT_SOURCE_DIR}"
    -D "${CMAKE_CURRENT_BINARY_DIR}"
    -a attribute-missing=warn
    --failure-level=WARN
    "${CMAKE_CURRENT_SOURCE_DIR}/presentation.adoc"
  MAIN_DEPENDENCY presentation.adoc
  DEPENDS
    ${pics}
    ${styles}
    ${cpp_sources}
    ${parts}
    ${fonts}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_target(
  presentation
  ALL
  DEPENDS presentation.html
  COMMENT "Generating presentation"
)

include(GNUInstallDirs)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/presentation.html"
    ${pics}
    ${styles}
    ${fonts}
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}"
)
