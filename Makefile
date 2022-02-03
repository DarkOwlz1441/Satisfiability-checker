logic2: scanner.l parser.y
		bison parser.y -d
		flex scanner.l
		gcc parser.tab.c lex.yy.c -o test.out