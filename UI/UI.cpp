#include "UI.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

using std::max;
using std::min;

namespace
{
constexpr float TEXT_PADDING = 12.0f;
constexpr float SCROLL_STEP = 60.0f;
}

Editor::Editor()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "C Syntax Checker - Editor"),
      cursorLine(0),
      cursorCol(0),
      scrollOffset(0.0f),
      dirty(false),
      charWidth(9.5f)
{
    window.setFramerateLimit(60);

    bgColor = sf::Color(40, 44, 52);
    textColor = sf::Color(222, 226, 233);
    lineNumColor = sf::Color(130, 137, 150);
    cursorColor = sf::Color(255, 255, 255, 220);
    errorColor = sf::Color(255, 100, 100, 110);
    keywordColor = sf::Color(198, 120, 221);

    setupLayout();

    lines.push_back("");
    statusMessage = "Sẵn sàng.";
}

void Editor::setupLayout()
{
    toolbarRect.setSize(sf::Vector2f(WINDOW_WIDTH, TOOLBAR_HEIGHT));
    toolbarRect.setPosition(0, 0);
    toolbarRect.setFillColor(sf::Color(33, 37, 43));

    textArea.setSize(
        sf::Vector2f(WINDOW_WIDTH - LEFT_MARGIN, WINDOW_HEIGHT - STATUS_HEIGHT - TOOLBAR_HEIGHT));
    textArea.setPosition(LEFT_MARGIN, TOOLBAR_HEIGHT);
    textArea.setFillColor(bgColor);

    lineNumberArea.setSize(
        sf::Vector2f(LEFT_MARGIN, WINDOW_HEIGHT - STATUS_HEIGHT - TOOLBAR_HEIGHT));
    lineNumberArea.setPosition(0, TOOLBAR_HEIGHT);
    lineNumberArea.setFillColor(sf::Color(33, 37, 43));

    statusBarRect.setSize(sf::Vector2f(WINDOW_WIDTH, STATUS_HEIGHT));
    statusBarRect.setPosition(0, WINDOW_HEIGHT - STATUS_HEIGHT);
    statusBarRect.setFillColor(sf::Color(24, 26, 32));
}

bool Editor::loadFont(const string& fontPath)
{
    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "Không thể load font: " << fontPath << std::endl;
        return false;
    }

    displayText.setFont(font);
    displayText.setCharacterSize(FONT_SIZE);
    displayText.setFillColor(textColor);

    lineNumbers.setFont(font);
    lineNumbers.setCharacterSize(FONT_SIZE);
    lineNumbers.setFillColor(lineNumColor);

    statusBar.setFont(font);
    statusBar.setCharacterSize(14);
    statusBar.setFillColor(sf::Color::White);

    charWidth = font.getGlyph(' ', FONT_SIZE, false).advance;
    if (charWidth <= 0.f)
        charWidth = 9.5f;

    setupToolbar();

    return true;
}

void Editor::setupToolbar()
{
    toolbarButtons.clear();

    const std::vector<std::pair<std::string, std::function<void()>>> buttonDefs = {
        {"Tập tin mới", [this]() {
             lines.assign(1, "");
             cursorLine = 0;
             cursorCol = 0;
             scrollOffset = 0.f;
             errors.clear();
             dirty = false;
             currentFilePath.clear();
             setStatus("Đã tạo tập tin mới.");
         }},
        {"Mở...", [this]() {
             const std::string path = promptFilePath("Nhập đường dẫn file cần mở: ");
             if (!path.empty())
             {
                 loadFile(path);
             }
         }},
        {"Lưu", [this]() {
             std::string path = currentFilePath;
             if (path.empty())
                 path = promptFilePath("Nhập đường dẫn file cần lưu: ");
             if (!path.empty())
             {
                 saveFile(path);
             }
         }},
        {"Kiểm tra", [this]() { checkSyntax(); }}
    };

    float x = 10.f;
    for (const auto& def : buttonDefs)
    {
        ToolbarButton button;
        button.shape.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
        button.shape.setPosition(x, (TOOLBAR_HEIGHT - BUTTON_HEIGHT) / 2.f);
        button.shape.setFillColor(sf::Color(52, 58, 64));
        button.shape.setOutlineColor(sf::Color(86, 93, 102));
        button.shape.setOutlineThickness(1.f);

        button.label.setFont(font);
        button.label.setCharacterSize(14);
        button.label.setFillColor(sf::Color::White);
        button.label.setString(def.first);
        sf::FloatRect bounds = button.label.getLocalBounds();
        button.label.setOrigin(bounds.left + bounds.width / 2.f,
                               bounds.top + bounds.height / 2.f);
        button.label.setPosition(x + BUTTON_WIDTH / 2.f, TOOLBAR_HEIGHT / 2.f);

        button.onClick = def.second;
        toolbarButtons.push_back(button);

        x += BUTTON_WIDTH + 10.f;
    }
}

void Editor::setStatus(const string& message)
{
    statusMessage = message;
    std::cout << "[Editor] " << message << std::endl;
}

string Editor::promptFilePath(const string& prompt)
{
    std::cout << prompt;
    std::cout.flush();

    std::string path;
    if (!std::getline(std::cin, path))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return "";
    }

    // Trim spaces
    const auto first = path.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";
    const auto last = path.find_last_not_of(" \t");
    return path.substr(first, last - first + 1);
}

void Editor::handleInput(sf::Event& event)
{
    if (event.type == sf::Event::TextEntered)
    {
        if (event.text.unicode == 9) // Tab
        {
            insertChar(' ');
            insertChar(' ');
            insertChar(' ');
            insertChar(' ');
        }
        else if (event.text.unicode == 8) // Backspace handled in TextEntered on some systems
        {
            deleteChar();
        }
        else if (event.text.unicode < 128)
        {
            const char c = static_cast<char>(event.text.unicode);
            if (c == '\r' || c == '\n')
            {
                newLine();
            }
            else if (c >= 32)
            {
                insertChar(c);
            }
        }
    }
    else if (event.type == sf::Event::KeyPressed)
    {
        const bool ctrl = event.key.control;

        if (ctrl && event.key.code == sf::Keyboard::S)
        {
            std::string path = currentFilePath;
            if (path.empty())
                path = promptFilePath("Nhập đường dẫn file cần lưu: ");
            if (!path.empty())
            {
                saveFile(path);
            }
        }
        else if (ctrl && event.key.code == sf::Keyboard::O)
        {
            const std::string path = promptFilePath("Nhập đường dẫn file cần mở: ");
            if (!path.empty())
            {
                loadFile(path);
            }
        }
        else if (ctrl && event.key.code == sf::Keyboard::N)
        {
            lines.assign(1, "");
            cursorLine = 0;
            cursorCol = 0;
            scrollOffset = 0.f;
            errors.clear();
            dirty = false;
            currentFilePath.clear();
            setStatus("Đã tạo tập tin mới.");
        }
        else
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Left:
                moveCursor(0, -1);
                break;
            case sf::Keyboard::Right:
                moveCursor(0, 1);
                break;
            case sf::Keyboard::Up:
                moveCursor(-1, 0);
                break;
            case sf::Keyboard::Down:
                moveCursor(1, 0);
                break;
            case sf::Keyboard::PageUp:
                moveCursor(-(WINDOW_HEIGHT - STATUS_HEIGHT - TOOLBAR_HEIGHT) / LINE_HEIGHT, 0);
                break;
            case sf::Keyboard::PageDown:
                moveCursor((WINDOW_HEIGHT - STATUS_HEIGHT - TOOLBAR_HEIGHT) / LINE_HEIGHT, 0);
                break;
            case sf::Keyboard::Home:
                moveCursorToStart();
                break;
            case sf::Keyboard::End:
                moveCursorToEnd();
                break;
            case sf::Keyboard::BackSpace:
                deleteChar();
                break;
            case sf::Keyboard::Delete:
                deleteForwardChar();
                break;
            case sf::Keyboard::F5:
                checkSyntax();
                break;
            default:
                break;
            }
        }
    }
}

void Editor::handleMousePress(const sf::Vector2f& position)
{
    for (auto& button : toolbarButtons)
    {
        if (button.shape.getGlobalBounds().contains(position))
        {
            if (button.onClick)
                button.onClick();
            return;
        }
    }

    if (textArea.getGlobalBounds().contains(position))
    {
        setCursorFromMouse(position);
    }
}

void Editor::setCursorFromMouse(const sf::Vector2f& position)
{
    if (lines.empty())
        lines.push_back("");

    int line = static_cast<int>((position.y - TOOLBAR_HEIGHT + scrollOffset) / LINE_HEIGHT);
    line = max(0, min(line, static_cast<int>(lines.size()) - 1));

    cursorLine = line;

    float x = position.x - (LEFT_MARGIN + TEXT_PADDING);
    int col = 0;
    if (x > 0.f)
        col = static_cast<int>((x / charWidth) + 0.5f);

    cursorCol = max(0, min(col, static_cast<int>(lines[cursorLine].size())));
    ensureCursorVisible();
}

void Editor::handleScroll(float delta)
{
    const float viewHeight = WINDOW_HEIGHT - STATUS_HEIGHT - TOOLBAR_HEIGHT;
    const float contentHeight = lines.size() * LINE_HEIGHT;
    const float maxScroll = std::max(0.f, contentHeight - viewHeight);

    scrollOffset -= delta * SCROLL_STEP;
    scrollOffset = std::clamp(scrollOffset, 0.f, maxScroll);
}

void Editor::ensureCursorVisible()
{
    const float viewHeight = WINDOW_HEIGHT - STATUS_HEIGHT - TOOLBAR_HEIGHT;
    const float cursorTop = cursorLine * LINE_HEIGHT;
    const float cursorBottom = cursorTop + LINE_HEIGHT;

    if (cursorTop < scrollOffset)
        scrollOffset = cursorTop;
    else if (cursorBottom > scrollOffset + viewHeight)
        scrollOffset = cursorBottom - viewHeight;

    const float contentHeight = lines.size() * LINE_HEIGHT;
    const float maxScroll = std::max(0.f, contentHeight - viewHeight);
    scrollOffset = std::clamp(scrollOffset, 0.f, maxScroll);
}

void Editor::insertChar(char c)
{
    if (cursorLine >= static_cast<int>(lines.size()))
        lines.push_back("");

    lines[cursorLine].insert(cursorCol, 1, c);
    cursorCol++;
    dirty = true;
    if (statusMessage != "Đang chỉnh sửa...")
        statusMessage = "Đang chỉnh sửa...";
    ensureCursorVisible();
}

void Editor::deleteChar()
{
    if (cursorLine >= static_cast<int>(lines.size()))
        return;

    if (cursorCol > 0)
    {
        lines[cursorLine].erase(cursorCol - 1, 1);
        cursorCol--;
        dirty = true;
    }
    else if (cursorLine > 0)
    {
        cursorCol = static_cast<int>(lines[cursorLine - 1].length());
        lines[cursorLine - 1] += lines[cursorLine];
        lines.erase(lines.begin() + cursorLine);
        cursorLine--;
        dirty = true;
    }

    ensureCursorVisible();
}

void Editor::deleteForwardChar()
{
    if (cursorLine >= static_cast<int>(lines.size()))
        return;

    if (cursorCol < static_cast<int>(lines[cursorLine].length()))
    {
        lines[cursorLine].erase(cursorCol, 1);
        dirty = true;
    }
    else if (cursorLine + 1 < static_cast<int>(lines.size()))
    {
        lines[cursorLine] += lines[cursorLine + 1];
        lines.erase(lines.begin() + cursorLine + 1);
        dirty = true;
    }

    ensureCursorVisible();
}

void Editor::newLine()
{
    if (cursorLine >= static_cast<int>(lines.size()))
        lines.push_back("");

    std::string remaining = lines[cursorLine].substr(cursorCol);
    lines[cursorLine] = lines[cursorLine].substr(0, cursorCol);
    lines.insert(lines.begin() + cursorLine + 1, remaining);
    cursorLine++;
    cursorCol = 0;
    dirty = true;
    ensureCursorVisible();
}

void Editor::moveCursor(int dLine, int dCol)
{
    if (lines.empty())
        lines.push_back("");

    cursorLine = max(0, min(static_cast<int>(lines.size()) - 1, cursorLine + dLine));
    cursorCol = max(0, min(static_cast<int>(lines[cursorLine].length()), cursorCol + dCol));
    ensureCursorVisible();
}

void Editor::moveCursorToStart()
{
    cursorCol = 0;
    ensureCursorVisible();
}

void Editor::moveCursorToEnd()
{
    if (cursorLine < static_cast<int>(lines.size()))
        cursorCol = static_cast<int>(lines[cursorLine].length());
    ensureCursorVisible();
}

void Editor::checkSyntax()
{
    std::stringstream ss;
    for (const auto& line : lines)
        ss << line << '\n';

    const std::string source = ss.str();

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    diag.clear();
    Parser parser(tokens);
    parser.setDiagnosticReporter(&diag);

    semantics sem;
    parser.setSemantics(&sem);
    parser.parseProgram();

    errors = diag.all();
    setStatus("Kiểm tra cú pháp: tìm thấy " + std::to_string(errors.size()) + " lỗi.");

    std::cout << "Kiểm tra cú pháp: tìm thấy " << errors.size() << " lỗi" << std::endl;
}

void Editor::updateDisplay()
{
    const int totalLines = static_cast<int>(lines.size());
    const int viewLines = (WINDOW_HEIGHT - STATUS_HEIGHT - TOOLBAR_HEIGHT) / LINE_HEIGHT + 2;
    const int startLine = static_cast<int>(scrollOffset) / LINE_HEIGHT;

    std::stringstream lineNumStr;
    for (int i = startLine; i < min(startLine + viewLines, max(1, totalLines)); ++i)
        lineNumStr << (i + 1) << '\n';

    lineNumbers.setString(lineNumStr.str());
    lineNumbers.setPosition(LEFT_MARGIN - 10.f, TOOLBAR_HEIGHT - scrollOffset);

    std::stringstream contentStr;
    for (int i = startLine; i < min(startLine + viewLines, totalLines); ++i)
        contentStr << lines[i] << '\n';

    displayText.setString(contentStr.str());
    displayText.setPosition(LEFT_MARGIN + TEXT_PADDING, TOOLBAR_HEIGHT - scrollOffset);

    std::stringstream status;
    status << "Line: " << (cursorLine + 1) << ", Col: " << (cursorCol + 1);
    status << " | Errors: " << errors.size();
    if (dirty)
        status << " | *Chưa lưu";
    status << " | F5: Kiểm tra cú pháp";
    if (!statusMessage.empty())
        status << " | " << statusMessage;

    statusBar.setString(status.str());
    statusBar.setPosition(12.f, WINDOW_HEIGHT - STATUS_HEIGHT + 6.f);
}

void Editor::drawCursor()
{
    const float x = LEFT_MARGIN + TEXT_PADDING + cursorCol * charWidth;
    const float y = TOOLBAR_HEIGHT + cursorLine * LINE_HEIGHT - scrollOffset;

    sf::RectangleShape cursor(sf::Vector2f(2.f, LINE_HEIGHT));
    cursor.setPosition(x, y);
    cursor.setFillColor(cursorColor);

    window.draw(cursor);
}

void Editor::drawErrorHighlights()
{
    for (const auto& err : errors)
    {
        const int line = err.line - 1;
        if (line < 0 || line >= static_cast<int>(lines.size()))
            continue;

        const float x = LEFT_MARGIN + TEXT_PADDING + err.col * charWidth;
        const float y = TOOLBAR_HEIGHT + line * LINE_HEIGHT - scrollOffset;
        const float width = max(1, err.length) * charWidth;

        sf::RectangleShape highlight(sf::Vector2f(width, LINE_HEIGHT));
        highlight.setPosition(x, y);
        highlight.setFillColor(errorColor);
        window.draw(highlight);
    }
}

void Editor::drawToolbar()
{
    window.draw(toolbarRect);
    const sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (auto& button : toolbarButtons)
    {
        const bool hovered = button.shape.getGlobalBounds().contains(mousePos);
        button.shape.setFillColor(hovered ? sf::Color(72, 78, 86) : sf::Color(52, 58, 64));
        window.draw(button.shape);
        window.draw(button.label);
    }
}

void Editor::run()
{
    sf::Clock clock;
    bool showCursor = true;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                const sf::Vector2f pos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                handleMousePress(pos);
            }
            else if (event.type == sf::Event::MouseWheelScrolled)
            {
                handleScroll(event.mouseWheelScroll.delta);
            }
            else
            {
                handleInput(event);
            }
        }

        if (clock.getElapsedTime().asSeconds() > 0.5f)
        {
            showCursor = !showCursor;
            clock.restart();
        }

        updateDisplay();

        window.clear(bgColor);
        window.draw(lineNumberArea);
        window.draw(textArea);
        drawErrorHighlights();
        window.draw(displayText);
        window.draw(lineNumbers);
        drawToolbar();
        window.draw(statusBarRect);
        window.draw(statusBar);

        if (showCursor)
            drawCursor();

        window.display();
    }
}

void Editor::loadFile(const string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Không thể mở file: " << filename << std::endl;
        setStatus("Không thể mở file: " + filename);
        return;
    }

    lines.clear();
    std::string line;
    while (std::getline(file, line))
        lines.push_back(line);

    if (lines.empty())
        lines.push_back("");

    cursorLine = 0;
    cursorCol = 0;
    scrollOffset = 0.f;
    errors.clear();
    dirty = false;

    setStatus("Đã mở: " + filename);
    currentFilePath = filename;
}

void Editor::saveFile(const string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Không thể ghi file: " << filename << std::endl;
        setStatus("Không thể ghi file: " + filename);
        return;
    }

    for (const auto& line : lines)
        file << line << '\n';

    dirty = false;
    currentFilePath = filename;
    setStatus("Đã lưu: " + filename);
    std::cout << "Đã lưu file: " << filename << std::endl;
}
