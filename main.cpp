#include "UI/UI.h"
#include <iostream>

int main(int argc, char *argv[])
{
    Editor editor;

    // Load font - bạn cần có file font (ví dụ: arial.ttf, consolas.ttf)
    // Có thể download từ: C:/Windows/Fonts/ (Windows)
    if (!editor.loadFont("UI/monotype-sorts-regular.ttf"))
    {
        // Thử các font khác
        if (!editor.loadFont("UI/monotype-sorts-regular.ttf"))
        {
            std::cerr << "Không thể load font. Vui lòng cập nhật đường dẫn font." << std::endl;
            return 1;
        }
    }

    // Load file nếu có argument
    if (argc > 1)
    {
        editor.loadFile(argv[1]);
    }

    std::cout << "=== C Syntax Checker Editor ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - Arrow keys: Di chuyển cursor" << std::endl;
    std::cout << "  - F5: Kiểm tra cú pháp" << std::endl;
    std::cout << "  - Type normally: Nhập code" << std::endl;
    std::cout << "  - Backspace: Xóa ký tự" << std::endl;
    std::cout << "================================" << std::endl;

    editor.run();

    return 0;
}