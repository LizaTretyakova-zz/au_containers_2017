all: ./bin/test

./bin/test: src/test.cpp
	g++ -static src/test.cpp -o $@

clean:
	rm -rf ./bin/*

.PHONY: clean
