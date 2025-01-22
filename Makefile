run:
	@gcc index.c -o index
	@./index

clean:
	@rm -f index request

request:
	@gcc request.c -o request
	@./request