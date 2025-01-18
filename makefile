build: 
	@g++ ./cmd/main.cpp -o out

run: 
	@g++ ./cmd/main.cpp -o out && ./out -h localhost -p 8080 -u pythonwithsean -p idk123 -db test