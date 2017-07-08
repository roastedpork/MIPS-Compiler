bin/c_compiler: src/codegen/c_parser.tab.o src/codegen/lex.yy.o src/codegen/codegen_ast.o
	g++ -o bin/c_compiler src/codegen/*.o

bin/c_codegen: src/codegen/c_parser.tab.o src/codegen/lex.yy.o src/codegen/codegen_ast.o
	g++ -o bin/c_codegen src/codegen/*.o

bin/c_parser: src/parser/c_parser.tab.o src/parser/lex.yy.o src/parser/ast.o
	g++ -o bin/c_parser src/parser/*.o

src/codegen/lex.yy.o: src/codegen/lex.yy.c src/codegen/c_parser.tab.h
	g++ -c src/codegen/lex.yy.c
	mv lex.yy.o src/codegen

src/codegen/c_parser.tab.o: src/codegen/c_parser.tab.c src/codegen/c_parser.tab.h
	g++ -c src/codegen/c_parser.tab.c
	mv c_parser.tab.o src/codegen

src/codegen/codegen_ast.o: src/codegen/codegen_ast.cpp src/codegen/codegen_ast.hpp
	g++ -c src/codegen/codegen_ast.cpp
	mv codegen_ast.o src/codegen

src/codegen/c_parser.tab.h: src/codegen/c_parser.y
	bison -d src/codegen/c_parser.y
	mv c_parser.tab.h src/codegen
	mv c_parser.tab.c src/codegen
	
src/codegen/c_parser.tab.c: src/codegen/c_parser.y
	bison -d src/codegen/c_parser.y

src/codegen/lex.yy.c: src/codegen/c_lexer.lex
	flex src/codegen/c_lexer.lex
	mv lex.yy.c src/codegen


src/parser/lex.yy.o: src/parser/lex.yy.c src/parser/c_parser.tab.h
	g++ -c src/parser/lex.yy.c
	mv lex.yy.o src/parser

src/parser/c_parser.tab.o: src/parser/c_parser.tab.c src/parser/c_parser.tab.h
	g++ -c src/parser/c_parser.tab.c
	mv c_parser.tab.o src/parser

src/parser/ast.o: src/parser/ast.cpp src/parser/ast.hpp
	g++ -c src/parser/ast.cpp
	mv ast.o src/parser

src/parser/c_parser.tab.h: src/parser/c_parser.y
	bison -d src/parser/c_parser.y
	mv c_parser.tab.h src/parser
	mv c_parser.tab.c src/parser

src/parser/c_parser.tab.c: src/parser/c_parser.y
	bison -d src/parser/c_parser.y

src/parser/lex.yy.c: src/parser/c_lexer.lex
	flex src/parser/c_lexer.lex 
	mv lex.yy.c src/parser

clean/codegen:
	rm -f src/codegen/lex.yy.c src/codegen/c_parser.tab.* src/codegen/*.o bin/c_codegen
	
clean/parser:
	rm -f src/parser/lex.yy.c src/parser/c_parser.tab.* src/parser/*.o bin/c_parser