project('freetype', 'c', version : '1.3.1')

libfreetype = declare_dependency(
		link_with : static_library('libfreetype1', 'lib/arch/ansi/freetype.c', 
		include_directories : include_directories(['lib', 'lib/arch/ansi'])), 
		include_directories : include_directories('lib'))
