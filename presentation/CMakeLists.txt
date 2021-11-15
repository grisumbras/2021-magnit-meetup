add_custom_command(
  OUTPUT presentation.html
  COMMAND bundler exec asciidoctor-revealjs
    -R "${CMAKE_CURRENT_SOURCE_DIR}"
    -D "${CMAKE_CURRENT_BINARY_DIR}"
    -a revealjsdir=https://cdn.jsdelivr.net/npm/reveal.js@3.9.2
    "${CMAKE_CURRENT_SOURCE_DIR}/presentation.adoc"
  MAIN_DEPENDENCY presentation.adoc
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_target(
  presentation
  ALL
  DEPENDS presentation.html
  COMMENT "Generating presentation"
)

