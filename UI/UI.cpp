#include "UI.h"
#include <fstream>
#include <sstream>
#include <iostream>

Editor::Editor()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "C Syntax Checker - Editor"),
      cursorLine(0), cursorCol(0), scrollOffset(0.0f)
{
    // Initialize colors
    bgColor = sf::Color(40, 44, 52);
    textColor = sf::Color(171, 178, 191);
    lineNumColor = sf::Color(92, 99, 112);
    cursorColor = sf::Color(255, 255, 255, 200);
    errorColor = sf::Color(255, 100, 100, 100);
    keywordColor = sf::Color(198, 120, 221);
    
    // Setup areas
    textArea.setSize(sf::Vector2f(WINDOW_WIDTH - LEFT_MARGIN, WINDOW_HEIGHT - STATUS_HEIGHT));
    textArea.setPosition(LEFT_MARGIN, 0);
    textArea.setFillColor(bgColor);
    
    lineNumberArea.setSize(sf::Vector2f(LEFT_MARGIN, WINDOW_HEIGHT - STATUS_HEIGHT));
    lineNumberArea.setPosition(0, 0);
    lineNumberArea.setFillColor(sf::Color(33, 37, 43));
    
    statusBarRect.setSize(sf::Vector2f(WINDOW_WIDTH, STATUS_HEIGHT));
    statusBarRect.setPosition(0, WINDOW_HEIGHT - STATUS_HEIGHT);
    statusBarRect.setFillColor(sf::Color(33, 37, 43));
    
    // Initial content
    lines.push_back("// C Code Editor");
    lines.push_back("// Nhập code C của bạn tại đây");
    lines.push_back("");
    lines.push_back("int main() {");
    lines.push_back("    return 0;");
    lines.push_back("}");
}

bool Editor::loadFont(const string& fontPath)
{
    if (!font.loadFromFile(fontPath))
    {
        cerr << "Không thể load font: " << fontPath << endl;
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
    
    return true;
}

void Editor::handleInput(sf::Event& event)
{
    if (event.type == sf::Event::TextEntered)
    {
        if (event.text.unicode < 128)
        {
            char c = static_cast<char>(event.text.unicode);
            
            if (c == '\r' || c == '\n')
            {
                newLine();
            }
            else if (c == '\b') // Backspace
            {
                deleteChar();
            }
            else if (c >= 32) // Printable characters
            {
                insertChar(c);
            }
        }
    }
    else if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Left)
            moveCursor(0, -1);
        else if (event.key.code == sf::Keyboard::Right)
            moveCursor(0, 1);
        else if (event.key.code == sf::Keyboard::Up)
            moveCursor(-1, 0);
        else if (event.key.code == sf::Keyboard::Down)
            moveCursor(1, 0);
        else if (event.key.code == sf::Keyboard::F5)
            checkSyntax();
    }
}

void Editor::insertChar(char c)
{
    if (cursorLine >= lines.size())
        lines.push_back("");
    
    lines[cursorLine].insert(cursorCol, 1, c);
    cursorCol++;
}

void Editor::deleteChar()
{
    if (cursorCol > 0)
    {
        lines[cursorLine].erase(cursorCol - 1, 1);
        cursorCol--;
    }
    else if (cursorLine > 0)
    {
        // Merge with previous line
        cursorCol = lines[cursorLine - 1].length();
        lines[cursorLine - 1] += lines[cursorLine];
        lines.erase(lines.begin() + cursorLine);
        cursorLine--;
    }
}

void Editor::newLine()
{
    string remaining = lines[cursorLine].substr(cursorCol);
    lines[cursorLine] = lines[cursorLine].substr(0, cursorCol);
    lines.insert(lines.begin() + cursorLine + 1, remaining);
    cursorLine++;
    cursorCol = 0;
}

void Editor::moveCursor(int dLine, int dCol)
{
    cursorLine = max(0, min((int)lines.size() - 1, cursorLine + dLine));
    cursorCol = max(0, min((int)lines[cursorLine].length(), cursorCol + dCol));
}

void Editor::checkSyntax()
{
    // Combine all lines
    stringstream ss;
    for (const auto& line : lines)
        ss << line << "\n";
    
    string source = ss.str();
    
    // Lexer
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    
    // Parser with diagnostics
    diag.clear();
    Parser parser(tokens);
    parser.setDiagnosticReporter(&diag);
    
    semantics sem;
    parser.setSemantics(&sem);
    parser.parseProgram();
    
    errors = diag.all();
    
    cout << "Kiểm tra cú pháp: tìm thấy " << errors.size() << " lỗi" << endl;
}

void Editor::updateDisplay()
{
    // Update line numbers
    stringstream lineNumStr;
    int visibleLines = (WINDOW_HEIGHT - STATUS_HEIGHT) / LINE_HEIGHT;
    int startLine = (int)scrollOffset / LINE_HEIGHT;
    
    for (int i = startLine; i < min(startLine + visibleLines, (int)lines.size()); i++)
    {
        lineNumStr << (i + 1) << "\n";
    }
    
    lineNumbers.setString(lineNumStr.str());
    lineNumbers.setPosition(10, -scrollOffset);
    
    // Update text content
    stringstream contentStr;
    for (int i = startLine; i < min(startLine + visibleLines + 5, (int)lines.size()); i++)
    {
        contentStr << lines[i] << "\n";
    }
    
    displayText.setString(contentStr.str());
    displayText.setPosition(LEFT_MARGIN + 5, -scrollOffset);
    
    // Update status bar
    stringstream status;
    status << "Line: " << (cursorLine + 1) << ", Col: " << (cursorCol + 1);
    status << " | Errors: " << errors.size();
    status << " | F5: Check Syntax";
    statusBar.setString(status.str());
    statusBar.setPosition(10, WINDOW_HEIGHT - STATUS_HEIGHT + 5);
}

void Editor::drawCursor()
{
    float x = LEFT_MARGIN + 5 + cursorCol * 9.6f; // Approximate char width
    float y = cursorLine * LINE_HEIGHT - scrollOffset;
    
    sf::RectangleShape cursor(sf::Vector2f(2, LINE_HEIGHT));
    cursor.setPosition(x, y);
    cursor.setFillColor(cursorColor);
    
    window.draw(cursor);
}

void Editor::drawErrorHighlights()
{
    for (const auto& err : errors)
    {
        int line = err.line - 1;
        if (line < 0 || line >= lines.size())
            continue;
        
        float x = LEFT_MARGIN + 5 + err.col * 9.6f;
        float y = line * LINE_HEIGHT - scrollOffset;
        float width = err.length * 9.6f;
        
        sf::RectangleShape highlight(sf::Vector2f(width, LINE_HEIGHT));
        highlight.setPosition(x, y);
        highlight.setFillColor(errorColor);
        
        window.draw(highlight);
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
                window.close();
            else
                handleInput(event);
        }
        
        // Cursor blink
        if (clock.getElapsedTime().asSeconds() > 0.5f)
        {
            showCursor = !showCursor;
            clock.restart();
        }
        
        updateDisplay();
        
        window.clear(bgColor);
        window.draw(lineNumberArea);
        window.draw(textArea);
        window.draw(statusBarRect);
        
        drawErrorHighlights();
        window.draw(lineNumbers);
        window.draw(displayText);
        
        if (showCursor)
            drawCursor();
        
        window.draw(statusBar);
        window.display();
    }
}

void Editor::loadFile(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Không thể mở file: " << filename << endl;
        return;
    }
    
    lines.clear();
    string line;
    while (getline(file, line))
        lines.push_back(line);
    
    if (lines.empty())
        lines.push_back("");
    
    cursorLine = 0;
    cursorCol = 0;
    file.close();
}

void Editor::saveFile(const string& filename)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cerr << "Không thể ghi file: " << filename << endl;
        return;
    }
    
    for (const auto& line : lines)
        file << line << "\n";
    
    file.close();
    cout << "Đã lưu file: " << filename << endl;
}