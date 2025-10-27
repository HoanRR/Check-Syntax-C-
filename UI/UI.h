#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>
#include "../lexer/Lexer.h"
#include "../parser/Parser.h"
#include "../Diagnostic/DiagnosticReporter.h"

using namespace std;

class Editor
{
private:
    struct ToolbarButton
    {
        sf::RectangleShape shape;
        sf::Text label;
        std::function<void()> onClick;
    };

    sf::RenderWindow window;
    sf::Font font;
    sf::Text displayText;
    sf::Text lineNumbers;
    sf::Text statusBar;
    sf::RectangleShape textArea;
    sf::RectangleShape lineNumberArea;
    sf::RectangleShape statusBarRect;
    sf::RectangleShape toolbarRect;

    vector<string> lines;
    int cursorLine;
    int cursorCol;
    float scrollOffset;
    bool dirty;

    string currentFilePath;
    string statusMessage;
    float charWidth;

    // Diagnostic
    DiagnosticReporter diag;
    vector<DiagnosticItem> errors;

    // Visual settings
    const int FONT_SIZE = 16;
    const int LINE_HEIGHT = 20;
    const int LEFT_MARGIN = 60;
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 700;
    const int STATUS_HEIGHT = 30;
    const int TOOLBAR_HEIGHT = 40;
    const int BUTTON_WIDTH = 110;
    const int BUTTON_HEIGHT = 28;

    // Colors
    sf::Color bgColor;
    sf::Color textColor;
    sf::Color lineNumColor;
    sf::Color cursorColor;
    sf::Color errorColor;
    sf::Color keywordColor;

    vector<ToolbarButton> toolbarButtons;

    void handleInput(sf::Event& event);
    void handleMousePress(const sf::Vector2f& position);
    void handleScroll(float delta);
    void ensureCursorVisible();
    void updateDisplay();
    void drawCursor();
    void drawErrorHighlights();
    void drawToolbar();
    void checkSyntax();
    void insertChar(char c);
    void deleteChar();
    void deleteForwardChar();
    void newLine();
    void moveCursor(int dLine, int dCol);
    void moveCursorToStart();
    void moveCursorToEnd();
    void setupLayout();
    void setupToolbar();
    void setStatus(const string& message);
    string promptFilePath(const string& prompt);
    void setCursorFromMouse(const sf::Vector2f& position);

public:
    Editor();
    bool loadFont(const string& fontPath);
    void run();
    void loadFile(const string& filename);
    void saveFile(const string& filename);
};