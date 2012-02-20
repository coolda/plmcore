// plm.cpp
// plmcore_asis_min3
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <cstdlib>
#include <cstring>
#include "isalpha.h"
#include "pl_error.h"
#include "stack.h"
using namespace std;
#define MAX_CODE_SIZE			1024	
#define MAX_NAME_TABLE_SIZE	1024

string str1;
string gstr;
char look;
char peek;
int strIndex;

// mem base offset
int immcnt=200;
const int ary_base=300;

// address
int cx;				// code[]index counting
int adr;				// name table counting
int left_adr;		// adr before increment

// array
bool aryFlag;
bool aryindexFlag;
bool aryintFlag;
bool arystrFlag;
bool aryfltFlag;
vector<int> multi_kindvec;	// added feb15
int multi_cnt;				// added 

// for
int fx1, fx2, fx3, fx4, fx5;	
bool forFlag;
// ifz if zero no else or anything
int iz1;

// ife els combination
int ifex1, ifex2, ifex3;

// immediate str and flt
int imminfo;

// inc dec
int incAdr;
int decAdr;
bool incFlag;
bool decFlag;
bool factopFlag;

// interp
int reg1, reg2;		// reg1 is result reg2 is return adr

// name table
int origtyp;

// procedure
int px1;		// used in proc part patch

// procedure call
bool proc_callFlag;
bool procFlag;
int proc_calcnt;

// rhs
bool rhsFlag;

// string
string strary[32];
bool no_stoFlag;

// string immediate
bool lparenFlag;		// then const immediate str may be used

// while patch
int wx1, wx2;		// used in while

// write info for operand1
int wrtinfo;
bool wrtmultiFlag; // added feb15

// keyset_ini
string key1, key2, key3, key4, key5;
set<string> keyset;

enum  { INT=1, CHA, FLT, STR, ARY, 
		ARYTMP, ARYINT, ARYCHA, ARYFLT, ARYSTR, 
		ARYINDEX, PRO, IMM, IMMCHA, IMMFLT, 
		IMMSTR, LID, LIDINT, LIDFLT, LIDSTR, 
	    LODCMD, MULTI 
};
//Instruction types 
enum {
	OP_NUL, OP_LIT, OP_OPR, OP_LOD, OP_STO, 
	OP_CAL, OP_INC, OP_JMP, OP_JPC, OP_WRT,
	OP_LDA, OP_CMD, OP_LODCMD, OP_IMM, 	
};
// Arithmetic operation types
enum {
	OPR_RET, OPR_NEG, OPR_ADD, OPR_SUB, OPR_MUL, 
	OPR_DIV, OPR_ODD, OPR_EQL, OPR_NEQ, OPR_LSS, 
	OPR_LEQ, OPR_GTR, OPR_GEQ, OPR_NOT, OPR_AND,
	OPR_OR, OPR_LID, OPR_SID, OPR_INC, OPR_DEC
};
//instruction names for print purposes 
const char *op_name[] = {
	"nul", "lit", "opr", "lod", "sto", 
	"cal", "inc", "jmp", "jpc", "wrt", 
	"lda", "cmd", "lodcmd", "imm"
};
//arithmetic operation names for print */
const char *opr_name[] = {
	"ret", "neg", "add", "sub", "mul", 
	"div", "odd", "eql", "neq", "lss", 
	"leq", "gtr", "geq", "not", "and",
	"or",  "lid", "sid", "inc", "dec"
};
// foward declarations
void expression();
void statement();
void inc_stmnt();
void dec_stmnt();

map<string, int> pdispmap;
bool set_pdispmap(string& str, int num) {
	pdispmap[str] = num;
	return true;
}
int get_pdispmap(string& str) {
	if (pdispmap.find(str) != pdispmap.end() )
		return pdispmap[str];
	else
		cout << "no such name in pdispmap \n";
	return 0;
}
void dolook_s() {
	// not doing skipWhite
	look = str1[strIndex];
	strIndex++;
}
void skipWhite() {
	while (isWhite(look))
		dolook_s();
}
char dopeek() {
	return str1[strIndex];
}
void dolook() {
	look = str1[strIndex];
	strIndex++;
	while (isWhite(look))
		dolook();
}
void skipParens() {
	if (isLParen(look))
		dolook();
		else
			pl_error(43);	
	if (isRParen(look))	
			dolook();
		else
			pl_error(22);
} 
struct Instruction{
	int opcode;
	int operand1;
	int operand2;
} code[MAX_CODE_SIZE];
void gen(int opcode, int operand1, int operand2) {
	if(cx > MAX_CODE_SIZE)
		pl_error(31);
	else {
		code[cx].opcode = opcode;
		code[cx].operand1 = operand1;
		code[cx++].operand2 = operand2;
	}
}
// global Nametable
struct Nametable {
	int			kind;
	int 			addr;
	string		name;
 	int			ints;
	float		flt;
	string		str;
	char			chars[64];
} table[MAX_NAME_TABLE_SIZE];
void set_nametab(string& s1) {
	table[adr].addr=adr;
	table[adr].name=s1;
	left_adr=adr;
	adr++;
}
int isin_tab(string& s1) {
	int j=-1;
	if ( (adr==1) && (table[0].name == s1) )
		j=0;
	else {
		for(int i = 0; i < adr; i++) {
			if(table[i].name == s1) 
				return i;
		}
	}
	return j;
}
void set_tabkind(string& s1, int k) {
	int n;
	n= isin_tab(s1);
	if (n != -1)
		table[n].kind=k;
	else 
		pl_error(62);
}
void set_tabints(string& s1, int i) {
	int n;
	n= isin_tab(s1);
	if (n != -1)
		table[n].ints=i;
	else 
		pl_error(62);
}
void set_tabflt(string& s1, float f1) {
	int n;
	n= isin_tab(s1);
	if (n != -1)
		table[n].flt=f1;
	else 
		pl_error(62);	
}
void set_tabstr(string& s1, string& s2) {
	int n;
	n= isin_tab(s1);
	if (n != -1)
		table[n].str=s2;
	else 
		pl_error(62);
}
int get_tabadr(string& s1) {
	int n, j;
	n= isin_tab(s1);
	if (n != -1)
		j=table[n].addr;
	else
		j=-1;
	return j;
}
int get_tabints(string& s1) {
	int n, j;
	n= isin_tab(s1);
	if (n != -1)
		j=table[n].ints;
	else
		j=-1;
	return j;
}
float get_tabflt(string& s1) {
	int n;
	float flt1;
	n= isin_tab(s1);
	if (n != -1)
		flt1=table[n].flt;	
	return flt1;
}
string& get_tabname(int adr) {
	return table[adr].name;
}
int get_tabkind(string& s1) {
	int n, j;
	n= isin_tab(s1);
	if (n != -1)
		j=table[n].kind;
	else
		j=-1;
	return j;
}
void keyset_ini() {
	// int cnt=keyset.count(gstr); rtn 1 or 0
	key1="for";
	key2="ife";
	key3="ifz";
	key4="while";
	key5="write";
	keyset.insert(key1);
	keyset.insert(key2);
	keyset.insert(key3);
	keyset.insert(key4);
	keyset.insert(key5);	
}
void condition() {
	bool notFlag=false;
	bool logic_andFlag=false;
	bool logic_orFlag=false;
	bool again_aFlag=false;
	bool again_orFlag=false;
	if (isBang(look)) {
		notFlag=true;
		dolook();
		// check another LParen after this
		if (isLParen(look)) {
			// then simply remove 
			dolook();
		}
	}	
AGAIN:	
	expression();
	char cnext = dopeek();
	int relop;
	if ( isAssign(look) ) {
		if ( isAssign(cnext) ) {	// case ==
			relop = OPR_EQL;
			dolook();	// increment to 2nd '='
		}
		else
			pl_error(55);
	}
	else if ( isBang(look) ) {
		if ( isAssign(cnext) ) {	// case !=
			relop = OPR_NEQ;
			dolook();
		}
		else
			pl_error(56);
	}
	else if ( isLess(look) ) {  
		if ( isAssign(cnext)) {	// case <=
			relop = OPR_LEQ;
			dolook();
		}
		else 
			relop = OPR_LSS;  // then <
	}
	else if ( isGreater(look)) {
		if ( isAssign(cnext)) {
			relop = OPR_GEQ;
			dolook();
		}
		else
			relop = OPR_GTR;
	}
	dolook();
	expression();
	if (isAnd(look)) {
		char c=dopeek();
		if (isAnd(c))
			logic_andFlag=true;
	}
	else if (isOr(look)) {
		char c=dopeek();
		if (isOr(c))
			logic_orFlag=true;
	}
	gen(OP_OPR, 0, relop);
	if (notFlag) {
		gen(OP_OPR, 0, OPR_NOT);
		notFlag=false;
	}
	if (logic_andFlag) {
		logic_andFlag=false; // no need
		dolook(); // 1st &
		dolook(); // 2nd &
		again_aFlag=true;
		goto AGAIN;
	}
	if (logic_orFlag) {
		logic_orFlag=false; // no need 
		dolook(); // 1st |
		dolook(); // 2nd |
		again_orFlag=true;
		goto AGAIN;
	}
	if (again_aFlag) {
		gen(OP_OPR, 0, OPR_AND);
		again_aFlag=false;
	}
	if (again_orFlag) {
		gen(OP_OPR, 0, OPR_OR);
		again_orFlag=false;
	}
}
void paren_cond() {
	if (isLParen(look))
		dolook();
	else
		pl_error(43);
	condition();
	if (isRParen(look))	
		dolook();
	else
		pl_error(22);
}
void paren_expre() {
	if (isLParen(look)) {
		dolook();
		expression();
		if (isRParen(look))
			dolook();
		else
			pl_error(22);
	}
}
void factor() {
	int n1, n2, n3, n4; 
	int n7, n8, n9;
	int n14, n15;
	int k1, k2, k3, k4 ;
	float f1;
	string s2, s3;
	string key1="while";
	string key2="for";
	string key3="write";
	string key4="ife";
	string key5="ifz";
	if (isAssign(look)) {
		rhsFlag=true;
		dolook();
	}
	if (isPlus(look)) {
		char c=dopeek();
		if (isPlus(c)) {
			// "inc found: "
			n14=isin_tab(gstr);
			dolook();	// '+'
			dolook();	// '+'
			if (n14 != -1) {
				incAdr = n14;
				factopFlag=true;
				inc_stmnt();
				goto TAIL;
			}
		}
	}
	if (isMinus(look)) {
		char c=dopeek();
		if (isMinus(c)) {
			// "dec found: " 
			n15=isin_tab(gstr);
			dolook();	// '-'
			dolook();	// '-'
			if (n15 != -1) {
				decAdr = n15;
				factopFlag=true;
				dec_stmnt();
				goto TAIL;
			}
		}
	}
	
	if (isLParen(look))
		paren_expre();
	if ( isNum(look) ) {
		string s1;
		do {
			s1 += look;
			dolook();
		} while ( isNumDot(look) );
		int pos=s1.find('.');
		f1 = atof(s1.c_str());
		n7 = atoi(s1.c_str());
		if (pos != -1 ) { 
			// set kind of left thing here
			k3=get_tabkind(gstr);
			// float situation 
			if (k3==ARYTMP || k3==ARYINT || k3==ARYSTR) {
				set_tabkind(gstr, ARYFLT);
				aryfltFlag=true;
				multi_kindvec.push_back(ARYFLT);
			}
			else 
				set_tabkind(gstr, FLT);
			
			gen(OP_IMM, IMMFLT, immcnt);
			//	imminfo=ARYFLT;
			table[immcnt++].flt=f1;
			wrtinfo=IMMFLT;
		}
		else {
			// "input immediate int case: n7: "
			
			if (aryindexFlag) {
				// "aryindex setting: "  
				// lets change this from ARYINDEX to ARYINT
				gen(OP_LIT, ARYINT, n7);	// ARYINDEX==11
				aryindexFlag=false;
				//		curr_ary_index=n7;
				if (gstr=="write") {
					n3=multi_kindvec[n7];
					wrtinfo=n3;
				}
				else 
					wrtinfo=ARYINT;
			}
			
			// call keyset.count(gstr) 
			else if (keyset.count(gstr)==0){
				// "gstr is not a keyword: "
				n8=isin_tab(gstr);
				k4=get_tabkind(gstr);
				// in case of compound array
				// has to be changed the kind to ARYINT
				if (k4==ARYTMP || k4==ARYFLT || k4==ARYSTR) {
					set_tabkind(gstr, ARYINT);
					aryintFlag=true;
					gen(OP_LIT, ARYINT, n7);
					wrtinfo=ARYINT;
					multi_kindvec.push_back(ARYINT);
				}
				else {
					if (k4==0)
						set_tabkind(gstr, INT);
					
					gen(OP_LIT, INT, n7);
					wrtinfo=INT;
				}
			}
			
			else {
				gen(OP_LIT, INT, n7);
				wrtinfo=INT;
				no_stoFlag=false;
			}
	//		cout << "lit: " << n7 << endl;
		}
	}
	else if (isAlpha(look)) {
		do {
			s2 += look;
			dolook_s();
		} while ( isAlNumDot(look) );
		skipWhite();
		// check the kind
		k1=get_tabkind(s2);
		if (keyset.count(gstr)==0) {
			k2=get_tabkind(gstr);
			if (k2==0)
				set_tabkind(gstr, k1);		// gstr==a
		}
		n1=isin_tab(s2);
		
		if (isPlus(look)) {
			char c = dopeek();
			if (isPlus(c)) {
				// inc thing
				dolook();	// '+'
				dolook();	// '+'
				incFlag=true;
				incAdr = n1;
			}
		}
		
		if (isMinus(look)) {
			char c = dopeek();
			if (isMinus(c)) {
				// "dec situation: "
				dolook();		// '-'
				dolook();		// '-'
				decFlag=true;
				decAdr = n1;
			}
		}
		
		if (isLSquare(look)) {
			if (k1==ARYINT)
				aryintFlag=true;
			else if (k1==ARYFLT)
				aryfltFlag=true;
			else if(k1==ARYSTR) 
				arystrFlag=true;
			goto LSQ;
		}
		if (n1 != -1) {
			gen( OP_LOD, k1, n1);
			wrtinfo=k1;
		//	if (isRSquare(look)) 
		//		dolook();
		}
		else 
			pl_error(62);
	}
	else if (isDquote(look)) {
		string s3;
		dolook();		// remove dquote
		do {
			s3 += look;
			dolook_s();
		} while ( !isDquote(look) );
		skipWhite();
		dolook(); // get rid dquote
		// put this s3 to temporary area of table[] which is above 200
		table[immcnt].str=s3;
		gen(OP_IMM, IMMSTR, immcnt++);
		wrtinfo=IMMSTR;
		
		// check gstr kind call keyset.count(gstr) 
		if (keyset.count(gstr)==0){
			// "gstr is not a keyword: "
			n2 = isin_tab(gstr);
			// set kind of gstr,  msg in this case
			// check the curr state of kind gstr
			n9= get_tabkind(gstr);
			// if aryindexFlag is on then case is mys[0]="hello";
			// use aryFlag which is available up to gen LID
			if (n9==ARYSTR) { // 10
				// "kind is already set to ARYSTR: "
				arystrFlag=true;
				wrtinfo=ARYSTR;
				multi_kindvec.push_back(ARYSTR);
			}
			else if (n9==ARYTMP || n9==ARYINT || n9==ARYFLT) {
				// "set_tabkind to ARYSTR:
				set_tabkind(gstr, ARYSTR );
				arystrFlag=true;
				wrtinfo=ARYSTR;		// try this and see
				multi_kindvec.push_back(ARYSTR);
			}
			else { 
				// "set_tabkind to STR: " 
				set_tabkind(gstr, STR );
				wrtinfo=STR ;
			}
		}
	}	
	else if (isQuote(look)) {
		string s4;
		int sz;
		int n4; 
		dolook();	// quote
		do {
			s4 += look;
			dolook_s();
		} while ( !isQuote(look) );
		skipWhite();
		dolook(); 	// quote
		n4=isin_tab(gstr);
		// set kind of msg which is gstr
		if (get_tabkind(gstr)==0)
			set_tabkind(gstr, CHA );
		sz=s4.size();
		if (sz > 64 )
			pl_error(64);
		int j=0;
		char cha[sz];
		while (j<sz) {
			cha [j]=s4[j];
			j++;
		}
		strcpy(table[immcnt].chars, cha);
		gen(OP_IMM, IMMCHA, immcnt++);
		wrtinfo=IMMCHA ;
	}
	else if (isLSquare(look)) {
		// a[0]=10; in this case, still left hand side
		int n5;
		aryFlag=true;
		// set kind thing
		// put newly created ARYTMP here
		if (get_tabkind(gstr)==0)
			set_tabkind(gstr, ARYTMP);
		n5=isin_tab(gstr);
		
	LSQ:		
		
		gen(OP_LDA, ARYTMP, ary_base);	// start at [300]
		wrtinfo=ARYTMP;
		
		if (isLSquare(look)) {
			// next will be array index int num or alpha identifier
			aryindexFlag=true;
			dolook();
			expression();
			dolook();
			gen(OP_OPR, 0, OPR_ADD);
		}
		
		if (isAssign(look)) {
			dolook();
			expression();
			
			if (aryintFlag) {
				gen(OP_OPR, ARYINT, OPR_SID);
				aryintFlag=false;
				wrtinfo=ARYINT;
				no_stoFlag=true;
			}
			else if (arystrFlag) {
				gen(OP_OPR, ARYSTR, OPR_SID);
				arystrFlag=false;
				wrtinfo=ARYSTR;
				no_stoFlag=true;
			}
			
			else if (aryfltFlag) { 
				gen(OP_OPR, ARYFLT, OPR_SID);
				aryfltFlag=false;
				wrtinfo=ARYFLT;
				no_stoFlag=true;
			}
		}
		else {
			n4=multi_kindvec.size();
			if(n4>0 && gstr=="write") { 
				gen(OP_OPR, MULTI, OPR_LID);
				wrtinfo=MULTI;
			}
			else if (aryintFlag) { 
				gen(OP_OPR, ARYINT, OPR_LID);
				aryintFlag=false;
				wrtinfo=ARYINT;
				no_stoFlag=true;
			}
			
			else if (aryfltFlag) {
				gen(OP_OPR, ARYFLT, OPR_LID);
				aryfltFlag=false;
				wrtinfo=ARYFLT;
				no_stoFlag=true;
			}
			
			else if (arystrFlag) {
				gen(OP_OPR, ARYSTR, OPR_LID);
				arystrFlag=false;
				wrtinfo=ARYSTR;
				no_stoFlag=true;
			}
		}
	}
	else
		pl_error(52);
TAIL:
	cout << "";
}

void term() {
	factor();
	while ( isMul(look) ) {
		dolook();
		factor();
		gen(OP_OPR, 0, OPR_MUL);
	}
	while ( isDiv(look) ) {
		dolook();
		factor();
		gen(OP_OPR, 0, OPR_DIV);
	} 
	if ( no_stoFlag )
		no_stoFlag=false;
}
void expression() {
	term();
	while ( isAdd(look) ) {
		dolook();
		term();
		gen(OP_OPR, 0, OPR_ADD);
	}
	while ( isSub(look) ) {
		dolook();
		term();
		gen(OP_OPR, 0, OPR_SUB);
	}
	if ( no_stoFlag )
		no_stoFlag=false;
}
void ife_stmnt() {
	string s1;
	paren_cond();
	ifex1 = cx;
	gen(OP_JPC, 0, 0);
	if (isLCurl(look)) {
		dolook();
		while (!isRCurl(look)) {
			statement();
			dolook();
		}
	}	
	else 
		statement();
	dolook();	// remove semicolon 
	do {
		s1 += look;
		dolook_s();
	} while (isAlpha(look));	
	skipWhite();
	cout << "s1: " << s1 << endl;	// els
	ifex2=cx;
	gen(OP_JMP, 0, 0);
	ifex3=cx;
	// LCurl_RCurl block for compound else
	if (isLCurl(look)) {
		dolook();
		while (!isRCurl(look)) {
			statement();
			dolook();  // semicolon
		}
		dolook();	// RCurl
	}	
	else		// one statement only
		statement();
	code[ifex1].operand2=ifex3;
	code[ifex2].operand2=cx;	
}
void ifz_stmnt() {
	// just about if only 
	paren_cond();
 	iz1 = cx;
	gen(OP_JPC, 0, 0);
	if (isLCurl(look)) {
		dolook();
		while (!isRCurl(look)) {
			statement();
			dolook();
		}
		dolook();	// rm RCurl
	}	
	else
		statement();
	code[iz1].operand2=cx;
}
void forstmnt() {
	if (isLParen(look))
		dolook();
	else
		pl_error(43);
	statement();		//  i to table, lit 0 sto 0
	if (isSemicolon(look))
		dolook();			// ';'
	fx1 = cx;			// keep the position of cx for backpatch 
	condition();		// lod 0 lit 3  op_opr lss
	if (isSemicolon(look))
		dolook();			// ';'
	fx2=cx;
	gen(OP_JPC, 0, 0);	// goto end if fails fall thru if true 
	fx3=cx;
	gen(OP_JMP, 0, 0);	// goto stmnt3 which is write(i)
	fx4=cx;
	statement();		// stmnt2 a=a+1
	gen(OP_JMP, 0, fx1);	// goto cmp portion
	if (isRParen(look))
		dolook();
	else
		pl_error(22);
	// stmnt3 body block
	fx5=cx;
	if (isLCurl(look)) {
		dolook();
		while (!isRCurl(look)) {
			statement();
			dolook();
		}
	}	
	else
		statement();
	// dolook(); // ';'
	// after do something in stmnt then increment a by 1
	// jump to that place which is cx4
	gen(OP_JMP, 0, fx4);
	// back patch jpc and jmp, to tale and to stmnt3
	code[fx2].operand2 = cx;
	code[fx3].operand2 = fx5;
}
void whilestmnt() {
	if (isLParen(look))
		dolook();
	else
		pl_error(43);
	wx1 = cx;
	condition();
	if (isRParen(look))	
		dolook();
	else
		pl_error(22);
	wx2 = cx;
	gen(OP_JPC, 0, 0);
	if (isLCurl(look)) {
		dolook();
		while (!isRCurl(look)) {
			statement();
			dolook();
		}
	}	
	else
		statement();
	gen(OP_JMP, 0, wx1);
	code[wx2].operand2 = cx;
}
void proc_stmnt() {
	// next a name and should be a new one
	int n1;
	string s1;
	if( isAlpha(look) ) {
		do {
			s1 += look;
			dolook_s();
		} while (isAlNum(look)) ;
	}
	skipWhite();
	procFlag=true;
	n1=isin_tab(s1);
	if (n1!=-1) 
		pl_error(59);
	else {
		set_nametab(s1);
		set_tabkind(s1, PRO);
	}
	px1=cx;
	gen(OP_JMP, 0, 0);
	// LCurl_RCurl block for compound statement
	if (isLCurl(look)) {
		dolook();
		while (!isRCurl(look)) {
			statement();
			dolook();  // semicolon
		}
	}	
	else	 
		statement();
	gen(OP_OPR, 0, OPR_RET);
	code[px1].operand2=cx;
	// number you back patch to top jmp is a size of opcodes
	set_pdispmap(s1, px1);
}
void proc_callstmnt() {
	// in case put parens included on call pr1();
	if (isLParen(look)) 
		skipParens();
	int n1, n2;
	n1=get_tabadr(gstr);
	// first one is jmp so, n1+1
	// if this call is to a 2nd proc a patch needed 
	n2=get_pdispmap(gstr);
	if (n1==0) // top one
		gen(OP_CAL, 0, n1+1);
	else 
	 	gen(OP_CAL, 0, n2+1);
	no_stoFlag=true;	
}
void inc_stmnt() {
	gen(OP_LOD, INT, incAdr);
	gen(OP_OPR, INT, OPR_INC);
	if (!factopFlag)
		gen(OP_STO, INT, incAdr);
	wrtinfo=INT;
 	incFlag=false;
	factopFlag=false;
}
void dec_stmnt() {
	gen(OP_LOD, INT, decAdr);
	gen(OP_OPR, INT, OPR_DEC);
	if (!factopFlag)
		gen(OP_STO, INT, decAdr);
	wrtinfo=INT;
	decFlag=false;
	factopFlag=false;
}

void statement() {
	int n2, n3;
	int address;
	if (incFlag) {
	//	cout << "incFlag is On: " << endl;
		inc_stmnt();
		goto BOTTOM;
	}
	if (decFlag) {
	//	cout << "decFlag is On: " << endl;
		dec_stmnt();
		goto BOTTOM;
	}
	
	
	gstr.clear();
	if (isAlpha(look)) {
		do {
			gstr += look;
			dolook_s();
		} while (isAlNum(look) );
	}
	skipWhite();
	n2=get_tabkind(gstr);
 	if (n2==PRO) {
		proc_callstmnt();
		goto BYPASS;
 	}
	else if ( gstr == "for" ) {
		forFlag=true;
		forstmnt();
		forFlag=false;
	}
	else if (gstr == "ife")
		ife_stmnt();
	else if (gstr == "ifz")
		ifz_stmnt();
	else if ( gstr == "procedure")
		proc_stmnt();
	else if ( gstr == "while")
		whilestmnt();
	else if ( gstr=="write" ) {
    	paren_expre();
		gen(OP_WRT, wrtinfo, 0);
	}
	else {
		// check in name table
		n3=isin_tab(gstr);
		if (n3 == -1) {	// the new name
			address	= adr;
			set_nametab(gstr);
		}
		else
			address	= n3;
		expression();
		
		if ( look != ';' && forFlag==false)
			pl_error(34);
BYPASS:	
		if (no_stoFlag)
			no_stoFlag=false;
		else if (aryFlag )
			aryFlag=false;
		else {
			int a1=isin_tab(gstr);
			if (a1 != -1)
				gen(OP_STO, wrtinfo, a1);
			else 
				pl_error(65);
		}
BOTTOM:
	cout << "";
	}
}

void interp()
{
	//	cout << "size of instructions: " << codeIndex << endl;
	cout << "Start pl_micro: \n";
	Stack<float> stack(MAX_CODE_SIZE);
	Stack<int> logicstack(16);
	string mystr, mystr2, mystr3;
	int opcode1, operand1, operand2;
	int num1=0, num2=0, num3=0;
	int ad1, ad2;
	int ok;
	int pc = 0;
	float fad1, fad2;
 	float fnum1, fnum2;
	bool retFlag=false;
	int multi_size=multi_kindvec.size();
	int mcnt=0;
	
	do {
		/* code[i].id holds a number representing opcode kind */
		opcode1 = code[pc].opcode;
		operand1 = code[pc].operand1;
		operand2 = code[pc].operand2;

		switch(opcode1) {
			case OP_LIT:	
				if (operand1==FLT) {
					fnum2=code[pc].operand2;
					ok = stack.push(fnum2);
					if (!ok)	
						pl_error(1);
				}
				else {
					ok = stack.push(operand2);
					if (!ok)	
						pl_error(1);
				}
				pc++;
				break;
			case OP_OPR:	
				switch(operand2) {
					case OPR_RET: {
						if (retFlag) {
							pc=reg2;
							retFlag=false;
						}
						else {
							pc = 0;
						}
						break;
					}
					case OPR_ADD:
						ok = stack.pop(fnum2);	// get a last input one
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						ok = stack.push(fnum1+fnum2);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_SUB:		
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						ok = stack.push(fnum1-fnum2);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_MUL:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						ok = stack.push(fnum1*fnum2);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_DIV:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						ok = stack.push(fnum1/fnum2);
						if (!ok)	
							pl_error(1);
						pc++;
						break;	
					case OPR_EQL:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						num3 = (fnum1 == fnum2);
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						ok = logicstack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_NEQ:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						num3 = (fnum1 != fnum2);
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						ok = logicstack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_LSS:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						num3 = (fnum1 < fnum2);
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						ok = logicstack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_LEQ:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						num3 = (fnum1 <= fnum2);
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						ok = logicstack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_GTR:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						num3 = (fnum1 > fnum2);
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						ok = logicstack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_GEQ:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						num3 = (fnum1 >= fnum2);
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						ok = logicstack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_NOT:
						ok = stack.pop(fnum1);
						if (!ok)	
							pl_error(2);
						if (fnum1) 
							num1=0;
						else
							num1=1;
						ok = stack.push(num1);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_AND:
						ok=logicstack.pop(num2);
						if (!ok)	
							pl_error(2);
						ok=logicstack.pop(num1);
						if (!ok)	
							pl_error(2);
						if (num2==1 && num1==1)
							num3 = 1;
						else 
							num3=0;
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_OR:
						ok=logicstack.pop(num2);
						if (!ok)	
							pl_error(2);
						ok=logicstack.pop(num1);
						if (!ok)	
							pl_error(2);
						if (num2==0 && num1==0)
							num3 = 0;
						else 
							num3=1;
						ok = stack.push(num3);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_INC:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						fnum2++;
						ok = stack.push(fnum2);
						if (!ok)	
							pl_error(1);
						pc++;
						
						break;
					case OPR_DEC:
						ok = stack.pop(fnum2);
						if (!ok)	
							pl_error(2);
						fnum2--;
						ok = stack.push(fnum2);
						if (!ok)	
							pl_error(1);
						pc++;
						break;
					case OPR_LID:
						ok = stack.pop(fad1);
						if (!ok)	
							pl_error(2);
						num1=int(fad1);
						if (operand1==ARYINT) {
							num2=table[num1].ints;
							ok = stack.push(num2);   
							if (!ok)	
								pl_error(1);
						}
						else if (operand1==ARYFLT) {
							fnum1=table[num1].flt;
							ok = stack.push(fnum1);
							if (!ok)	
								pl_error(1);
						}
						else {
							// push the adr again
							ok = stack.push(fad1);   
							if (!ok)	
								pl_error(1);
						}
						pc++;
						break;
						
					 case OPR_SID:    
						// lda 300 lit 2 add lit 123 opr sid
						ok = stack.pop(fnum1);  // value may be adr as well
						if (!ok)	
							pl_error(2);
						ok = stack.pop(fad2); // fad2 is lda+offset 
						if (!ok)	
							pl_error(2);
						ad1=int(fnum1);		// imm adr or int value 123
						ad2=int(fad2);
						// flt value will not go direct
						if (operand1==ARYFLT) { // 9
							fnum2=table[ad1].flt;
							table[ad2].flt = fnum2;
						}
						else if (operand1==ARYINT)
							table[ad2].ints = ad1;
						else 
							table[ad2].str = table[ad1].str;
						pc++;
						break;
						
					default:
						cout << "Not Implemented yet, or may be wrong thing happens" << endl;
						pc++;
				}
				break;
				
			case OP_LOD:	
				if (operand1==STR ) {
					ok = stack.push(operand2);
					if (!ok)	
						pl_error(1);
				}
				else if ( operand1==ARYSTR ) {
					ok = stack.push(operand2);
					if (!ok)	
						pl_error(1);
				}
				else if (operand1==FLT) {
					fnum1=table[operand2].flt;
					ok = stack.push(fnum1);
					if (!ok)	
						pl_error(1);
				}
				else if (operand1==ARYFLT) {
					fnum1=table[operand2].flt;
					ok = stack.push(fnum1);
					if (!ok)	
						pl_error(1);
				}
				else if (operand1==ARYINT) {
					num1=table[operand2].ints;
					ok = stack.push(num1);
					if (!ok)	
						pl_error(1);
				}
				else {
					num1 = table[operand2].ints;
					ok = stack.push(num1);
					if (!ok)	
						pl_error(1);
				}
				pc++;
				break;
			case OP_LDA:	
				ok = stack.push(operand2);
				if (!ok)	
					pl_error(1);
				pc++;
				break;
			case OP_IMM:
				// offset starts from 200 + n
				ok = stack.push(operand2);
				if (!ok)	
					pl_error(1);
				pc++;
				break;
			case OP_STO:	
				ok = stack.pop(fnum2);
				if (!ok)	
					pl_error(2);
				num2=int(fnum2);
				if (operand1==INT || operand1==FLT ) {
					table[operand2].ints=num2;
					table[operand2].flt=fnum2;
				}
				else if (operand1==STR )
					table[operand2].str= table[num2].str;
				else if (operand1==ARYSTR)
					table[operand2].str= table[num2].str;
				else if (operand1==ARYFLT)
					table[operand2].flt= fnum2;
				else if (operand1==ARYINT)
					table[operand2].ints= fnum2;
				else if(operand1==IMMSTR)
					table[operand2].str= table[num2].str;
				else if(operand1==IMMFLT)
					table[operand2].flt= table[num2].flt;
				else if(operand1==IMMCHA)
					strcpy(table[operand2].chars, table[num2].chars);
				else {
					table[operand2].ints=fnum2;
					cout << "Warning! no kind info: " << operand2 << " operand1: " << operand1 << endl;
				}
				pc++;
				break;
			case OP_JMP: {
				pc = operand2;
				break;
			}
			case OP_JPC: {// if stack has 1 then go to next
				/* if 0 then skip these statement */ 
				ok = stack.pop(fnum1);
				if (!ok)	
					pl_error(2);
				num1= int(fnum1);
				if ( num1 == 0 ) {
					pc = operand2; // jump to do else
				}
				else
					pc++;
				break;
			}
			case OP_CAL:
				reg2=pc+1;
				retFlag=true;
				pc=operand2;	// jump to procedure top after jmp
				break;
			case OP_WRT:
				ok = stack.pop(fnum2);	// get a last input one
				if (!ok)	
					pl_error(2);
				num2=int(fnum2);
		//		cout << "operand1 wrt: " << operand1 << endl;
		//		cout << "stack popped wrt: " << fnum2 << endl;
				if (operand1==MULTI) {
					if (mcnt<multi_size)
						num1=multi_kindvec[mcnt++];
					if (num1==ARYINT)
						cout << "wrt aryint: " << table[num2].ints << endl;
					else if (num1==ARYFLT)
						cout << "wrt aryflt: " << table[num2].flt << endl;
					else if (num1==ARYSTR)
						cout << "wrt arystr: " << table[num2].str << endl;
					else 
						cout << "wrong thing happened: fix it!!!" << endl;
				}
				else if (operand1==IMMSTR) 
					cout << "wrt immstr: " << table[num2].str << endl;
				else if (operand1==IMMFLT) 
					cout << "wrt immflt: " << table[num2].flt << endl;
				else if (operand1==IMMCHA) 
					cout << "wrt immcha: " << table[num2].chars << endl;
				
				else if (operand1==ARYINT)
					cout << "wrt aryint: " << num2 << endl;
				else if (operand1==ARYFLT)
					cout << "wrt aryflt: " << fnum2 << endl;
				else if (operand1==ARYSTR)
					cout << "wrt arystr: " << table[num2].str << endl;
				
				else if ( operand1==INT )
					cout << "wrt int: " << num2 << endl;
				else if ( operand1==CHA  )
					cout << "wrt cha: " << table[num2].chars << endl;
				else if ( operand1==FLT )
					cout << "wrt flt: " << fnum2 << endl;
				else if (operand1==STR )
					cout << "wrt str: " << table[num2].str << endl;
				else 
					cout << "wrt fnum2: " << fnum2 << endl;
				ok = stack.push(fnum2);  // push it back 
				if (!ok)	
					pl_error(1);
				pc++;
				break;
				
			default:
				cout << "wrong thing happened in interp switch: " << pc << '\n';
				pc++;
		}
	} while ( pc != 0 );
	cout << "End pl_micro: \n";
}


int main(int argc, char* argv[]) {
	keyset_ini();
//	cout << "> ";
//	getline(cin, str1, '$');
//	str1 += '$';
	ifstream ifs(argv[1]);
	if(!ifs)
	{
		cerr << "Cannot open file " << "\n";
		return EXIT_FAILURE;
	}
	string line;
	while(getline(ifs,line))
 		str1 += line;
	str1 += '$';
    cout << str1 << endl;
	dolook();
	while (!isDollar(look)) {
		char ch;
		ch = dopeek();
		if ( isSlash(look) && isStar(ch) ) {
			do {
				dolook();
			} while (!isSlash(look));
			dolook();	// slash
		}		
		if ( isAlpha(look) )
			statement();
		else
			dolook();
	} 
	/* just adding ret thing to the tale of code[] to stop */
	gen(OP_OPR, 0, OPR_RET);	
	/* print code[] by using op_name[] opr_name */
	/* size of instruction is cx */
	cout << endl;
	for (int i=0; i < cx; i++) {
		if ( code[i].opcode == OP_OPR )
			cout << i << ": " << op_name[code[i].opcode] <<
			'\t' << opr_name[code[i].operand2] << endl;
		else
			cout << i << ": " << op_name[code[i].opcode] <<
			 '\t' << code[i].operand2 << endl;
		//'\t' << code[i].operand1 <<
	}
	
		interp();
}

