all:

	@make backend
	
	@make frontend
	
	
backend: backend/backend.o
	@gcc -c backend/items/itemsList.c globals.h debug.h
	@gcc -o backend/backend backend/backend.c backend/items/itemsList.c backend/users/users_lib.o -pthread
		
frontend: frontend/frontend.o
	@gcc -o frontend/frontend frontend/frontend.c globals.h debug.h -pthread
	
clean:
	@rm frontend/frontend frontend/frontend.o backend/backend backend/backend.o itemsList.o
