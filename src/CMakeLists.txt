add_executable(SimpleCInterpreter
Common.h
CompiledProgram.h
Interpreter.h
main.cpp
Parser.h
SourceCode.h
Tokenizer.h
)
target_link_libraries(
  SimpleCInterpreter
  PRIVATE project_options
          project_warnings
          CONAN_PKG::fmt
          CONAN_PKG::spdlog
	  CONAN_PKG::magic_enum
	  )
