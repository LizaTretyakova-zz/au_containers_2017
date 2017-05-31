all: ./bin/test

./bin/test: ./src/test.cpp
	g++ --static $< -o $@

clean:
	rm -rf ./bin/*

.PHONY: clean
