CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++20 -Isrc
LDFLAGS = 
OBJDIR = obj
SRCDIR = src

SERVER_TARGET = kierki-serwer
CLIENT_TARGET = kierki-klient

SERVER_SRCS := $(wildcard $(SRCDIR)/server/*.cpp)
CLIENT_SRCS := $(wildcard $(SRCDIR)/client/*.cpp)
COMMON_SRCS := $(wildcard $(SRCDIR)/common/*.cpp)

SERVER_OBJS := $(SERVER_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 
CLIENT_OBJS := $(CLIENT_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
COMMON_OBJS := $(COMMON_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS) $(COMMON_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) -o $@ $^

$(CLIENT_TARGET): $(CLIENT_OBJS) $(COMMON_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(SERVER_TARGET) $(CLIENT_TARGET)

.PHONY: all clean
