#include "pch.h"
#include "CTextHandler.h"

// Public constructors
CTextHandler::CTextHandler()
{
    this->shft = FALSE;
}
CTextHandler::~CTextHandler()
{
}

// Public commands
CString CTextHandler::RecieveText()
{
    CString temp = this->text;
    this->text = "";
    return temp;
}
void CTextHandler::DestroyText()
{
    this->text = "";
}

// Public message handlers
BOOL CTextHandler::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL caps = GetKeyState(VK_CAPITAL);

    switch (nChar) {

    case VK_SPACE:
        this->text.Append(L" ");
        break;

    case VK_SHIFT:
        this->shft = TRUE;
        break;

    case 0x30:
        if (this->shft) {
            this->text.Append(L")");
        }
        else {
            this->text.Append(L"0");
        }
        break;

    case 0x31:
        if (this->shft) {
            this->text.Append(L"!");
        }
        else {
            this->text.Append(L"1");
        }
        break;

    case 0x32:
        if (this->shft) {
            this->text.Append(L"\"");
        }
        else {
            this->text.Append(L"2");
        }
        break;

    case 0x33:
        if (this->shft) {
            this->text.Append(L"£");
        }
        else {
            this->text.Append(L"3");
        }
        break;

    case 0x34:
        if (this->shft) {
            this->text.Append(L"$");
        }
        else {
            this->text.Append(L"4");
        }
        break;

    case 0x35:
        if (this->shft) {
            this->text.Append(L"%");
        }
        else {
            this->text.Append(L"5");
        }
        break;

    case 0x36:
        if (this->shft) {
            this->text.Append(L"^");
        }
        else {
            this->text.Append(L"6");
        }
        break;

    case 0x37:
        if (this->shft) {
            this->text.Append(L"&");
        }
        else {
            this->text.Append(L"7");
        }
        break;

    case 0x38:
        if (this->shft) {
            this->text.Append(L"*");
        }
        else {
            this->text.Append(L"8");
        }
        break;

    case 0x39:
        if (this->shft) {
            this->text.Append(L"(");
        }
        else {
            this->text.Append(L"9");
        }
        break;

    case 0x41:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"A");
        }
        else {
            this->text.Append(L"a");
        }
        break;

    case 0x42:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"B");
        }
        else {
            this->text.Append(L"b");
        }
        break;

    case 0x43:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"C");
        }
        else {
            this->text.Append(L"c");
        }
        break;

    case 0x44:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"D");
        }
        else {
            this->text.Append(L"d");
        }
        break;

    case 0x45:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"E");
        }
        else {
            this->text.Append(L"e");
        }
        break;

    case 0x46:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"F");
        }
        else {
            this->text.Append(L"f");
        }
        break;

    case 0x47:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"G");
        }
        else {
            this->text.Append(L"g");
        }
        break;

    case 0x48:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"H");
        }
        else {
            this->text.Append(L"h");
        }
        break;

    case 0x49:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"I");
        }
        else {
            this->text.Append(L"i");
        }
        break;

    case 0x4A:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"J");
        }
        else {
            this->text.Append(L"j");
        }
        break;

    case 0x4B:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"K");
        }
        else {
            this->text.Append(L"k");
        }
        break;

    case 0x4C:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"L");
        }
        else {
            this->text.Append(L"l");
        }
        break;

    case 0x4D:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"M");
        }
        else {
            this->text.Append(L"m");
        }
        break;

    case 0x4E:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"N");
        }
        else {
            this->text.Append(L"n");
        }
        break;

    case 0x4F:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"O");
        }
        else {
            this->text.Append(L"o");
        }
        break;

    case 0x50:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"P");
        }
        else {
            this->text.Append(L"p");
        }
        break;

    case 0x51:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"Q");
        }
        else {
            this->text.Append(L"q");
        }
        break;

    case 0x52:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"R");
        }
        else {
            this->text.Append(L"r");
        }
        break;

    case 0x53:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"S");
        }
        else {
            this->text.Append(L"s");
        }
        break;

    case 0x54:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"T");
        }
        else {
            this->text.Append(L"t");
        }
        break;

    case 0x55:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"U");
        }
        else {
            this->text.Append(L"u");
        }
        break;

    case 0x56:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"V");
        }
        else {
            this->text.Append(L"v");
        }
        break;

    case 0x57:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"W");
        }
        else {
            this->text.Append(L"w");
        }
        break;

    case 0x58:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"X");
        }
        else {
            this->text.Append(L"x");
        }
        break;

    case 0x59:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"Y");
        }
        else {
            this->text.Append(L"y");
        }
        break;

    case 0x5A:
        if (!(this->shft && caps) && (this->shft || caps)) {
            this->text.Append(L"Z");
        }
        else {
            this->text.Append(L"z");
        }
        break;

    case VK_MULTIPLY:
        this->text.Append(L"*");
        break;

    case VK_ADD:
        this->text.Append(L"+");
        break;

    case VK_SUBTRACT:
        this->text.Append(L"-");
        break;

    case VK_DIVIDE:
        this->text.Append(L"/");
        break;

    case VK_OEM_1:
        if (this->shft) {
            this->text.Append(L":");
        }
        else {
            this->text.Append(L";");
        }
        break;

    case VK_OEM_PLUS:
        if (this->shft) {
            this->text.Append(L"+");
        }
        else {
            this->text.Append(L"=");
        }
        break;

    case VK_OEM_COMMA:
        if (this->shft) {
            this->text.Append(L"<");
        }
        else {
            this->text.Append(L",");
        }
        break;

    case VK_OEM_MINUS:
        if (this->shft) {
            this->text.Append(L"_");
        }
        else {
            this->text.Append(L"-");
        }
        break;

    case VK_OEM_PERIOD:
        if (this->shft) {
            this->text.Append(L">");
        }
        else {
            this->text.Append(L".");
        }
        break;

    case VK_OEM_2:
        if (this->shft) {
            this->text.Append(L"?");
        }
        else {
            this->text.Append(L"/");
        }
        break;

    case VK_OEM_3:
        if (this->shft) {
            this->text.Append(L"@");
        }
        else {
            this->text.Append(L"'");
        }
        break;

    case VK_OEM_4:
        if (this->shft) {
            this->text.Append(L"{");
        }
        else {
            this->text.Append(L"[");
        }
        break;

    case VK_OEM_5:
        if (this->shft) {
            this->text.Append(L"|");
        }
        else {
            this->text.Append(L"\\");
        }
        break;

    case VK_OEM_6:
        if (this->shft) {
            this->text.Append(L"}");
        }
        else {
            this->text.Append(L"]");
        }
        break;

    default:
        return FALSE;

    }
    return TRUE;
}
BOOL CTextHandler::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_SHIFT) {
        this->shft = FALSE;
    }
    return 0;
}

