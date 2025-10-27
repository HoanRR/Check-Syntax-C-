#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "../lexer/Lexer.h"
#include "../parser/Parser.h"
#include "../Diagnostic/DiagnosticReporter.h"

using namespace std;

class Editor
{
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text displayText;
    sf::Text lineNumbers;
    sf::Text statusBar;
    sf::RectangleShape textArea;
    sf::RectangleShape lineNumberArea;
    sf::RectangleShape statusBarRect;
    
    vector<string> lines;
    int cursorLine;
    int cursorCol;
    float scrollOffset;
    
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
    
    // Colors
    sf::Color bgColor;
    sf::Color textColor;
    sf::Color lineNumColor;
    sf::Color cursorColor;
    sf::Color errorColor;
    sf::Color keywordColor;
    
    void handleInput(sf::Event& event);
    void updateDisplay();
    void drawCursor();
    void drawErrorHighlights();
    void checkSyntax();
    string getColoredText();
    void insertChar(char c);
    void deleteChar();
    void newLine();
    void moveCursor(int dLine, int dCol);
    
public:
    Editor();
    bool loadFont(const string& fontPath);
    void run();
    void loadFile(const string& filename);
    void saveFile(const string& filename);
};