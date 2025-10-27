#include "UI/UI.h"
#include <iostream>

int main(int argc, char *argv[])
{
    Editor editor;

    // Load font - bạn cần có file font (ví dụ: Consolas, Fira Code...)
    if (!editor.loadFont("UI/monotype-sorts-regular.ttf"))
    {
        std::cerr << "Không thể load font. Vui lòng cập nhật đường dẫn font." << std::endl;
        return 1;
    }

    // Load file nếu có argument
    if (argc > 1)
    {
        editor.loadFile(argv[1]);
    }

    std::cout << "=== C Syntax Checker Editor ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - Chuột: Chọn dòng/cột, dùng thanh công cụ để Mở/Lưu/Kiểm tra" << std::endl;
    std::cout << "  - Ctrl + N: Tạo file mới" << std::endl;
    std::cout << "  - Ctrl + O: Mở file" << std::endl;
    std::cout << "  - Ctrl + S: Lưu file" << std::endl;
    std::cout << "  - F5: Kiểm tra cú pháp" << std::endl;
    std::cout << "  - Phím mũi tên, PageUp/PageDown, Home/End: di chuyển con trỏ" << std::endl;
    std::cout << "  - Nhập văn bản trực tiếp, Backspace/Delete để chỉnh sửa" << std::endl;
    std::cout << "================================" << std::endl;

    editor.run();

    return 0;
}