run:
	@gcc index.c -o index
	@./index

clean:
	@rm -f index request_compiled

test_request:
	@gcc ./request/request.c -o request_compiled
	@./request_compiled
	@rm -f request_compiled