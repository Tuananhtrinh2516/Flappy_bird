# Trình biên dịch
CXX = clang++

# Các cờ biên dịch
CXXFLAGS = -std=c++17 $(shell sdl2-config --cflags --libs) -lSDL2_ttf -lSDL2_image

# Tên file thực thi
TARGET = flappy_bird

# File mã nguồn
SRCS = main.cpp

# Mục tiêu mặc định
all: $(TARGET)

# Biên dịch chương trình
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)  # <-- Dùng tab ở đây

# Xóa file thực thi
clean:
	rm -f $(TARGET)  # <-- Dùng tab ở đây