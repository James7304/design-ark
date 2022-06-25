#pragma once
#include "CTextLineObject.h"


class CTextEditorObject :
    public CAppObject
{
public:
    // Public contructors
    CTextEditorObject(CRect bounds, CString ID, BOOL lineNums, int maxLines = 0, int defBoxHeight = 0, BOOL active = FALSE, std::vector<CString> text = {L""}, std::vector<int> line = {1}, int lineOffset = 0);
    ~CTextEditorObject();
    
    // Public implementations
    int draw(CDC* pDc, CSize textExtent, int xScrollPosition, int returnNewLines, BOOL printing, CRect printAreaLength = CRect());

    // Public getters, setters and checkers
    int getCaretPos();
    CPoint getCaretPoint(CSize caretSize);
    CSize getTextExtent();
    void setTextExtent(CSize size);

    int getRecentPos();

    BOOL pointHighlighted(CPoint point);
    BOOL hasHighlight();
    void hlghtingOff();
    CRect getHighlightClippingRect();
    CRgn* getHighlightExactRgn(int x_offset, int y_offset);
    int getStartLine();
    BOOL isHlghtMultiline();
    int lineHighlight(int line); 
    // RETURNS  0 for none
    //          1 for partially highlighted
    //          2 for line highlighted

    std::vector <int> iGetLineNum(int a = 0);
    CString sGetLineNum(int a = 0);

    void incrementSublines(int subline, int val);
    int getActiveLine();
    int getBlockLine();
    int getPrintLine();
    CString getLineText(int line = 0);
    CRect getLineBounds(int line = 0);
    CString getHighlightedText();
    int getLineTextWidth();
    int getNumLines();
    int getBoxHeight(BOOL default = FALSE);

    virtual void setActive(BOOL active);
    virtual void move(int x, int y);
    virtual void setBounds(CRect bounds);
    std::tuple<CRect, int> getPrintBounds(int returnNewLines, int printAreaLength);

    int getCursorArrow();

    void initialise();

    // Public message handlers
    virtual void OnSize(UINT nType, int x, int y);

    virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);
    virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual void OnRButtonUp(UINT nFlags, CPoint point);
    virtual void OnRButtonDown(UINT nFlags, CPoint point);
    virtual BOOL OnMouseMove(UINT nFlags, CPoint point);

    virtual int OnRecieveText(CString text, BOOL open = FALSE);
    virtual void OnRecieveBackspace();
    virtual void OnRecieveTab();
    virtual void OnRecieveReturn(BOOL open = FALSE);

    virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual BOOL OnContextMenu(CWnd* pWnd, CPoint point);
    
private:

    // Private Implementations
    void setSidebar();
    void setHighlighter();
    void setBracketsNull();

    // Private resources

    // Positioning
    int activeLine;
    int mouseLine;
    int blockLine;
    int printLine;
    int caretPos;

    // Sizes
    int defBoxHeight;
    CSize textExtent;

    // Highlights
    BOOL hlght;             // Used to tell the editor that there is a highlight
    BOOL hlghting;          // Used to tell that the user is currently highlighting
    CPoint hlghtStartP;     // Stores the start of the highlight
    CPoint hlghtEndP;       // Stores the end/current mouse pos for the highlighting
    int startPos;           // Stores the original starting pos in terms of character spaces for the highlight
    int startLine;          // Stores the original starting line for the highlight

    // Recents
    BOOL recentHlght;
    int recentPos;
    int recentLine;

    // Line stuff
    std::vector<CTextLineObject*> lines;
    int maxLines;
    int lineOffset;
    BOOL lineNums;
    BOOL hoverSubLine;
    BOOL clickSubLine;
    CRect sidebar;

    // Editing
    std::vector<std::vector<int>> brackets;

    /*
    
    Brackets contain the positions and types of all the auto completed brackets currently in the editor.
    Auto-brackets must always be on the same line and it will only be auto for as long the user is editing that line.

    Each record in brackets contain three integers. 
    
    [x][0] contains the position of the first bracket.
    [x][1] contains the position of the second bracket.
    [x][2] contains the type of bracket that has been saved.

    if [x][2] == 0 : type = (
                 1 : type = {
                 2 : type = [
                 3 : type = "
                 4 : type = '
    
    */


    BOOL bracketContains(int value, int& itPos, int type, int side = 1);
    void moveBrackets(int val, int index);

    // Stuff for view
    int cursor_arrow;
    BOOL lMouseDown;
};

