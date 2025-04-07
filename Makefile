build: 
	mkdir -p dist/
	cc ./src/htmlpaint.c -Wno-unused-result -o ./dist/htmlpaint
run: ./dist/htmlpaint
	./dist/htmlpaint
