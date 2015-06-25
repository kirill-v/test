SOURCES = server.cpp led.cpp
HEADERS = led.h server.h

main: $(SOURCES) $(HEADERS) main.cpp

	g++ -g $(SOURCES) main.cpp -o test --std=c++11

# server: $(SERVER_SOURCES) led.h
# 	echo "Hello"