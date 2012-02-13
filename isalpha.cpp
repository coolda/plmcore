/* isalpha.c */
#include "isalpha.h"
bool isAlpha(char c) {
	return ((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z'))||(c=='_');
}
bool isAlNum(char c){
	return isAlpha(c) || isNum(c);
}
bool isAlNumDot(char c){
	return isAlpha(c) || isNum(c) || isDot(c);
}
bool isAssign(char c){
	return (c == '=');
}
bool isSemicolon(char c) {
	return (c == ';');
}
bool isLn(char c) {
	return (c == '\n');
}
bool isZero(char c)　{
	return (c=='0');
}
bool isComma(char c){
	return (c == ',');
}
bool isPound(char c){
	return (c == '#');
}
bool isNum(char c){
	return (c>='0')&&(c<='9');
}
bool isDot(char c){
	return (c == '.');
}
bool isNumDot(char c){
	return isNum(c) || isDot(c);
}
bool isLParen(char c){
	return (c =='(');
}
bool isRParen(char c){
	return (c == ')');
}
bool isAddSub(char c){
	return (c == '+') || (c == '-');
}
bool isAdd(char c){
	return (c == '+');
}
bool isPlus(char c){
	return (c == '+');
}
bool isMinus(char c){
	return (c == '-');
}
bool isSub(char c){
	return (c == '-');
}
bool isMulDiv(char c){
	return (c == '*') || (c == '/');
}
bool isMul(char c){
	return (c == '*');
}
bool isStar(char c){
	return (c=='*');
}
bool isDelim(char c){
	return (c == '\0');
}

bool isPow(char c){
	return (c == '^');
}
bool isWhite(char c){
	return (c == ' ') || (c == '\t');
}
bool isLCurl(char c){
	return (c == '{');
}
bool isRCurl(char c){
	return (c == '}');
}
bool isLess(char c){
	return (c == '<');
}
bool isGreater(char c){
	return (c == '>');
}
// known informally as a bang or a shriek
bool isBang(char c){
	return (c == '!');
}
bool isDquote(char c) {
	return (c=='"');
}	
bool isQuote(char c) {
	return (c=='\'');
}
// Square Brackets
bool isLSquare(char c) {
	return (c=='[');
}
bool isRSquare(char c) {
	return (c==']');
}
bool isSlash(char c) {
	return (c=='/');
}
bool isDiv(char c) {
	return (c=='/');
}
bool isAt(char c) {
	return (c=='@');
}
bool isDollar(char c) {
	return ( c== '$');
}
bool isAnd(char c) {
	return (c=='&');
}
bool isCapital(char c) {
	return ( c >= 65 && c <= 90 );
}
bool isLower(char c) {
	return (c>=97 && c<=122);
}
bool isOr(char c){
	return (c=='|');
}
bool isI(char c) {
	return (c=='i');
}
bool isF(char c) {
	return (c=='f');
}
bool isS (char c) {
	return (c=='s');
}
bool isAlNumQuote(char c ) {
	return isAlpha(c) || isNum(c) || isQuote(c);
}
bool isColon(char c) {
	return (c==':');
}
bool isAlNumAnd(char c) {
	return isAlpha(c) || isNum(c) || isAnd(c);
}
bool isAlAnd(char c) {
	return isAlpha(c) || isAnd(c);
}
