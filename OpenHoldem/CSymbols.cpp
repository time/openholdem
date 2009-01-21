#include "stdafx.h"
#include <process.h>
#include <float.h>

#include "CScraper.h"
#include "CSymbols.h"
#include "CIteratorThread.h"
#include "CAutoplayer.h"
#include "CPokerTrackerThread.h"
#include "CPreferences.h"
#include "CGameState.h"
#include "..\CTablemap\CTablemap.h"
#include "..\CTransform\CTransform.h"
#include "CGrammar.h"
#include "CVersus.h"
#include "CRunRon.h"
#include "CPokerPro.h"
#include "CGameState.h"

#include "OpenHoldem.h"
#include "MainFrm.h"

#include "inlines/eval.h"

CSymbols			*p_symbols = NULL;
CRITICAL_SECTION	CSymbols::cs_symbols;

// These can not be function scoped statics because we need to be able to
// reset them when we connect to a table.
double CSymbols::_dealerchair_last = -1;
double CSymbols::_handnumber_last = -1;
int CSymbols::_br_last = -1;
unsigned int CSymbols::_player_card_last[2] = {CARD_NOCARD, CARD_NOCARD};


char handrank169[10][169][4] =
{
	{"AAo", "KKo", "QQo", "JJo", "TTo", "99o", "88o", "AKs", "77o", "AQs", "AKo", "AJs", "AQo", "ATs", "66o", "AJo", "KQs", "ATo", "A9s", "KJs", "A8s", "KTs", "KQo", "55o", "A7s", "A9o", "KJo", "QJs", "K9s", "KTo", "A8o", "A6s", "QTs", "A5s", "A4s", "A7o", "QJo", "K8s", "A3s", "K9o", "44o", "Q9s", "JTs", "QTo", "A6o", "K7s", "A5o", "A2s", "K6s", "A4o", "K8o", "Q8s", "J9s", "A3o", "K5s", "Q9o", "JTo", "K7o", "A2o", "K4s", "33o", "Q7s", "K6o", "T9s", "J8s", "K3s", "Q8o", "Q6s", "J9o", "K5o", "K2s", "Q5s", "T8s", "J7s", "K4o", "Q7o", "T9o", "Q4s", "J8o", "K3o", "22o", "Q6o", "Q3s", "98s", "T7s", "J6s", "K2o", "Q2s", "Q5o", "J5s", "T8o", "J7o", "Q4o", "97s", "J4s", "T6s", "J3s", "Q3o", "98o", "T7o", "J6o", "87s", "J2s", "96s", "Q2o", "J5o", "T5s", "T4s", "97o", "J4o", "T6o", "86s", "95s", "T3s", "J3o", "76s", "87o", "T2s", "96o", "J2o", "85s", "T5o", "94s", "T4o", "75s", "93s", "86o", "65s", "95o", "T3o", "84s", "92s", "76o", "T2o", "74s", "85o", "54s", "64s", "83s", "94o", "75o", "82s", "93o", "73s", "65o", "53s", "63s", "84o", "92o", "43s", "74o", "72s", "54o", "64o", "52s", "62s", "83o", "82o", "42s", "73o", "53o", "63o", "32s", "43o", "72o", "52o", "62o", "42o", "32o"},
	{"AAo", "KKo", "QQo", "JJo", "TTo", "99o", "AKs", "88o", "AQs", "AKo", "AJs", "77o", "KQs", "ATs", "AQo", "KJs", "AJo", "KTs", "KQo", "A9s", "QJs", "ATo", "66o", "A8s", "KJo", "QTs", "K9s", "JTs", "A7s", "KTo", "QJo", "A9o", "55o", "A5s", "A6s", "Q9s", "QTo", "A8o", "K8s", "A4s", "J9s", "K9o", "A3s", "JTo", "K7s", "A7o", "T9s", "Q8s", "A2s", "K6s", "44o", "Q9o", "A5o", "A6o", "J8s", "K5s", "K8o", "T8s", "A4o", "J9o", "Q7s", "K4s", "98s", "A3o", "K7o", "T9o", "Q6s", "K3s", "J7s", "Q8o", "A2o", "33o", "K6o", "Q5s", "K2s", "T7s", "J8o", "97s", "87s", "Q4s", "K5o", "T8o", "J6s", "Q3s", "Q7o", "98o", "K4o", "T6s", "J5s", "Q2s", "96s", "Q6o", "76s", "86s", "J7o", "22o", "K3o", "J4s", "T7o", "Q5o", "J3s", "K2o", "T5s", "97o", "87o", "65s", "J2s", "95s", "75s", "Q4o", "T4s", "85s", "J6o", "T3s", "Q3o", "54s", "T6o", "J5o", "T2s", "96o", "64s", "86o", "76o", "Q2o", "94s", "74s", "84s", "J4o", "93s", "53s", "J3o", "92s", "T5o", "63s", "65o", "73s", "43s", "95o", "75o", "83s", "J2o", "85o", "T4o", "82s", "52s", "T3o", "54o", "62s", "42s", "64o", "72s", "T2o", "74o", "94o", "84o", "32s", "93o", "53o", "92o", "63o", "43o", "73o", "83o", "82o", "52o", "62o", "42o", "72o", "32o"},
	{"AAo", "KKo", "QQo", "JJo", "TTo", "99o", "AKs", "AQs", "AKo", "AJs", "KQs", "88o", "ATs", "AQo", "KJs", "QJs", "KTs", "KQo", "AJo", "77o", "QTs", "A9s", "ATo", "JTs", "KJo", "A8s", "K9s", "QJo", "66o", "KTo", "A7s", "Q9s", "QTo", "J9s", "A5s", "T9s", "A9o", "A6s", "JTo", "K8s", "A4s", "55o", "A3s", "K7s", "Q8s", "A8o", "K9o", "J8s", "A2s", "T8s", "K6s", "98s", "Q9o", "A7o", "K5s", "J9o", "T9o", "Q7s", "A5o", "44o", "K4s", "A6o", "K8o", "J7s", "Q6s", "T7s", "97s", "87s", "K3s", "A4o", "Q5s", "K2s", "Q8o", "K7o", "A3o", "J8o", "Q4s", "T8o", "J6s", "76s", "A2o", "98o", "33o", "K6o", "T6s", "86s", "96s", "Q3s", "J5s", "Q2s", "K5o", "J4s", "65s", "Q7o", "75s", "J7o", "J3s", "85s", "K4o", "T7o", "T5s", "95s", "97o", "87o", "22o", "Q6o", "J2s", "54s", "T4s", "K3o", "64s", "Q5o", "T3s", "K2o", "74s", "84s", "T2s", "76o", "Q4o", "94s", "53s", "J6o", "T6o", "86o", "96o", "93s", "Q3o", "63s", "43s", "J5o", "92s", "73s", "Q2o", "65o", "83s", "J4o", "52s", "75o", "82s", "85o", "42s", "J3o", "T5o", "62s", "95o", "54o", "72s", "T4o", "J2o", "32s", "64o", "T3o", "74o", "84o", "T2o", "53o", "94o", "93o", "63o", "43o", "92o", "73o", "83o", "52o", "82o", "42o", "62o", "72o", "32o"},
	{"AAo", "KKo", "QQo", "JJo", "TTo", "AKs", "AQs", "99o", "KQs", "AKo", "AJs", "KJs", "ATs", "AQo", "88o", "QJs", "KTs", "KQo", "QTs", "AJo", "JTs", "A9s", "KJo", "77o", "ATo", "K9s", "A8s", "QJo", "Q9s", "KTo", "A7s", "J9s", "T9s", "QTo", "A5s", "66o", "JTo", "A6s", "K8s", "A4s", "A9o", "A3s", "Q8s", "K7s", "J8s", "T8s", "A2s", "98s", "K9o", "55o", "K6s", "A8o", "Q9o", "K5s", "T9o", "Q7s", "J9o", "K4s", "T7s", "J7s", "87s", "A7o", "97s", "Q6s", "K3s", "44o", "A5o", "K8o", "K2s", "Q5s", "A6o", "76s", "A4o", "86s", "Q4s", "Q8o", "J6s", "T8o", "T6s", "96s", "J8o", "A3o", "K7o", "Q3s", "98o", "33o", "J5s", "65s", "Q2s", "A2o", "75s", "J4s", "K6o", "85s", "54s", "J3s", "22o", "T5s", "95s", "J2s", "64s", "K5o", "Q7o", "87o", "T4s", "T7o", "J7o", "97o", "74s", "K4o", "T3s", "53s", "Q6o", "84s", "T2s", "K3o", "94s", "43s", "63s", "76o", "Q5o", "93s", "K2o", "86o", "92s", "73s", "96o", "52s", "Q4o", "J6o", "T6o", "83s", "42s", "65o", "82s", "Q3o", "J5o", "62s", "75o", "32s", "Q2o", "72s", "J4o", "54o", "85o", "95o", "T5o", "J3o", "64o", "J2o", "T4o", "74o", "53o", "T3o", "84o", "T2o", "94o", "43o", "63o", "93o", "73o", "92o", "52o", "83o", "42o", "82o", "62o", "32o", "72o"},
	{"AAo", "KKo", "QQo", "JJo", "AKs", "TTo", "AQs", "KQs", "AKo", "AJs", "99o", "KJs", "ATs", "QJs", "AQo", "KTs", "KQo", "QTs", "88o", "JTs", "AJo", "A9s", "KJo", "K9s", "A8s", "QJo", "ATo", "77o", "Q9s", "T9s", "J9s", "KTo", "A7s", "A5s", "QTo", "JTo", "K8s", "A4s", "A6s", "66o", "A3s", "Q8s", "T8s", "K7s", "J8s", "98s", "A2s", "A9o", "K6s", "K9o", "55o", "K5s", "87s", "Q7s", "A8o", "Q9o", "T7s", "97s", "K4s", "T9o", "J7s", "J9o", "K3s", "Q6s", "44o", "K2s", "76s", "Q5s", "A7o", "86s", "A5o", "Q4s", "96s", "T6s", "33o", "K8o", "J6s", "65s", "Q3s", "A6o", "A4o", "T8o", "J5s", "Q8o", "Q2s", "75s", "J8o", "98o", "54s", "22o", "A3o", "K7o", "J4s", "85s", "J3s", "64s", "95s", "A2o", "T5s", "J2s", "K6o", "T4s", "53s", "74s", "87o", "T3s", "97o", "43s", "T7o", "T2s", "84s", "K5o", "Q7o", "63s", "J7o", "94s", "K4o", "93s", "52s", "73s", "Q6o", "76o", "92s", "K3o", "42s", "86o", "83s", "K2o", "Q5o", "82s", "62s", "32s", "96o", "65o", "T6o", "Q4o", "J6o", "72s", "75o", "Q3o", "54o", "J5o", "Q2o", "85o", "J4o", "64o", "95o", "J3o", "T5o", "53o", "J2o", "74o", "T4o", "43o", "T3o", "84o", "63o", "T2o", "94o", "52o", "93o", "73o", "42o", "92o", "83o", "62o", "32o", "82o", "72o"},
	{"AAo", "KKo", "QQo", "JJo", "AKs", "AQs", "TTo", "KQs", "AJs", "AKo", "KJs", "ATs", "QJs", "99o", "KTs", "AQo", "QTs", "JTs", "KQo", "88o", "A9s", "AJo", "K9s", "KJo", "A8s", "Q9s", "QJo", "T9s", "J9s", "ATo", "77o", "A7s", "KTo", "A5s", "A4s", "QTo", "K8s", "A6s", "JTo", "A3s", "66o", "T8s", "Q8s", "A2s", "J8s", "K7s", "98s", "K6s", "55o", "A9o", "87s", "K5s", "97s", "Q7s", "K4s", "T7s", "K9o", "J7s", "K3s", "T9o", "44o", "Q6s", "Q9o", "J9o", "K2s", "76s", "A8o", "86s", "Q5s", "33o", "65s", "Q4s", "96s", "22o", "T6s", "Q3s", "A7o", "J6s", "75s", "54s", "Q2s", "A5o", "J5s", "K8o", "85s", "64s", "A4o", "T8o", "J4s", "A6o", "98o", "Q8o", "J8o", "J3s", "53s", "A3o", "95s", "J2s", "T5s", "74s", "K7o", "T4s", "43s", "A2o", "T3s", "84s", "63s", "T2s", "87o", "K6o", "52s", "94s", "97o", "93s", "73s", "42s", "T7o", "K5o", "92s", "Q7o", "J7o", "32s", "76o", "83s", "62s", "K4o", "82s", "86o", "Q6o", "K3o", "72s", "K2o", "65o", "96o", "Q5o", "54o", "T6o", "75o", "Q4o", "J6o", "Q3o", "Q2o", "85o", "64o", "J5o", "53o", "J4o", "95o", "J3o", "74o", "T5o", "43o", "J2o", "T4o", "63o", "T3o", "84o", "52o", "T2o", "94o", "42o", "73o", "93o", "92o", "32o", "83o", "62o", "82o", "72o"},
	{"AAo", "KKo", "QQo", "JJo", "AKs", "AQs", "KQs", "TTo", "AJs", "AKo", "KJs", "ATs", "QJs", "KTs", "99o", "QTs", "AQo", "JTs", "KQo", "A9s", "88o", "AJo", "K9s", "A8s", "T9s", "KJo", "Q9s", "J9s", "QJo", "77o", "A7s", "A5s", "ATo", "A4s", "KTo", "A6s", "K8s", "A3s", "JTo", "QTo", "66o", "T8s", "A2s", "Q8s", "98s", "J8s", "K7s", "K6s", "55o", "87s", "K5s", "97s", "44o", "K4s", "T7s", "Q7s", "J7s", "K3s", "A9o", "76s", "33o", "K2s", "Q6s", "22o", "K9o", "86s", "T9o", "65s", "Q5s", "J9o", "Q9o", "96s", "54s", "Q4s", "A8o", "75s", "T6s", "Q3s", "J6s", "Q2s", "64s", "85s", "J5s", "53s", "A7o", "J4s", "A5o", "J3s", "95s", "74s", "T8o", "J2s", "43s", "A4o", "K8o", "T5s", "98o", "J8o", "T4s", "63s", "A6o", "A3o", "Q8o", "T3s", "84s", "T2s", "52s", "A2o", "K7o", "42s", "87o", "94s", "73s", "93s", "92s", "32s", "97o", "K6o", "62s", "83s", "T7o", "82s", "76o", "K5o", "J7o", "72s", "Q7o", "K4o", "86o", "65o", "K3o", "Q6o", "K2o", "54o", "96o", "75o", "Q5o", "T6o", "Q4o", "64o", "J6o", "Q3o", "85o", "53o", "Q2o", "J5o", "74o", "43o", "J4o", "95o", "J3o", "J2o", "63o", "T5o", "52o", "T4o", "84o", "T3o", "42o", "T2o", "73o", "94o", "32o", "93o", "62o", "92o", "83o", "82o", "72o"},
	{"AAo", "KKo", "QQo", "AKs", "JJo", "AQs", "KQs", "AJs", "TTo", "AKo", "KJs", "ATs", "QJs", "KTs", "QTs", "99o", "JTs", "AQo", "KQo", "A9s", "88o", "K9s", "T9s", "AJo", "A8s", "J9s", "Q9s", "KJo", "77o", "A5s", "A7s", "QJo", "A4s", "A3s", "A6s", "ATo", "K8s", "66o", "A2s", "T8s", "KTo", "98s", "Q8s", "J8s", "JTo", "QTo", "K7s", "55o", "87s", "K6s", "44o", "97s", "33o", "22o", "K5s", "T7s", "K4s", "76s", "Q7s", "K3s", "J7s", "K2s", "86s", "65s", "Q6s", "54s", "A9o", "Q5s", "T9o", "96s", "75s", "Q4s", "K9o", "J9o", "Q3s", "64s", "T6s", "Q9o", "Q2s", "J6s", "85s", "53s", "A8o", "J5s", "J4s", "74s", "43s", "J3s", "95s", "J2s", "63s", "T5s", "A5o", "A7o", "T8o", "T4s", "98o", "T3s", "84s", "52s", "A4o", "T2s", "42s", "K8o", "A3o", "J8o", "A6o", "73s", "Q8o", "94s", "32s", "93s", "A2o", "87o", "92s", "62s", "K7o", "83s", "97o", "82s", "76o", "72s", "K6o", "T7o", "65o", "K5o", "86o", "J7o", "54o", "Q7o", "K4o", "K3o", "75o", "K2o", "Q6o", "96o", "64o", "Q5o", "T6o", "53o", "Q4o", "85o", "J6o", "Q3o", "Q2o", "43o", "74o", "J5o", "J4o", "95o", "63o", "J3o", "J2o", "52o", "T5o", "84o", "42o", "T4o", "T3o", "32o", "T2o", "73o", "94o", "62o", "93o", "92o", "83o", "82o", "72o"},
	{"AAo", "KKo", "QQo", "AKs", "JJo", "AQs", "KQs", "AJs", "TTo", "KJs", "AKo", "ATs", "QJs", "KTs", "QTs", "JTs", "99o", "AQo", "A9s", "KQo", "88o", "K9s", "T9s", "A8s", "J9s", "Q9s", "77o", "AJo", "A5s", "A7s", "A4s", "KJo", "A3s", "66o", "A6s", "QJo", "K8s", "A2s", "T8s", "98s", "J8s", "ATo", "Q8s", "55o", "K7s", "JTo", "KTo", "44o", "33o", "22o", "QTo", "87s", "K6s", "97s", "K5s", "76s", "T7s", "K4s", "K3s", "Q7s", "K2s", "J7s", "86s", "65s", "54s", "Q6s", "75s", "Q5s", "96s", "Q4s", "Q3s", "64s", "T9o", "Q2s", "A9o", "T6s", "53s", "J6s", "85s", "J9o", "K9o", "43s", "J5s", "Q9o", "74s", "J4s", "J3s", "J2s", "95s", "63s", "A8o", "T5s", "52s", "42s", "T4s", "T3s", "84s", "98o", "T2s", "A5o", "T8o", "A7o", "73s", "32s", "A4o", "94s", "93s", "62s", "A3o", "K8o", "J8o", "92s", "A6o", "87o", "Q8o", "83s", "A2o", "82s", "97o", "72s", "K7o", "76o", "T7o", "65o", "K6o", "86o", "54o", "K5o", "J7o", "Q7o", "75o", "K4o", "K3o", "96o", "64o", "K2o", "53o", "Q6o", "85o", "T6o", "Q5o", "43o", "Q4o", "Q3o", "74o", "Q2o", "J6o", "63o", "J5o", "95o", "52o", "J4o", "42o", "J3o", "J2o", "84o", "T5o", "32o", "T4o", "T3o", "73o", "T2o", "62o", "94o", "93o", "92o", "83o", "82o", "72o"}
};

//handrank table used to prime weighted prwin lookup table.
//reflects incidence of cards people actually play to flop.
//left in this form for ease of developer modification.
//converted at startup to the tables actually used by prwin calculation
char *prwhandrank169[169] =
{
	"AA ","KK ","QQ ","AKs","JJ ","AQs","KQs","TT ","AJs","KJs",
	"JTs","QJs","QTs","99 ","ATs","KTs","88 ","T9s","AK ","J9s",
	"77 ","98s","Q9s","66 ","A9s","K9s","T8s","55 ","A5s","54s",
	"44 ","A8s","87s","33 ","65s","22 ","AQ ","A4s","J8s","A3s",
	"76s","97s","A7s","KQ ","A2s","Q8s","86s","K8s","A6s","75s",
	"T7s","53s","64s","K7s","AJ ","KJ ","43s","QJ ","96s","JT ",
	"J7s","K6s","K5s","QT ","85s","Q7s","KT ","AT ","K4s","K3s",
	"K2s","74s","T6s","52s","Q6s","63s","Q5s","Q4s","42s","Q3s",
	"95s","J6s","J5s","32s","Q2s","T9 ","J4s","84s","T5s","J3s",
	"T4s","73s","J2s","J9 ","T3s","T2s","62s","94s","93s","92s",
	"83s","Q9 ","A9 ","98 ","82s","T8 ","K9 ","72s","54 ","87 ",
	"A8 ","A5 ","65 ","A4 ","76 ","J8 ","97 ","A3 ","A7 ","A2 ",
	"86 ","A6 ","Q8 ","75 ","53 ","K8 ","T7 ","64 ","K7 ","43 ",
	"96 ","K6 ","J7 ","85 ","K5 ","Q7 ","K4 ","K3 ","74 ","52 ",
	"T6 ","Q6 ","K2 ","Q5 ","42 ","63 ","Q4 ","95 ","Q3 ","J6 ",
	"J5 ","32 ","Q2 ","J4 ","T5 ","84 ","J3 ","J2 ","T4 ","73 ",
	"T3 ","T2 ","94 ","62 ","93 ","92 ","83 ","82 ","72 "
};
//weighted prwin lookup tables for non-suited and suited cards
int pair2ranko[170] = {0}, pair2ranks[170] = {0};
//used to resolve ascii cards to numbers for init of above
char ctonum[14]="23456789TJQKA";
int willplay = 0, wontplay = 0, topclip = 0, mustplay = 0;

int handrank2652[10][169] =
{
	{12, 24, 36, 48, 60, 72, 84, 92, 104, 112, 136, 144, 168, 176, 188, 212, 220, 244, 252, 260, 268, 276, 300, 312, 320, 344, 368, 376, 384, 408, 432, 440, 448, 456, 464, 488, 512, 520, 528, 552, 564, 572, 580, 604, 628, 636, 660, 668, 676, 700, 724, 732, 740, 764, 772, 796, 820, 844, 868, 876, 888, 896, 920, 928, 936, 944, 968, 976, 1000, 1024, 1032, 1040, 1048, 1056, 1080, 1104, 1128, 1136, 1160, 1184, 1196, 1220, 1228, 1236, 1244, 1252, 1276, 1284, 1308, 1316, 1340, 1364, 1388, 1396, 1404, 1412, 1420, 1444, 1468, 1492, 1516, 1524, 1532, 1540, 1564, 1588, 1596, 1604, 1628, 1652, 1676, 1684, 1692, 1700, 1724, 1732, 1756, 1764, 1788, 1812, 1820, 1844, 1852, 1876, 1884, 1892, 1916, 1924, 1948, 1972, 1980, 1988, 2012, 2036, 2044, 2068, 2076, 2084, 2092, 2116, 2140, 2148, 2172, 2180, 2204, 2212, 2220, 2244, 2268, 2276, 2300, 2308, 2332, 2356, 2364, 2372, 2396, 2420, 2428, 2452, 2476, 2500, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 48, 60, 72, 80, 92, 100, 124, 132, 144, 152, 160, 184, 192, 216, 224, 248, 256, 264, 288, 300, 308, 332, 340, 348, 356, 364, 388, 412, 436, 448, 456, 464, 472, 496, 520, 528, 536, 544, 568, 576, 600, 608, 632, 640, 648, 656, 664, 676, 700, 724, 748, 756, 764, 788, 796, 820, 844, 852, 860, 868, 892, 916, 940, 948, 956, 964, 988, 1012, 1024, 1048, 1056, 1064, 1072, 1096, 1104, 1112, 1120, 1144, 1168, 1176, 1184, 1208, 1232, 1256, 1264, 1272, 1280, 1288, 1312, 1320, 1328, 1352, 1364, 1388, 1396, 1420, 1444, 1452, 1476, 1484, 1508, 1532, 1540, 1548, 1556, 1564, 1588, 1596, 1604, 1628, 1636, 1660, 1668, 1692, 1716, 1724, 1748, 1756, 1780, 1804, 1828, 1836, 1844, 1852, 1876, 1884, 1892, 1916, 1924, 1948, 1956, 1980, 1988, 1996, 2020, 2044, 2052, 2076, 2100, 2124, 2132, 2140, 2164, 2188, 2196, 2204, 2228, 2236, 2260, 2284, 2308, 2332, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 48, 60, 72, 80, 88, 112, 120, 128, 140, 148, 172, 180, 188, 196, 220, 244, 256, 264, 272, 296, 304, 328, 336, 344, 368, 380, 404, 412, 420, 444, 452, 460, 468, 492, 500, 524, 532, 540, 552, 560, 568, 576, 600, 624, 632, 640, 648, 656, 664, 688, 712, 720, 744, 768, 776, 800, 812, 820, 844, 868, 876, 884, 892, 900, 908, 916, 940, 948, 956, 980, 1004, 1028, 1052, 1060, 1084, 1092, 1100, 1124, 1148, 1160, 1184, 1192, 1200, 1208, 1216, 1224, 1232, 1256, 1264, 1272, 1296, 1304, 1328, 1336, 1344, 1368, 1392, 1400, 1408, 1432, 1456, 1468, 1492, 1500, 1508, 1516, 1540, 1548, 1572, 1580, 1604, 1612, 1620, 1628, 1652, 1676, 1684, 1692, 1716, 1740, 1764, 1788, 1796, 1820, 1828, 1836, 1860, 1868, 1876, 1900, 1924, 1932, 1956, 1964, 1988, 1996, 2020, 2028, 2052, 2076, 2084, 2108, 2132, 2140, 2164, 2188, 2196, 2220, 2244, 2268, 2292, 2316, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 48, 60, 68, 76, 88, 96, 120, 128, 136, 144, 168, 180, 188, 196, 220, 228, 252, 260, 268, 292, 304, 328, 336, 344, 368, 376, 400, 408, 416, 424, 448, 456, 468, 492, 500, 508, 516, 540, 548, 556, 564, 572, 580, 588, 596, 620, 632, 640, 664, 688, 696, 720, 728, 752, 760, 768, 776, 784, 808, 816, 824, 832, 844, 868, 892, 900, 908, 932, 940, 964, 972, 980, 1004, 1012, 1036, 1044, 1052, 1076, 1100, 1124, 1132, 1156, 1168, 1176, 1184, 1192, 1216, 1224, 1232, 1256, 1264, 1272, 1280, 1292, 1300, 1308, 1316, 1324, 1348, 1372, 1396, 1404, 1428, 1452, 1476, 1484, 1508, 1516, 1524, 1548, 1556, 1564, 1588, 1596, 1604, 1612, 1636, 1660, 1668, 1692, 1716, 1724, 1732, 1756, 1764, 1788, 1812, 1836, 1844, 1852, 1876, 1884, 1908, 1932, 1940, 1964, 1972, 1996, 2004, 2028, 2052, 2076, 2100, 2124, 2148, 2172, 2196, 2220, 2244, 2268, 2292, 2316, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 48, 56, 68, 76, 84, 108, 116, 128, 136, 144, 152, 176, 184, 208, 216, 228, 236, 260, 268, 292, 300, 308, 332, 356, 368, 376, 384, 392, 416, 424, 432, 456, 480, 488, 496, 504, 516, 524, 532, 540, 548, 556, 564, 572, 596, 604, 628, 640, 648, 656, 664, 688, 712, 720, 728, 736, 760, 768, 792, 800, 808, 820, 828, 836, 844, 868, 876, 900, 908, 916, 924, 936, 960, 968, 976, 984, 1008, 1032, 1056, 1064, 1088, 1096, 1104, 1128, 1152, 1160, 1172, 1196, 1220, 1228, 1236, 1244, 1252, 1260, 1284, 1292, 1300, 1324, 1332, 1340, 1348, 1372, 1380, 1404, 1412, 1436, 1444, 1452, 1476, 1500, 1508, 1532, 1540, 1564, 1572, 1580, 1588, 1612, 1636, 1644, 1668, 1676, 1700, 1708, 1732, 1756, 1764, 1772, 1780, 1804, 1828, 1852, 1876, 1900, 1908, 1932, 1956, 1980, 2004, 2028, 2052, 2076, 2100, 2124, 2148, 2172, 2196, 2220, 2244, 2268, 2292, 2316, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 48, 56, 64, 76, 84, 92, 116, 124, 132, 140, 152, 160, 184, 192, 200, 224, 236, 244, 268, 276, 300, 308, 316, 340, 348, 356, 380, 392, 400, 424, 432, 440, 464, 472, 480, 504, 512, 524, 532, 540, 548, 556, 564, 572, 580, 592, 616, 624, 632, 640, 648, 656, 664, 688, 696, 704, 728, 740, 748, 772, 796, 804, 812, 836, 844, 852, 864, 872, 880, 888, 900, 908, 916, 940, 948, 956, 964, 972, 996, 1004, 1028, 1036, 1044, 1068, 1092, 1100, 1124, 1148, 1172, 1196, 1204, 1212, 1236, 1244, 1252, 1260, 1268, 1292, 1300, 1308, 1332, 1340, 1348, 1356, 1364, 1388, 1412, 1420, 1428, 1452, 1460, 1468, 1476, 1500, 1524, 1532, 1556, 1580, 1588, 1612, 1620, 1628, 1652, 1660, 1684, 1708, 1732, 1740, 1764, 1788, 1812, 1836, 1860, 1884, 1908, 1932, 1956, 1980, 2004, 2028, 2052, 2076, 2100, 2124, 2148, 2172, 2196, 2220, 2244, 2268, 2292, 2316, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 48, 56, 64, 72, 84, 92, 116, 124, 132, 140, 148, 160, 168, 192, 200, 224, 232, 244, 268, 276, 284, 292, 316, 324, 332, 356, 368, 376, 384, 408, 416, 440, 448, 456, 464, 488, 512, 524, 532, 540, 548, 556, 564, 572, 580, 592, 600, 608, 616, 628, 636, 644, 652, 660, 668, 692, 700, 712, 720, 728, 740, 764, 772, 796, 804, 812, 836, 860, 868, 876, 884, 908, 916, 924, 932, 940, 948, 956, 964, 972, 980, 1004, 1012, 1036, 1044, 1052, 1060, 1084, 1092, 1100, 1124, 1148, 1156, 1180, 1204, 1212, 1220, 1244, 1268, 1292, 1300, 1308, 1316, 1324, 1348, 1372, 1380, 1404, 1412, 1420, 1428, 1436, 1444, 1468, 1492, 1500, 1508, 1532, 1540, 1564, 1588, 1612, 1620, 1644, 1668, 1692, 1716, 1740, 1764, 1788, 1812, 1836, 1860, 1884, 1908, 1932, 1956, 1980, 2004, 2028, 2052, 2076, 2100, 2124, 2148, 2172, 2196, 2220, 2244, 2268, 2292, 2316, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 44, 56, 64, 72, 80, 92, 116, 124, 132, 140, 148, 156, 168, 176, 200, 224, 232, 244, 252, 260, 284, 292, 300, 308, 332, 344, 352, 360, 384, 392, 400, 408, 432, 440, 452, 460, 468, 492, 500, 508, 516, 540, 564, 572, 584, 592, 600, 612, 620, 632, 644, 652, 660, 668, 676, 684, 692, 700, 708, 716, 724, 732, 740, 764, 772, 796, 804, 812, 820, 844, 868, 876, 884, 892, 916, 924, 932, 940, 948, 972, 980, 988, 996, 1004, 1012, 1020, 1028, 1036, 1044, 1068, 1092, 1116, 1124, 1148, 1156, 1164, 1172, 1196, 1204, 1212, 1236, 1260, 1284, 1308, 1316, 1340, 1348, 1356, 1364, 1388, 1412, 1420, 1428, 1452, 1460, 1484, 1492, 1516, 1524, 1548, 1572, 1596, 1620, 1644, 1668, 1692, 1716, 1740, 1764, 1788, 1812, 1836, 1860, 1884, 1908, 1932, 1956, 1980, 2004, 2028, 2052, 2076, 2100, 2124, 2148, 2172, 2196, 2220, 2244, 2268, 2292, 2316, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652},
	{12, 24, 36, 44, 56, 64, 72, 80, 92, 100, 124, 132, 140, 148, 156, 164, 176, 200, 208, 232, 244, 252, 260, 268, 276, 284, 296, 320, 328, 336, 344, 368, 376, 388, 396, 420, 428, 436, 444, 452, 460, 484, 492, 504, 512, 536, 560, 572, 584, 596, 620, 628, 636, 644, 652, 660, 668, 676, 684, 692, 700, 708, 716, 724, 732, 740, 748, 756, 764, 772, 780, 788, 812, 820, 844, 852, 860, 868, 876, 900, 924, 932, 940, 964, 972, 980, 988, 996, 1004, 1012, 1036, 1044, 1052, 1060, 1068, 1076, 1084, 1108, 1116, 1140, 1164, 1188, 1196, 1204, 1228, 1236, 1244, 1252, 1276, 1300, 1324, 1332, 1356, 1380, 1404, 1412, 1436, 1444, 1468, 1476, 1500, 1524, 1548, 1572, 1596, 1620, 1644, 1668, 1692, 1716, 1740, 1764, 1788, 1812, 1836, 1860, 1884, 1908, 1932, 1956, 1980, 2004, 2028, 2052, 2076, 2100, 2124, 2148, 2172, 2196, 2220, 2244, 2268, 2292, 2316, 2340, 2364, 2388, 2412, 2436, 2460, 2484, 2508, 2532, 2556, 2580, 2604, 2628, 2652}
};

CSymbols::CSymbols()
{
	__SEH_SET_EXCEPTION_HANDLER

	int			i = 0, j = 0, k = 0, vndx = 0;
	char		*ptr = NULL;

	InitializeCriticalSectionAndSpinCount(&cs_symbols, 4000);

	ResetSymbolsFirstTime();

	//Initialise the handrank tables used by prwin
	vndx=0; //used to provide an offset into the vanilla table
	for (i=0;i<169;i++)
	{
		//normal weighted prwin table
		ptr=prwhandrank169[i];
		j=(strchr(ctonum,*ptr)-ctonum)*13 + (strchr(ctonum,*(ptr+1))-ctonum);
		if (*(ptr+2)=='s')pair2ranks[j]=i+1;
		else pair2ranko[j]=i+1;
		//prw1326 vanilla table
		j=strchr(ctonum,*ptr)-ctonum;
		k=strchr(ctonum,*(ptr+1))-ctonum;
		for(;;)
		{
			//I originally had an algorithm to do this, but it was obscure and impenetrable
			//so now I have switched to the clumsy but simple approach.
			if(j==k)//pair
			{
				_prw1326.vanilla_chair.rankhi[vndx]=j;	//h
				_prw1326.vanilla_chair.rankhi[vndx+1]=j;	//h
				_prw1326.vanilla_chair.rankhi[vndx+2]=j; //h
				_prw1326.vanilla_chair.rankhi[vndx+3]=j+13; //d
				_prw1326.vanilla_chair.rankhi[vndx+4]=j+13; //d
				_prw1326.vanilla_chair.rankhi[vndx+5]=j+26; //c
				_prw1326.vanilla_chair.ranklo[vndx]=k+13;	//d
				_prw1326.vanilla_chair.ranklo[vndx+1]=k+26;	//c
				_prw1326.vanilla_chair.ranklo[vndx+2]=k+39;	//s
				_prw1326.vanilla_chair.ranklo[vndx+3]=k+26;	//c	
				_prw1326.vanilla_chair.ranklo[vndx+4]=k+39;	//s
				_prw1326.vanilla_chair.ranklo[vndx+5]=k+39;	//s
				vndx+=6;
				break;
			}
		
			if (*(ptr+2)=='s') //suited
			{
				_prw1326.vanilla_chair.rankhi[vndx]=j;		//h
				_prw1326.vanilla_chair.rankhi[vndx+1]=j+13;	//d
				_prw1326.vanilla_chair.rankhi[vndx+2]=j+26;	//c
				_prw1326.vanilla_chair.rankhi[vndx+3]=j+39;	//s
				_prw1326.vanilla_chair.ranklo[vndx]=k;		//h
				_prw1326.vanilla_chair.ranklo[vndx+1]=k+13;	//d
				_prw1326.vanilla_chair.ranklo[vndx+2]=k+26;	//c
				_prw1326.vanilla_chair.ranklo[vndx+3]=k+39;	//s
				vndx+=4;
				break;
			}
		
			//only unsuited non-pairs left
			_prw1326.vanilla_chair.rankhi[vndx]=j;		//h
			_prw1326.vanilla_chair.rankhi[vndx+1]=j;		//h
			_prw1326.vanilla_chair.rankhi[vndx+2]=j;		//h
			_prw1326.vanilla_chair.rankhi[vndx+3]=j+13;	//d
			_prw1326.vanilla_chair.rankhi[vndx+4]=j+13;	//d
			_prw1326.vanilla_chair.rankhi[vndx+5]=j+13;	//d
			_prw1326.vanilla_chair.rankhi[vndx+6]=j+26;	//c
			_prw1326.vanilla_chair.rankhi[vndx+7]=j+26;	//c
			_prw1326.vanilla_chair.rankhi[vndx+8]=j+26;	//c
			_prw1326.vanilla_chair.rankhi[vndx+9]=j+39;	//s
			_prw1326.vanilla_chair.rankhi[vndx+10]=j+39;	//s
			_prw1326.vanilla_chair.rankhi[vndx+11]=j+39; //s Matrix corrected typo
			_prw1326.vanilla_chair.ranklo[vndx]=k+13;	//d
			_prw1326.vanilla_chair.ranklo[vndx+1]=k+26;	//c
			_prw1326.vanilla_chair.ranklo[vndx+2]=k+39;	//s
			_prw1326.vanilla_chair.ranklo[vndx+3]=k;		//h
			_prw1326.vanilla_chair.ranklo[vndx+4]=k+26;	//c
			_prw1326.vanilla_chair.ranklo[vndx+5]=k+39;	//s
			_prw1326.vanilla_chair.ranklo[vndx+6]=k;		//h
			_prw1326.vanilla_chair.ranklo[vndx+7]=k+13;	//d
			_prw1326.vanilla_chair.ranklo[vndx+8]=k+39;	//s
			_prw1326.vanilla_chair.ranklo[vndx+9]=k;		//h
			_prw1326.vanilla_chair.ranklo[vndx+10]=k+13;	//d
			_prw1326.vanilla_chair.ranklo[vndx+11]=k+26;	//c
			vndx+=12;
			break;
		}
	}

	_prw1326.vanilla_chair.level=1024;
	_prw1326.vanilla_chair.limit=820; //cut off a little early, since 820-884 very improbable

	// now assign a weight table. Assume upper third fully probable, next third reducing
	// probability, lowest third not played.
	for(i=0;i<442;i++)
		_prw1326.vanilla_chair.weight[i]=_prw1326.vanilla_chair.level;
	for(i=442;i<884;i++)
		_prw1326.vanilla_chair.weight[i]=_prw1326.vanilla_chair.level*(884-i)/442;
	for(i=884;i<1326;i++)
		_prw1326.vanilla_chair.weight[i]=0;

	//finally copy the vanilla to all user chairs so that someone who just turns on prw1326
	//experimentally does not cause a crash
	for(i=0;i<10;i++)
		_prw1326.chair[i]=_prw1326.vanilla_chair ;

	//end of handrank initialisation
}

CSymbols::~CSymbols()
{
	DeleteCriticalSection(&cs_symbols);
}

void CSymbols::ResetSymbolsFirstTime(void)
{
	int		i = 0;

	// general
	_sym.ismanual = 0;
	_sym.isppro = 0;
	_sym.site = 1;
	_sym.nchairs = 0;
	_sym.isbring = 0;
	_sym.session = 1;
	_sym.handnumber = 0;
	_sym.version = VERSION_NUMBER;

	// profile
	_sym.swagdelay = 0;
	_sym.allidelay = 0;
	_sym.swagtextmethod = 3;
	_sym.potmethod = 1;
	_sym.activemethod = 1;

	// formula
	_sym.rake = _sym.bankroll = 0;
	_sym.nit = 0;

	// limits
	_sym.bblind = _sym.sblind = _sym.ante = 0;
	_sym.lim = -1;
	_sym.isnl = _sym.ispl = _sym.isfl = 0;
	_sym.istournament = 0;
	_sym.sraiprev = _sym.sraimin = _sym.sraimax = 0;

	// handrank
	_sym.handrank169 = _sym.handrank2652 = _sym.handrank1326 = _sym.handrank1000 = 0;
	_sym.handrankp = _sym.handrank = 0;

	// chairs
	_sym.chair = _sym.userchair = _sym.dealerchair = _sym.raischair = 0;
	_user_chair_confirmed = false;

	// rounds positions
	_sym.betround = _sym.br = 0;
	_sym.betposition = _sym.dealposition = _sym.callposition = _sym.seatposition = 1;
	_sym.dealpositionrais = _sym.betpositionrais = 1;

	// probabilities
	_sym.random = _sym.randomhand = 0;

	for (i=0; i<5; i++)
		_sym.randomround[i] = 0;

	_sym.prwin = _sym.prlos = _sym.prtie = 0;

	// statistics
	_sym.callror = _sym.raisror = _sym.srairor = _sym.alliror = 0;
	_sym.callmean = _sym.raismean = _sym.sraimean = _sym.allimean = 0;
	_sym.callvariance = _sym.raisvariance = _sym.sraivariance = _sym.allivariance = 0;

	// p formula
	_sym.defcon = _sym.isdefmode = _sym.isaggmode = 0;

	// chip amounts
	for (i=0; i<11; i++)
	{
		_sym.balance[i] = 0;
		_sym.currentbet[i] = 0;
	}

	for (i=0; i<10; i++)
		_sym.stack[i] = 0;

	for (i=0; i<5; i++)
		_sym.bet[i] = 0;

	_sym.pot = _sym.potcommon = _sym.potplayer = 0;
	_sym.call = _sym.callshort = _sym.raisshort = 0;

	// number of bets
	_sym.nbetstocall = _sym.nbetstorais = _sym.ncurrentbets = _sym.ncallbets = _sym.nraisbets = 0;

	// list tests
	_sym.islistcall = _sym.islistrais = _sym.islistalli = 0;
	for (i=0; i<MAX_HAND_LISTS; i++) {
		_sym.islist[i] = 0;
	}
	_sym.isemptylistcall = _sym.isemptylistrais = _sym.isemptylistalli = 0;

	// list numbers
	_sym.nlistmax = -1;
	_sym.nlistmin = -1;

	// poker values
	_sym.pokerval = _sym.pokervalplayer = _sym.pokervalcommon = _sym.pcbits = _sym.npcbits = 0;

	// hand tests
	for (i=0; i<=1; i++)
	{
		_sym.$$pc[i] = WH_NOCARD;
		_sym.$$pr[i] = 0;
		_sym.$$ps[i] = 0;
	}

	for (i=0; i<=4; i++)
	{
		_sym.$$cc[i] = WH_NOCARD;
		_sym.$$cs[i] = 0;
		_sym.$$cr[i] = 0;
	}

	for (i=0; i<=3; i++)
	{
		_phandval[i] = 0;
		_chandval[i] = 0;
	}
	_sym.ishandup = _sym.ishandupcommon = 0;
	_sym.ishicard = _sym.isonepair = _sym.istwopair = _sym.isthreeofakind = _sym.isstraight = 0;
	_sym.isflush = _sym.isfullhouse = _sym.isfourofakind = _sym.isstraightflush = _sym.isroyalflush = 0;
	_sym.isfiveofakind = 0;

	// pocket tests
	_sym.ispair = _sym.issuited = _sym.isconnector = 0;

	// pocket/common tests
	_sym.ishipair = _sym.islopair = _sym.ismidpair = _sym.ishistraight = _sym.ishiflush = 0;

	// players friends opponents
	_sym.nopponents = 0;
	_sym.nopponentsmax = 0;
	_sym.nplayersseated = _sym.nplayersactive = _sym.nplayersdealt = 0;
	_sym.nplayersplaying = _sym.nplayersblind = 0;
	_sym.nopponentsseated = _sym.nopponentsactive = _sym.nopponentsdealt = 0;
	_sym.nopponentsplaying = _sym.nopponentsblind = 0;
	_sym.nopponentschecking = _sym.nopponentscalling = _sym.nopponentsraising = 0;
	_sym.nopponentsbetting = _sym.nopponentsfolded = 0;
	_sym.nplayerscallshort = _sym.nchairsdealtright = _sym.nchairsdealtleft = 0;
	_sym.playersseatedbits = _sym.playersactivebits = _sym.playersdealtbits = 0;
	_sym.playersplayingbits = _sym.playersblindbits = 0;
	_sym.opponentsseatedbits = _sym.opponentsactivebits = _sym.opponentsdealtbits = 0;
	_sym.opponentsplayingbits = _sym.opponentsblindbits = 0;
	_sym.nfriendsseated = _sym.nfriendsactive = _sym.nfriendsdealt = 0;
	_sym.nfriendsplaying = _sym.nfriendsblind = 0;
	_sym.friendsseatedbits = _sym.friendsactivebits = _sym.friendsdealtbits = 0;
	_sym.friendsplayingbits = _sym.friendsblindbits = 0;

	// common cards
	_sym.ncommoncardspresent = _sym.ncommoncardsknown = _sym.nflopc = 0;

	// (un)known cards
	_sym.nouts = _sym.ncardsknown = _sym.ncardsunknown = _sym.ncardsbetter = 0;

	// nhands
	_sym.nhands = _sym.nhandshi = _sym.nhandslo = _sym.nhandsti = _sym.prwinnow = _sym.prlosnow = 0;

	// flushes straights sets
	_sym.nsuited = _sym.nsuitedcommon = _sym.tsuit = _sym.tsuitcommon = 0;
	_sym.nranked = _sym.nrankedcommon = _sym.trank = _sym.trankcommon = 0;
	//straightfill inits changed from 0 to 5; 2008-03-25 Matrix
	_sym.nstraight = _sym.nstraightcommon = 0;
	_sym.nstraightflush = _sym.nstraightflushcommon = 0;
	_sym.nstraightfill = _sym.nstraightfillcommon = _sym.nstraightflushfill = _sym.nstraightflushfillcommon = 5;
	// rank bits
	_sym.rankbits = _sym.rankbitscommon = _sym.rankbitsplayer = _sym.rankbitspoker = 0;
	_sym.srankbits = _sym.srankbitscommon = _sym.srankbitsplayer = _sym.srankbitspoker = 0;

	// rank hi
	_sym.rankhi = _sym.rankhicommon = _sym.rankhiplayer = _sym.rankhipoker = 0;
	_sym.srankhi = _sym.srankhicommon = _sym.srankhiplayer = _sym.srankhipoker = 0;

	// rank lo
	_sym.ranklo = _sym.ranklocommon = _sym.rankloplayer = _sym.ranklopoker = 0;
	_sym.sranklo = _sym.sranklocommon = _sym.srankloplayer = _sym.sranklopoker = 0;

	// time
	_sym.elapsed = _sym.elapsedhand = _sym.elapsedauto = _sym.elapsedtoday = _sym.elapsed1970 = 0;
	_sym.clocks = _sym.nclockspersecond = _sym.ncps = 0;
	time(&_elapsedhold);
	time(&_elapsedhandhold);
	time(&_elapsedautohold);

	// autoplayer
	_sym.myturnbits = _sym.ismyturn = _sym.issittingin = _sym.isautopost = 0;
	_sym.issittingout = 1;
	_sym.isfinalanswer = 0;

	// history
	for (i=0; i<=4; i++)
	{
		_sym.nplayersround[i] = 0;
		_sym.nbetsround[i] = 0;
		_sym.didchec[i] = 0;
		_sym.didcall[i] = 0;
		_sym.didrais[i] = 0;
		_sym.didswag[i] = 0;
	}

	//run$ ron$
	_sym.ron$royfl = _sym.ron$strfl = _sym.ron$4kind = _sym.ron$fullh = _sym.ron$flush = _sym.ron$strai = 0;
	_sym.ron$3kind = _sym.ron$2pair = _sym.ron$1pair = _sym.ron$hcard = _sym.ron$total = _sym.ron$pokervalmax = 0;
	_sym.ron$prnuts = _sym.ron$prbest = _sym.ron$clocks = 0;
	_sym.run$royfl = _sym.run$strfl = _sym.run$4kind = _sym.run$fullh = _sym.run$flush = _sym.run$strai = 0;
	_sym.run$3kind = _sym.run$2pair = _sym.run$1pair = _sym.run$hcard = _sym.run$total = _sym.run$pokervalmax = 0;
	_sym.run$prnuts = _sym.run$prbest = _sym.run$clocks = 0;

	// vs$
	_sym.vs$nhands = _sym.vs$nhandshi = _sym.vs$nhandsti = _sym.vs$nhandslo = 0;
	_sym.vs$prwin = _sym.vs$prtie = _sym.vs$prlos = 0;
	_sym.vs$prwinhi = _sym.vs$prtiehi = _sym.vs$prloshi = 0;
	_sym.vs$prwinti = _sym.vs$prtieti = _sym.vs$prlosti = 0;
	_sym.vs$prwinlo = _sym.vs$prtielo = _sym.vs$prloslo = 0;

	// action symbols
	_f$alli = _f$swag = _f$rais = _f$call = _f$play = _f$prefold = 0;


	_bigbet = 0;
	set_reset_stakes(true);

	// icm
	for (i=0; i<=9; i++)
		_stacks_at_hand_start[i] = 0;

	// delay
	_f$delay = 0;

	// Reset semi-persistent hand state when we instantiate CSymbols.
	CSymbols::_dealerchair_last = -1;
	CSymbols::_handnumber_last = -1;
	CSymbols::_br_last = -1;
	CSymbols::_player_card_last[0] = CARD_NOCARD;
	CSymbols::_player_card_last[1] = CARD_NOCARD;

	// log$ symbols
	_logsymbols_collection.RemoveAll();

	_symboltrace_collection.RemoveAll();

	// time
	LARGE_INTEGER PerformanceCount;
	QueryPerformanceCounter(&PerformanceCount);
	p_scraper->set_clocks_hold(PerformanceCount);
}

void CSymbols::ResetSymbolsNewHand(void)
{
	int		i = 0;

	// handrank
	_sym.handrank169 = _sym.handrank2652 = _sym.handrank1326 = _sym.handrank1000 = _sym.handrank = 0;

	// chip amounts
	for (i=0; i<5; i++)
		_sym.bet[i] = 0;

	// list tests
	_sym.islistcall = _sym.islistrais = _sym.islistalli = 0;
	for (i=0; i<MAX_HAND_LISTS; i++)
		_sym.islist[i] = 0;

	_sym.isemptylistcall = _sym.isemptylistrais = _sym.isemptylistalli = 0;

	// list numbers
	_sym.nlistmax = -1;
	_sym.nlistmin = -1;

	// hand tests
	for (i=0; i<2; i++)
	{
		_sym.$$pc[i] = WH_NOCARD;
		_sym.$$pr[i] = 0;
		_sym.$$ps[i] = 0;
	}

	for (i=0; i<5; i++)
	{
		_sym.$$cc[i] = WH_NOCARD;
		_sym.$$cs[i] = 0;
		_sym.$$cr[i] = 0;
	}

	for (i=0; i<4; i++)
	{
		_phandval[i] = 0;
		_chandval[i] = 0;
	}

	// players friends opponents
	_sym.nopponentsmax = 0;
	_sym.nplayersseated = _sym.nplayersdealt = _sym.nplayersblind = 0;
	_sym.nopponentsseated = _sym.nopponentsdealt = _sym.nopponentsblind = 0;
	_sym.playersseatedbits = _sym.playersdealtbits = _sym.playersblindbits = 0;
	_sym.opponentsseatedbits = _sym.opponentsdealtbits = _sym.opponentsblindbits = 0;

	// time
	time(&_elapsedhandhold);

	// history
	for (i=0; i<=4; i++)
	{
		_sym.nplayersround[i] = 0;
		_sym.nbetsround[i] = 0;
		_sym.didchec[i] = 0;
		_sym.didcall[i] = 0;
		_sym.didrais[i] = 0;
		_sym.didswag[i] = 0;
	}

	// action symbols
	_f$alli = _f$swag = _f$rais = _f$call = _f$play = _f$prefold = 0;

	// icm
	for (i=0; i<=9; i++)
		_stacks_at_hand_start[i] = 0;
}

void CSymbols::ResetSymbolsNewRound(void)
{
	_sym.didchec[4] = 0;
	_sym.didcall[4] = 0;
	_sym.didrais[4] = 0;
	_sym.didswag[4] = 0;
}

void CSymbols::ResetSymbolsEveryCalc(void)
{
	int		i = 0;

	// general
	_sym.isbring = 0;

	// limits
	_sym.sraiprev = _sym.sraimin = _sym.sraimax = 0;

	// handrank
	_sym.handrankp = 0;

	// chairs
	_sym.raischair = 0;

	// rounds positions
	_sym.betposition = _sym.dealposition = _sym.callposition = _sym.seatposition = 1;
	_sym.dealpositionrais = _sym.betpositionrais = 1;

	// statistics
	_sym.callror = _sym.raisror = _sym.srairor = _sym.alliror = 0;
	_sym.callmean = _sym.raismean = _sym.sraimean = _sym.allimean = 0;
	_sym.callvariance = _sym.raisvariance = _sym.sraivariance = _sym.allivariance = 0;

	// chip amounts
	for (i=0; i<11; i++)
		_sym.currentbet[i] = 0;

	for (i=0; i<10; i++)
		_sym.stack[i] = 0;

	_sym.pot = _sym.potcommon = _sym.potplayer = 0;
	_sym.call = _sym.callshort = _sym.raisshort = 0;

	// number of bets
	_sym.nbetstocall = _sym.nbetstorais = _sym.ncurrentbets = _sym.ncallbets = _sym.nraisbets = 0;

	// poker values
	_sym.pokerval = _sym.pokervalplayer = _sym.pokervalcommon = _sym.pcbits = _sym.npcbits = 0;

	// hand tests
	_sym.ishandup = _sym.ishandupcommon = 0;
	_sym.ishicard = _sym.isonepair = _sym.istwopair = _sym.isthreeofakind = _sym.isstraight = 0;
	_sym.isflush = _sym.isfullhouse = _sym.isfourofakind = _sym.isstraightflush = _sym.isroyalflush = 0;
	_sym.isfiveofakind = 0;

	// pocket tests
	_sym.ispair = _sym.issuited = _sym.isconnector = 0;

	// pocket/common tests
	_sym.ishipair = _sym.islopair = _sym.ismidpair = _sym.ishistraight = _sym.ishiflush = 0;

	// players friends opponents
	_sym.nopponents = 0;
	_sym.nopponentschecking = _sym.nopponentscalling = _sym.nopponentsraising = 0;
	_sym.nopponentsbetting = _sym.nopponentsfolded = 0;
	_sym.nplayerscallshort = _sym.nchairsdealtright = _sym.nchairsdealtleft = 0;
	_sym.nplayersactive = _sym.playersactivebits = 0;
	_sym.nplayersplaying = _sym.playersplayingbits = 0;
	_sym.nopponentsactive = _sym.opponentsactivebits = 0;
	_sym.nopponentsplaying = _sym.opponentsplayingbits = 0;

	// flags
	_sym.fmax = _sym.fbits = 0;
	for (i=0; i<10; i++)
		_sym.f[i] = 0;

	// (un)known cards
	_sym.nouts = _sym.ncardsknown = _sym.ncardsunknown = _sym.ncardsbetter = 0;

	// nhands
	_sym.nhands = _sym.nhandshi = _sym.nhandslo = _sym.nhandsti = _sym.prwinnow = _sym.prlosnow = 0;

	// flushes straights sets
	_sym.nsuited = _sym.nsuitedcommon = _sym.tsuit = _sym.tsuitcommon = 0;
	_sym.nranked = _sym.nrankedcommon = _sym.trank = _sym.trankcommon = 0;
	//straightfill inits changed from 0 to 5; 2008-03-25 Matrix
	_sym.nstraight = _sym.nstraightcommon = 0;
	_sym.nstraightflush = _sym.nstraightflushcommon = 0;
	_sym.nstraightfill = _sym.nstraightfillcommon = _sym.nstraightflushfill = _sym.nstraightflushfillcommon = 5;

	// rank bits
	_sym.rankbits = _sym.rankbitscommon = _sym.rankbitsplayer = _sym.rankbitspoker = 0;
	_sym.srankbits = _sym.srankbitscommon = _sym.srankbitsplayer = _sym.srankbitspoker = 0;

	// rank hi
	_sym.rankhi = _sym.rankhicommon = _sym.rankhiplayer = _sym.rankhipoker = 0;
	_sym.srankhi = _sym.srankhicommon = _sym.srankhiplayer = _sym.srankhipoker = 0;

	// rank lo
	_sym.ranklo = _sym.ranklocommon = _sym.rankloplayer = _sym.ranklopoker = 0;
	_sym.sranklo = _sym.sranklocommon = _sym.srankloplayer = _sym.sranklopoker = 0;

	// autoplayer
	_sym.myturnbits = _sym.ismyturn = _sym.issittingin = _sym.isautopost = 0;
	_sym.issittingout = 1;
	_sym.isfinalanswer = 0;

	//run$ ron$
	_sym.ron$royfl = _sym.ron$strfl = _sym.ron$4kind = _sym.ron$fullh = _sym.ron$flush = _sym.ron$strai = 0;
	_sym.ron$3kind = _sym.ron$2pair = _sym.ron$1pair = _sym.ron$hcard = _sym.ron$total = _sym.ron$pokervalmax = 0;
	_sym.ron$prnuts = _sym.ron$prbest = _sym.ron$clocks = 0;
	_sym.run$royfl = _sym.run$strfl = _sym.run$4kind = _sym.run$fullh = _sym.run$flush = _sym.run$strai = 0;
	_sym.run$3kind = _sym.run$2pair = _sym.run$1pair = _sym.run$hcard = _sym.run$total = _sym.run$pokervalmax = 0;
	_sym.run$prnuts = _sym.run$prbest = _sym.run$clocks = 0;

	// vs$
	_sym.vs$nhands = _sym.vs$nhandshi = _sym.vs$nhandsti = _sym.vs$nhandslo = 0;
	_sym.vs$prwin = _sym.vs$prtie = _sym.vs$prlos = 0;
	_sym.vs$prwinhi = _sym.vs$prtiehi = _sym.vs$prloshi = 0;
	_sym.vs$prwinti = _sym.vs$prtieti = _sym.vs$prlosti = 0;
	_sym.vs$prwinlo = _sym.vs$prtielo = _sym.vs$prloslo = 0;

	// log$ symbols
	_logsymbols_collection.RemoveAll();

	_symboltrace_collection.RemoveAll();
}

void CSymbols::CalcSymbols(void)
{
	int					i = 0;
	char				classname[50] = {0}, title[512] = {0};
	unsigned int		player_card_cur[2] = {0};
	char				card0[10] = {0}, card1[10] = {0};
	CGrammar			gram;
	CMainFrame			*pMyMainWnd  = (CMainFrame *) (theApp.m_pMainWnd);

	// Clear em, before we start
	ResetSymbolsEveryCalc();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Global environment symbols
	_sym.session = theApp._session_id;												// session
	_sym.nopponentsmax = prefs.max_opponents();										// nopponentsmax
	_sym.swagdelay = prefs.swag_delay_3();											// swagdelay
	_sym.allidelay = -1;															// allidelay  (unused in OpenHoldem)
	_sym.version = VERSION_NUMBER;													// version
	GetClassName(pMyMainWnd->attached_hwnd(), classname, 50);
	if (strcmp(classname, "BRING")==0)
		_sym.isbring = 1;															// isbring

	if (strcmp(classname, "OpenHoldemManualMode")==0)
		_sym.ismanual = 1;															// ismanual

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Identification of dealerchair
	for (i=0; i < p_tablemap->s$items()->num_chairs; i++)
	{
		if (p_scraper->dealer(i))
		{
			_sym.dealerchair = i;													// dealerchair
			i = p_tablemap->s$items()->num_chairs + 1;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Identification of userchair
	if (!_user_chair_confirmed)
	{
		_user_chair_confirmed = CalcUserChair();

		if (_user_chair_confirmed)
			time(&_elapsedhold);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Things to do when we have a new hand
	// Get current userchair's cards
	if (_user_chair_confirmed)
	{
		player_card_cur[0] = p_scraper->card_player((int) _sym.userchair, 0);
		player_card_cur[1] = p_scraper->card_player((int) _sym.userchair, 1);
	}
	else
	{
		player_card_cur[0] = player_card_cur[1] = CARD_NOCARD;
	}

	_sym.handnumber = p_scraper->s_limit_info()->handnumber;									// handnumber

	// New hand is triggered by change in dealerchair (button moves), or change in userchair's cards (as long as it is not
	// a change to nocards or cardbacks), or a change in handnumber
	if (_sym.dealerchair != _dealerchair_last ||
			player_card_cur[0]!=CARD_NOCARD && player_card_cur[0]!=CARD_BACK && player_card_cur[0]!=_player_card_last[0] ||
			player_card_cur[1]!=CARD_NOCARD && player_card_cur[1]!=CARD_BACK && player_card_cur[1]!=_player_card_last[1] ||
			_sym.handnumber != _handnumber_last)
	{
		// Save for next pass
		_dealerchair_last = _sym.dealerchair;
		_player_card_last[0] = player_card_cur[0];
		_player_card_last[1] = player_card_cur[1];
		_handnumber_last = _sym.handnumber;
		_br_last = -1;	// ensure betround reset

		// Update game_state so it knows that a new hand has happened
		p_game_state->set_new_hand(true);

		// Reset autoplayer structures
		p_autoplayer->ResetHand();

		// Reset symbols and display
		ResetSymbolsNewHand();
		InvalidateRect(theApp.m_pMainWnd->GetSafeHwnd(), NULL, true);

		// randoms
		_sym.randomhand = (double) rand() / (double) RAND_MAX;						// randomhand
		_sym.randomround[0] = (double) rand() / (double) RAND_MAX;					// randomround1
		_sym.randomround[1] = (double) rand() / (double) RAND_MAX;					// randomround2
		_sym.randomround[2] = (double) rand() / (double) RAND_MAX;					// randomround3
		_sym.randomround[3] = (double) rand() / (double) RAND_MAX;					// randomround4

		// search for new stakes
		set_reset_stakes(true);

		// icm
		for (i=0; i<=9; i++)
			_stacks_at_hand_start[i] = p_scraper->player_balance(i) + p_scraper->player_bet(i);

		// log new hand
		if (player_card_cur[0]==CARD_NOCARD || player_card_cur[0]==CARD_NOCARD)
		{
			strcpy_s(card0, 10, "NONE");
			strcpy_s(card1, 10, "");
		}
		else if (player_card_cur[1]==CARD_BACK || player_card_cur[1]==CARD_BACK)
		{
			strcpy_s(card0, 10, "BACKS");
			strcpy_s(card1, 10, "");
		}
		else
		{
			StdDeck_cardToString(player_card_cur[0], card0);
			StdDeck_cardToString(player_card_cur[1], card1);
		}
		GetWindowText(pMyMainWnd->attached_hwnd(), title, 512);
		write_log("\n*************************************************************\nHAND RESET (num:%.0f dealer:%.0f cards:%s%s): %s\n*************************************************************\n",
				  _sym.handnumber, _sym.dealerchair, card0, card1, title);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// common cards
	_sym.nflopc = 0;

	for (i=0; i<=4; i++)
	{
		if (p_scraper->card_common(i) != CARD_NOCARD)
			_sym.nflopc++;															// nflopc
	}

	_sym.ncommoncardspresent = _sym.nflopc;											// ncommoncardspresent
	_sym.ncommoncardsknown = _sym.nflopc;												// ncommoncardsknown

	// If no common card animation is going on
	if (!p_scraper->IsCommonAnimation())
	{
		_sym.betround = p_scraper->card_common(4) != CARD_NOCARD ? 4 :
						   p_scraper->card_common(3) != CARD_NOCARD ? 3 :
						   p_scraper->card_common(2) != CARD_NOCARD &&
						   p_scraper->card_common(1) != CARD_NOCARD &&
						   p_scraper->card_common(0) != CARD_NOCARD ? 2 :
						   1;																// betround
	}
	else
	{
		// There is a common card animation going on currently
		// so lets not try to determine the betround,
		// but if it's a new hand then lets default to pre-flop
		if (_br_last == -1)
		{
			_sym.betround = 1;
		}
	}

	_sym.br = _sym.betround;															// br

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Things to do when we have a new round
	if (_sym.br == 0) 
		return;

	if (_sym.br != _br_last)
	{
		_br_last = _sym.br;

		// Reset autoplayer structures
		p_autoplayer->ResetRound();

		// Reset symbols
		ResetSymbolsNewRound();

		// log betting round change
		write_log("ROUND %.0f\n", _sym.br);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Symbols derived from current profile/formula
	_sym.site = 1;																		// site
	_sym.nchairs = p_tablemap->s$items()->num_chairs;									// nchairs
	_sym.isppro = p_pokerpro->IsConnected();											// isppro
	_sym.rake = p_formula->formula()->dRake;											// rake
	_sym.nit = p_formula->formula()->dNit;												// nit
	_sym.bankroll = p_formula->formula()->dBankroll;									// bankroll
	_sym.defcon = p_formula->formula()->dDefcon;										// defcon
	_sym.isdefmode = p_formula->formula()->dDefcon == 0.0;								// isdefmode
	_sym.isaggmode = p_formula->formula()->dDefcon == 1.0;								// isaggmode
	_sym.swagtextmethod = p_tablemap->s$items()->swagtextmethod;						// swagtextmethod
	_sym.potmethod = p_tablemap->s$items()->potmethod;									// potmethod
	_sym.activemethod = p_tablemap->s$items()->activemethod;							// activemethod

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Other scraped items
	if (_reset_stakes || _sym.sblind==0 || _sym.bblind==0 ||
		(p_pokerpro->IsConnected() && p_pokerpro->ppdata()->m_tinf.m_tid != 0))
	{
		CalcStakes();	// bblind/sblind/bbet/ante/lim
		set_reset_stakes(false);
	}

	_sym.isnl = (_sym.lim == LIMIT_NL);												// isnl
	_sym.ispl = (_sym.lim == LIMIT_PL);												// ispl
	_sym.isfl = (_sym.lim == LIMIT_FL);												// isfl

	_sym.istournament = (double) p_scraper->s_limit_info()->istournament;		// istournament

	_sym.bet[0] = _sym.bblind;														// bet1
	_sym.bet[1] = _sym.bblind;														// bet2
	_sym.bet[2] = (_bigbet!=0 ? _bigbet : (_sym.isnl || _sym.ispl ? _sym.bblind : _sym.bblind*2));	// bet3
	_sym.bet[3] = (_bigbet!=0 ? _bigbet : (_sym.isnl || _sym.ispl ? _sym.bblind : _sym.bblind*2));	// bet4

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CalcBetBalanceStack();				// bets, balances, stacks
	CalcPlayersFriendsOpponents();		// players/friends/opponents, raischair
	CalcChipamtsLimits();				// chip amounts, limits
	CalcNumbets();						// number of bets
	CalcFlags();						// flags
	CalcTime();							// time
	CalcAutoplayer();					// autoplayer

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (_user_chair_confirmed &&
		p_scraper->card_player(_sym.chair, 0)!=CARD_NOCARD && p_scraper->card_player(_sym.chair, 1)!=CARD_NOCARD &&
		p_scraper->card_player(_sym.chair, 0)!=CARD_BACK && p_scraper->card_player(_sym.chair, 1)!=CARD_BACK)
	{
		CalcRoundsPositions();			// rounds positions
		CalcPokerValues();				// pokerval, pocket-common tests
		CalcUnknownCards();				// (un)known cards
		CalcHandTests();				// hand tests
		CalcPocketTests();				// pocket tests
		CalcListTests();				// list tests, list numbers
		CalcFlushesStraightsSets();		// flushes, straights, sets
		CalcRankbits();					// rankbits, rankhi, ranklo
		p_versus->GetCounts();			// calculate versus values

		_sym.originaldealposition = _sym.dealposition; //remember dealposition Matrix 2008-05-09
	}

	CalcHistory();						// history symbols

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// number of opponents (from f$P)
	int e = SUCCESS;
	_sym.nopponents = gram.CalcF$symbol(p_formula, "f$P", &e);

	if (_sym.nopponents > prefs.max_opponents())
	{
		_sym.nopponents = prefs.max_opponents();					// nopponents
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// dependent on nopponents calulation - must come after
	if (_user_chair_confirmed &&
		p_scraper->card_player(_sym.chair, 0)!=CARD_NOCARD && p_scraper->card_player(_sym.chair, 1)!=CARD_NOCARD &&
		p_scraper->card_player(_sym.chair, 0)!=CARD_BACK && p_scraper->card_player(_sym.chair, 1)!=CARD_BACK)
	{
		CalcNhands();				// nhands
		CalcHandrank();			// handrank
		CalcProbabilities();		// prwin, random
		CalcRunRon();				// run$, ron$
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// dependent on prwin calulation - must come after
	if (_user_chair_confirmed &&
		p_scraper->card_player(_sym.chair, 0)!=CARD_NOCARD && p_scraper->card_player(_sym.chair, 1)!=CARD_NOCARD &&
		p_scraper->card_player(_sym.chair, 0)!=CARD_BACK && p_scraper->card_player(_sym.chair, 1)!=CARD_BACK)
	{
		CalcStatistics();			// ror, mean, variance
	}
}

bool CSymbols::CalcUserChair(void)
{
	int				i = 0;
	int				num_buttons_enabled = 0;

	for (i=0; i<=9; i++)
	{
		CString button_label = p_scraper->button_label(i);

		if (p_scraper->GetButtonState(i) &&
			(p_scraper->IsStringFold(button_label) ||
			 p_scraper->IsStringCheck(button_label) ||
			 p_scraper->IsStringCall(button_label) ||
			 p_scraper->IsStringRaise(button_label) ||
			 p_scraper->IsStringAllin(button_label) ||
			 button_label.MakeLower() == "swag"))
		{
			num_buttons_enabled++;
		}
	}

	if (num_buttons_enabled>=2)
	{
		for (i=0; i<p_tablemap->s$items()->num_chairs; i++)
		{
			if (p_scraper->card_player(i, 0) != CARD_NOCARD && p_scraper->card_player(i, 0) != CARD_BACK &&
				p_scraper->card_player(i, 1) != CARD_NOCARD && p_scraper->card_player(i, 1) != CARD_BACK)
			{
				_sym.userchair = i;													// userchair
				_sym.chair = i;														// chair
				return true;
			}
		}
	}

	return false;
}

void CSymbols::CalcStakes(void)
{
	int		i = 0;
	bool	found_inferred_sb = false, found_inferred_bb = false;

	EnterCriticalSection(&cs_symbols);

		_sym.sblind = _sym.bblind = _bigbet = _sym.ante = 0;

		// Save the parts we scraped successfully
		if (p_scraper->s_limit_info()->found_sblind)
			_sym.sblind = p_scraper->s_limit_info()->sblind;									// sblind

		if (p_scraper->s_limit_info()->found_bblind)
			_sym.bblind = p_scraper->s_limit_info()->bblind;									// bblind

		if (p_scraper->s_limit_info()->found_ante)
			_sym.ante = p_scraper->s_limit_info()->ante;										// ante

		if (p_scraper->s_limit_info()->found_limit)
			_sym.lim = p_scraper->s_limit_info()->limit;										// lim

		if (p_scraper->s_limit_info()->found_bbet)
			_bigbet = p_scraper->s_limit_info()->bbet;

		// Figure out bb/sb based on game type
		if (_sym.lim == LIMIT_NL || _sym.lim == LIMIT_PL)
		{
			if (_sym.sblind==0)
			{
				if (p_scraper->s_limit_info()->found_sb_bb)
					_sym.sblind = p_scraper->s_limit_info()->sb_bb;
			}

			if (_sym.bblind==0)
			{
				if (p_scraper->s_limit_info()->found_bb_BB)
					_sym.bblind = p_scraper->s_limit_info()->bb_BB;
			}

			if (_bigbet==0)
			{
				if (p_scraper->s_limit_info()->found_bb_BB)
					_bigbet = p_scraper->s_limit_info()->bb_BB;

				else if (p_scraper->s_limit_info()->found_sb_bb)
					_bigbet = p_scraper->s_limit_info()->sb_bb*2;

				else if (_sym.bblind!=0)
					_bigbet = _sym.bblind;

				else if (_sym.sblind!=0)
					_bigbet = _sym.sblind*2;

			}
		}

		else if (_sym.lim == LIMIT_FL || _sym.lim == -1)
		{
			if (_sym.sblind==0)
			{
				if (p_scraper->s_limit_info()->found_sb_bb)
					_sym.sblind = p_scraper->s_limit_info()->sb_bb;
			}

			if (_sym.bblind==0)
			{
				if (p_scraper->s_limit_info()->found_bb_BB)
					_sym.bblind = p_scraper->s_limit_info()->bb_BB;
			}

			if (_bigbet==0)
			{
				if (p_scraper->s_limit_info()->found_bb_BB)
					_bigbet = p_scraper->s_limit_info()->bb_BB*2;

				else if (p_scraper->s_limit_info()->found_sb_bb)
					_bigbet = p_scraper->s_limit_info()->sb_bb*4;

				else if (_sym.bblind!=0)
					_bigbet = _sym.bblind*2;

				else if (_sym.sblind!=0)
					_bigbet = _sym.sblind*4;
			}
		}

		// Return now, if we have locked the blinds
		if (!p_scraper->s_lock_blinds()->blinds_are_locked) {
			// if we still do not have blinds, then infer them from the posted bets
			if (_sym.br == 1 && (_sym.sblind==0 || _sym.bblind==0))
			{
				for (i=_sym.dealerchair+1; i<=_sym.dealerchair+p_tablemap->s$items()->num_chairs; i++)
				{
					if (p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 0) != CARD_NOCARD && 
						p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 0) != CARD_NOCARD)
					{
						if (p_scraper->player_bet(i%p_tablemap->s$items()->num_chairs) != 0 && !found_inferred_sb)
						{
							if (_sym.sblind==0)
							{
								_sym.sblind = p_scraper->player_bet(i%p_tablemap->s$items()->num_chairs);
								found_inferred_sb = true;
							}
						}

						else if (p_scraper->player_bet(i%p_tablemap->s$items()->num_chairs) != 0 && found_inferred_sb && !found_inferred_bb)
						{
							if (_sym.bblind==0)
							{
								// !heads up - normal blinds
								if (i%p_tablemap->s$items()->num_chairs != _sym.dealerchair)
								{
									_sym.bblind = p_scraper->player_bet(i%p_tablemap->s$items()->num_chairs);
								}
								// heads up - reversed blinds
								else
								{
									_sym.bblind = _sym.sblind;
									_sym.sblind = p_scraper->player_bet(i%p_tablemap->s$items()->num_chairs);
								}
								found_inferred_bb = true;
							}
						}
					}
				}

				// check for reasonableness
				if (_sym.bblind > _sym.sblind*2)
					_sym.bblind = _sym.sblind*2;

				if (_sym.sblind >= _sym.bblind)
					_sym.sblind = _sym.bblind/2;
			}
		}
	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcBetBalanceStack(void)
{
	int				i = 0, j = 0, oppcount = 0;
	double			stack[10] = {0}, temp = 0.;

	EnterCriticalSection(&cs_symbols);
		_sym.bet[4] = _sym.bet[(int) (_sym.br-1)];											// bet

		for (i=0; i<_sym.nchairs; i++)
			_sym.balance[i] = _sym.nchairs>i ? p_scraper->player_balance(i) : 0;						// balance0-9

		_sym.balance[10] = _user_chair_confirmed ? p_scraper->player_balance(_sym.userchair) : 0;			// balance

		// simple bubble sort for 10 stack values
		for (i=0; i<_sym.nchairs; i++)
		{
			if (p_scraper->card_player(i, 0) != CARD_NOCARD && p_scraper->card_player(i, 1) != CARD_NOCARD)
				stack[i] = _sym.balance[i];

			else
				stack[i] = 0;
		}
	LeaveCriticalSection(&cs_symbols);


	for (i=0; i<_sym.nchairs-1; i++)
	{
		for (j=i+1; j<_sym.nchairs; j++)
		{
			if (stack[i]<stack[j])
			{
				temp = stack[i];
				stack[i]=stack[j];
				stack[j]=temp;
			}
		}
	}


	EnterCriticalSection(&cs_symbols);

		for (i=0; i<_sym.nchairs; i++)
			_sym.stack[i] = _sym.nchairs>i ? stack[i] : 0;								// stack0-9


		//
		// currentbet sanity check
		//
		// Get count of opponents
		oppcount=0;
		for (i=0; i<_sym.nchairs; i++)
		{
			if (p_scraper->card_player(i, 0) == CARD_BACK && p_scraper->card_player(i, 1) == CARD_BACK && i != _sym.userchair)
				oppcount+=1;
		}

		for (i=0; i<_sym.nchairs; i++)
		{
			temp = _sym.nchairs>i ? p_scraper->player_bet(i) : 0;

			// Only record currentbet if pot>0 or num of opponents>0
			//if (pot[0]>0 || oppcount>0) {
			if (oppcount>0)
			{
				// fixed limit
				if (_sym.isfl)
				{
					if (temp/_sym.bet[4]<=4)
						_sym.currentbet[i] = temp;										// currentbet0-9
				}

				// no limit, pot limit
				else
				{
					_sym.currentbet[i] = temp; 											// currentbet0-9
				}
			}
		}

		_sym.currentbet[10] = _user_chair_confirmed ? p_scraper->player_bet(_sym.userchair) : 0;			// currentbet

		_sym.potplayer=0;
		for (i=0; i<_sym.nchairs; i++)
		{
			_sym.potplayer += _sym.currentbet[i];											// potplayer
		}

		if (_sym.potmethod == 2)															// pot, potcommon ->
		{
			_sym.pot = p_scraper->pot(0);
			_sym.potcommon = _sym.pot - _sym.potplayer;
		}
		else
		{
			_sym.potcommon = 0;
			for (i=0; i<=4; i++)
				_sym.potcommon += p_scraper->pot(i);

			_sym.pot = _sym.potcommon + _sym.potplayer;									// <- pot, potcommon
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcChipamtsLimits(void) 
{
	int				i = 0;
	int				next_largest_bet = 0;

	EnterCriticalSection(&cs_symbols);	

		_sym.call = _user_chair_confirmed ?
				   _sym.currentbet[(int) _sym.raischair] - _sym.currentbet[(int) _sym.userchair] : 0; // call
		for (i=_sym.raischair+1; i<_sym.raischair+p_tablemap->s$items()->num_chairs; i++)
		{
			if ((int) _sym.playersplayingbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
			{
				_sym.callshort +=
					(_sym.currentbet[(int) _sym.raischair] - _sym.currentbet[(i%p_tablemap->s$items()->num_chairs)]);	// callshort
			}
		}
		_sym.raisshort = _sym.callshort + _sym.bet[4] * _sym.nplayersplaying;				// raisshort

		next_largest_bet = 0;
		for (i=0; i<p_tablemap->s$items()->num_chairs; i++)
		{
			if (_sym.currentbet[i] != _sym.currentbet[(int) _sym.raischair] && _sym.currentbet[i]>next_largest_bet)
			{
				next_largest_bet = _sym.currentbet[i];
			}
		}

		_sym.sraiprev = _sym.currentbet[(int) _sym.raischair] - next_largest_bet;			// sraiprev
		_sym.sraimin = _user_chair_confirmed ? _sym.currentbet[10] + _sym.call : 0;			// sraimin
		_sym.sraimax = _user_chair_confirmed ? _sym.balance[10] - _sym.call : 0;
		if (_sym.sraimax < 0)
			_sym.sraimax = 0;															// sraimax

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcNumbets(void)
{
	EnterCriticalSection(&cs_symbols);	

		if (_user_chair_confirmed)
		{
			_sym.nbetstocall = _sym.call / _sym.bet[4];									// nbetstocall
			_sym.nbetstorais = _sym.nbetstocall + 1;										// nbetstorais
			_sym.ncurrentbets =
				_sym.bet[4]==0 ? 0 : _sym.currentbet[(int) _sym.userchair] / _sym.bet[4];	// ncurrentbets
		}
		_sym.ncallbets =
			_sym.bet[4]==0 ? 0 : _sym.currentbet[(int) _sym.raischair] / _sym.bet[4];		// ncallbets
		_sym.nraisbets = _sym.ncallbets + 1;												// nraisbets

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcFlags(void)
{
	CMainFrame			*pMyMainWnd  = (CMainFrame *) (theApp.m_pMainWnd);

	EnterCriticalSection(&cs_symbols);	

		int			i = 0;

		for (i=0; i<10; i++)
		{
			_sym.f[i] = pMyMainWnd->flags(i);											// fn
			if (_sym.f[i] != 0)
			{
				if (i > _sym.fmax)
					_sym.fmax = i;														// fmax

				_sym.fbits = (int) _sym.fbits | (1<<i);									// fbits
			}
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcTime(void)
{
	LARGE_INTEGER	lFrequency = {0}, clocksnow = {0};
	time_t			t_now_time = 0, t_midnight_time = 0;
	tm				s_midnight_time = {0};

	time(&t_now_time);

	EnterCriticalSection(&cs_symbols);	

		_sym.elapsed1970 = t_now_time;													// elapsed1970

		localtime_s(&s_midnight_time, &t_now_time);
		s_midnight_time.tm_hour = 0;
		s_midnight_time.tm_min = 0;
		s_midnight_time.tm_sec = 0;
		t_midnight_time = mktime(&s_midnight_time);
		_sym.elapsedtoday = t_now_time - t_midnight_time;								// elapsedtoday

		_sym.elapsed = t_now_time - _elapsedhold;											// elapsed
		_sym.elapsedhand = t_now_time - _elapsedhandhold;									// elapsedhand
		_sym.elapsedauto = t_now_time - _elapsedautohold;									// elapsedauto

		QueryPerformanceFrequency(&lFrequency);
		_sym.nclockspersecond = lFrequency.LowPart;										// nclockspersecond
		_sym.ncps = _sym.nclockspersecond;												// ncps

		QueryPerformanceCounter(&clocksnow);

		_sym.clocks = clocksnow.LowPart - p_scraper->clocks_hold().LowPart;				// clocks

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcAutoplayer(void)
{
	int		i = 0;
	bool	sitin_but = false, sitout_but = false, sitin_state = false, sitout_state = false;

	EnterCriticalSection(&cs_symbols);	

		for (i=0; i<=9; i++)
		{
			if (p_scraper->GetButtonState(i))
			{
				CString button_label = p_scraper->button_label(i);

				if (p_scraper->IsStringFold(button_label))
					_sym.myturnbits = (int) _sym.myturnbits | (1<<0);

				else if (p_scraper->IsStringCall(button_label))
					_sym.myturnbits = (int) _sym.myturnbits | (1<<1);

				else if (p_scraper->IsStringRaise(button_label) || button_label.MakeLower() == "swag")
					_sym.myturnbits = (int) _sym.myturnbits | (1<<2);

				else if (p_scraper->IsStringAllin(button_label))
					_sym.myturnbits = (int) _sym.myturnbits | (1<<3);						// myturnbits

				else if (p_scraper->IsStringAutopost(button_label))
					_sym.isautopost = 1;													// isautopost

			}
		}
		_sym.ismyturn = (int) _sym.myturnbits & 0x7;										// ismyturn

		// Figure out sitin/sitout state
		sitin_but = sitout_but = sitin_state = sitout_state = false;
		for (i=0; i < 10; i++)
		{
			if (p_scraper->IsStringSitin(p_scraper->button_label(i)))
			{
				sitin_but = true;
				sitin_state = p_scraper->GetButtonState(i);
			}

			else if (p_scraper->IsStringSitout(p_scraper->button_label(i)))
			{
				sitout_but = true;
				sitout_state = p_scraper->GetButtonState(i);
			}
		}



		// we have a sitin button
		if (sitin_but)
		{
			if (sitin_state)
			{
				_sym.issittingin = 1;												// issittingin
				_sym.issittingout = 0;												// issittingout
			}

			else
			{
				_sym.issittingin = 0;												// issittingin
				_sym.issittingout = 1;												// issittingout
			}
		}

		// we have a sitout button
		else if (sitout_but)
		{
			if (sitout_state)
			{
				_sym.issittingin = 0;												// issittingin
				_sym.issittingout = 1;												// issittingout
			}

			else
			{
				_sym.issittingin = 1;												// issittingin
				_sym.issittingout = 0;												// issittingout
			}
		}

		// we have neither a sitout or sitin button
		else
		{
			_sym.issittingin = 1;												// issittingin
			_sym.issittingout = 0;												// issittingout
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcProbabilities(void)
{
	bool				need_recalc = false;
	int					i = 0;

	EnterCriticalSection(&cs_symbols);	
		_sym.random = (double) rand() / (double) RAND_MAX;								// random
		_sym.randomround[4] = _sym.randomround[(int) (_sym.br-1)];						// randomround

		_sym.prwin = _iter_vars.prwin;													// prwin
		_sym.prtie = _iter_vars.prtie;													// prtie
		_sym.prlos = _iter_vars.prlos;													// prlos
	LeaveCriticalSection(&cs_symbols);	

	// Start/restart prwin thread on these conditions:
	// - changed f$P (nopponents)
	// - changed nit
	// - changed br
	// - changed player cards
	// - changed common cards
	need_recalc = false;
	if ((int) _sym.nopponents != _iter_params.f$p ||
		(int) _sym.nit != _iter_params.nit ||
		(int) _sym.br != _iter_params.br)
	{
		need_recalc = true;
	}

	if (p_scraper->card_player(_sym.userchair, 0) != _iter_params.pcard[0] || 
		p_scraper->card_player(_sym.userchair, 1) != _iter_params.pcard[1])
		need_recalc = true;

	for (i=0; i<=4; i++)
	{
		if (p_scraper->card_common(i) != _iter_params.ccard[i])
			need_recalc = true;
	}

	// restart iterator thread
	if (_sym.nit==0)
	{
		EnterCriticalSection(&cs_iterator);
		_iter_vars.iterator_thread_complete = true;
		LeaveCriticalSection(&cs_iterator);
	}
	else if (need_recalc)
	{
		if (p_iterator_thread)
			delete p_iterator_thread;

		p_iterator_thread = new CIteratorThread;
	}
}

void CSymbols::CalcPlayersFriendsOpponents(void)
{
	int				i = 0;
	double			lastbet = 0.;
	bool			sblindfound = false, bblindfound = false, found_userchair = false;

	EnterCriticalSection(&cs_symbols);

		for (i=_sym.dealerchair+1; i<=_sym.dealerchair+p_tablemap->s$items()->num_chairs; i++)
		{
			double p_bet = p_scraper->player_bet(i%p_tablemap->s$items()->num_chairs);

			if (p_bet > lastbet)
			{
				lastbet = p_bet;
				_sym.raischair = i%p_tablemap->s$items()->num_chairs;									// raischair
			}

			if (!sblindfound && p_bet<=_sym.sblind)
			{
				sblindfound = true;
				_sym.playersblindbits = (int) _sym.playersblindbits | (1<<(i%p_tablemap->s$items()->num_chairs));  // playersblindbits

				if (_user_chair_confirmed && (i%p_tablemap->s$items()->num_chairs) != _sym.userchair)
				{
					_sym.opponentsblindbits =
						(int) _sym.opponentsblindbits | (1<<(i%p_tablemap->s$items()->num_chairs));		// opponentsblindbits
				}

				if (_user_chair_confirmed && (i%p_tablemap->s$items()->num_chairs) == _sym.userchair)
				{
					_sym.friendsblindbits = (int) _sym.friendsblindbits | (1<<(i%p_tablemap->s$items()->num_chairs));  // friendsblindbits
				}
			}

			else if (!bblindfound && p_bet<=_sym.bblind)
			{
				bblindfound = true;
				_sym.bblindbits = 0; //prwin change
				_sym.playersblindbits = (int) _sym.playersblindbits | (1<<(i%p_tablemap->s$items()->num_chairs));	// playersblindbits
				if (_user_chair_confirmed && (i%p_tablemap->s$items()->num_chairs) != _sym.userchair)
				{
					_sym.opponentsblindbits =  (int) _sym.opponentsblindbits | (1<<(i%p_tablemap->s$items()->num_chairs));	// opponentsblindbits
					_sym.bblindbits =  //prwin change
						(int) _sym.bblindbits | (1<<(i%p_tablemap->s$items()->num_chairs));		// big blind bit
				}
			}
		}

		_sym.nplayersblind = bitcount(_sym.playersblindbits);								// nplayersblind
		_sym.nopponentsblind = bitcount(_sym.opponentsblindbits);							// nopponentsblind
		_sym.nfriendsblind = bitcount(_sym.friendsblindbits);								// nfriendsblind

		for (i=0; i<p_tablemap->s$items()->num_chairs; i++)
		{
			if (p_scraper->card_player(i, 0) != CARD_NOCARD && p_scraper->card_player(i, 1) != CARD_NOCARD)
			{
				_sym.playersdealtbits = (int) _sym.playersdealtbits | (1<<i);				// playersdealtbits
				_sym.playersplayingbits = (int) _sym.playersplayingbits | (1<<i);			// playersplayingbits

				if (_user_chair_confirmed && i!=_sym.userchair)
				{
					_sym.opponentsdealtbits = (int) _sym.opponentsdealtbits | (1<<i);		// opponentsdealtbits
					_sym.opponentsplayingbits = (int) _sym.opponentsplayingbits | (1<<i);	// opponentsplayingbits
				}

				if (_user_chair_confirmed && i==_sym.userchair)
				{
					_sym.friendsdealtbits = (int) _sym.friendsdealtbits | (1<<i);			// friendsdealtbits
					_sym.friendsplayingbits = (int) _sym.friendsplayingbits | (1<<i);		// friendsplayingbits
				}
			}

			if (p_scraper->IsStringSeated(p_scraper->seated(i)))
			{
				_sym.playersseatedbits = (int) _sym.playersseatedbits | (1<<i);			// playersseatedbits

				if (_user_chair_confirmed && i!=_sym.userchair)
				{
					_sym.opponentsseatedbits = (int) _sym.opponentsseatedbits | (1<<i);	// opponentsseatedbits
				}
				if (_user_chair_confirmed && i==_sym.userchair)
				{
					_sym.friendsseatedbits = (int) _sym.friendsseatedbits | (1<<i);		// friendsseatedbits
				}
			}

			if (p_scraper->IsStringActive(p_scraper->active(i)))
			{
				_sym.playersactivebits = (int) _sym.playersactivebits | (1<<i);			// playersactivebits
				if (_user_chair_confirmed && i!=_sym.userchair)
				{
					_sym.opponentsactivebits = (int) _sym.opponentsactivebits | (1<<i);	// opponentsactivebits
				}
				if (_user_chair_confirmed && i==_sym.userchair)
				{
					_sym.friendsactivebits = (int) _sym.friendsactivebits | (1<<i);		// friendsactivebits
				}
			}
		}

		_sym.nplayersseated = bitcount(_sym.playersseatedbits);							// nplayersseated
		_sym.nplayersactive = bitcount(_sym.playersactivebits);							// nplayersactive
		_sym.nplayersdealt = bitcount(_sym.playersdealtbits);								// nplayersdealt
		_sym.nplayersplaying = bitcount(_sym.playersplayingbits);							// nplayersplaying
		_sym.nopponentsseated = bitcount(_sym.opponentsseatedbits);						// nopponentsseated
		_sym.nopponentsactive = bitcount(_sym.opponentsactivebits);						// nopponentsactive
		_sym.nopponentsdealt = bitcount(_sym.opponentsdealtbits);							// nopponentsdealt
		_sym.nopponentsplaying = bitcount(_sym.opponentsplayingbits);						// nopponentsplaying
		_sym.nfriendsseated = bitcount(_sym.friendsseatedbits);							// nfriendsseated
		_sym.nfriendsactive = bitcount(_sym.friendsactivebits);							// nfriendsactive
		_sym.nfriendsdealt = bitcount(_sym.friendsdealtbits);								// nfriendsdealt
		_sym.nfriendsplaying = bitcount(_sym.friendsplayingbits);							// nfriendsplaying

		found_userchair = false;
		lastbet = 0;
		for (i=_sym.dealerchair+1; i<=_sym.dealerchair+p_tablemap->s$items()->num_chairs && _user_chair_confirmed; i++)
		{
			double p_bet = p_scraper->player_bet(i%p_tablemap->s$items()->num_chairs);

			if ((i%p_tablemap->s$items()->num_chairs) == _sym.userchair)
				found_userchair = true;

			else
			{
				if ((int) _sym.playersdealtbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
				{
					if (!found_userchair)
						_sym.nchairsdealtright += 1;										// nchairsdealtright

					else
						_sym.nchairsdealtleft += 1;										// nchairsdealtleft

				}
			}

			if (p_bet > lastbet && p_bet != 0)
			{
				lastbet = p_bet;

				if ((i%p_tablemap->s$items()->num_chairs) != _sym.userchair)
					_sym.nopponentsraising += 1;											// nopponentsraising
			}

			else if ((i%p_tablemap->s$items()->num_chairs) != _sym.userchair &&
					 p_bet == lastbet && lastbet != 0)
			{
				_sym.nopponentscalling += 1;												// nopponentscalling
			}

			if ((i%p_tablemap->s$items()->num_chairs) != _sym.userchair &&	p_bet > 0)
			{
				_sym.nopponentsbetting += 1;												// nopponentsbetting
			}

			if ((i%p_tablemap->s$items()->num_chairs) != _sym.userchair &&
				(int) _sym.playersdealtbits & (1<<(i%p_tablemap->s$items()->num_chairs)) &&
				(p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 0) == CARD_NOCARD ||
				 p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 1) == CARD_NOCARD))
			{
				_sym.nopponentsfolded += 1;												// nopponentsfolded
			}

			if ((i%p_tablemap->s$items()->num_chairs) != _sym.userchair &&
				p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 0) != CARD_NOCARD &&
				p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 1) != CARD_NOCARD &&
				p_bet == 0)
			{
				_sym.nopponentschecking += 1;											// nopponentschecking
			}

			if (p_bet < _sym.currentbet[(int) _sym.raischair] &&
				p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 0) != CARD_NOCARD &&
				p_scraper->card_player(i%p_tablemap->s$items()->num_chairs, 1) != CARD_NOCARD)
			{
				_sym.nplayerscallshort += 1;												// nplayerscallshort
			}
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcRoundsPositions(void) 
{
	EnterCriticalSection(&cs_symbols);	

		int			i = 0;

		for (i=_sym.dealerchair+1; i<=_sym.dealerchair+p_tablemap->s$items()->num_chairs && (i%p_tablemap->s$items()->num_chairs)!=_sym.userchair; i++)
		{
			if ((int)_sym.playersplayingbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
				_sym.betposition+=1;													// betposition

			if ((int)_sym.playersdealtbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
				_sym.dealposition+=1;												// dealposition
		}

		for (i=_sym.raischair+1; i<=_sym.raischair+p_tablemap->s$items()->num_chairs && (i%p_tablemap->s$items()->num_chairs)!=_sym.userchair; i++)
		{
			if ((int)_sym.playersplayingbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
				_sym.callposition+=1;												// callposition
		}

		for (i=_sym.dealerchair+1; i<=_sym.dealerchair+p_tablemap->s$items()->num_chairs && (i%p_tablemap->s$items()->num_chairs)!=_sym.userchair; i++)
		{
			if ((int)_sym.playersseatedbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
				_sym.seatposition+=1;												// seatposition
		}

		for (i=_sym.dealerchair+1; i<=_sym.dealerchair+p_tablemap->s$items()->num_chairs && (i%p_tablemap->s$items()->num_chairs)!=_sym.raischair; i++)
		{
			if ((int)_sym.playersplayingbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
				_sym.betpositionrais+=1;												// betpositionrais

			if ((int)_sym.playersdealtbits & (1<<(i%p_tablemap->s$items()->num_chairs)))
				_sym.dealpositionrais+=1;											// dealpositionrais
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcPokerValues(void)
{
	int				i = 0;
	CardMask		Cards = {0};
	int				nCards = 0;
	HandVal			handval = 0;
	double			dummy = 0.;
	int				hi_common_rank = 0, lo_common_rank = 0;

	// poker constants
	_sym.hicard			= 0x00000001;
	_sym.onepair			= 0x01000000;
	_sym.twopair			= 0x02000000;
	_sym.threeofakind	= 0x04000000;
	_sym.straight		= 0x08000000;
	_sym.flush			= 0x10000000;
	_sym.fullhouse		= 0x20000000;
	_sym.fourofakind		= 0x40000000;
	_sym.straightflush	= 0x80000000;
	_sym.royalflush		= 0x800EDCBA;
	_sym.fiveofakind		= 0xFF000000;

	///////////////////////////////////////////////////////////////////
	// pokerval
	nCards = 0;
	CardMask_RESET(Cards);
	for (i=0; i<=1; i++)
	{
		// player cards
		if (p_scraper->card_player(_sym.userchair, i) != CARD_BACK &&
			p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD)
		{
			CardMask_SET(Cards, p_scraper->card_player(_sym.userchair, i));
			nCards++;
		}
	}

	hi_common_rank = -1;
	lo_common_rank = 99;
	for (i=0; i<=4; i++)
	{
		// common cards
		if (p_scraper->card_common(i) != CARD_BACK &&
			p_scraper->card_common(i) != CARD_NOCARD)
		{
			CardMask_SET(Cards, p_scraper->card_common(i));
			nCards++;
			if ((int) StdDeck_RANK(p_scraper->card_common(i)) > hi_common_rank)
				hi_common_rank = StdDeck_RANK(p_scraper->card_common(i));

			if ((int) StdDeck_RANK(p_scraper->card_common(i)) < lo_common_rank)
				lo_common_rank = StdDeck_RANK(p_scraper->card_common(i));
		}
	}

	handval = Hand_EVAL_N(Cards, nCards);

	EnterCriticalSection(&cs_symbols);

		_sym.pcbits = 0;
		_sym.pokerval = CalcPokerval(handval, nCards, &_sym.pcbits,					// pcbits
									 p_scraper->card_player(_sym.userchair, 0), 
									 p_scraper->card_player(_sym.userchair, 1));		// pokerval

		_sym.npcbits = bitcount(_sym.pcbits);											// npcbits
		_phandval[(int)_sym.br-1] = (int)_sym.pokerval&0xff000000; //change from previous handval assignment 2008-03-02
		if (_sym.br>1 &&	_phandval[(int)_sym.br-1] > _phandval[(int)_sym.br-2])
			_sym.ishandup = 1;														// ishandup

		if (HandVal_HANDTYPE(handval) == HandType_STFLUSH &&
				StdDeck_RANK(HandVal_TOP_CARD(handval)) == Rank_ACE)
		{
			_sym.isroyalflush = 1; 													// isroyalflush
		}

		else if (HandVal_HANDTYPE(handval) == HandType_STFLUSH)
		{
			_sym.isstraightflush = 1; 												// isstraightflush
		}

		else if (HandVal_HANDTYPE(handval) == HandType_QUADS)
		{
			_sym.isfourofakind = 1; 													// isfourofakind
		}

		else if (HandVal_HANDTYPE(handval) == HandType_FULLHOUSE)
		{
			_sym.isfullhouse = 1; 													// isfullhouse
		}

		else if (HandVal_HANDTYPE(handval) == HandType_FLUSH)
		{
			_sym.isflush = 1;														// isflush
			// ishiflush ->
			// If we have a pocket Ace in our flush
			if (StdDeck_RANK(HandVal_TOP_CARD(handval)) == 12 && ((int)_sym.pcbits & 0x10))
			{
				_sym.ishiflush = 1;
			}
			// If we have a pocket King, and it's the second best card in our flush
			else if (StdDeck_RANK(HandVal_SECOND_CARD(handval)) == 11 && ((int)_sym.pcbits & 0x8))
			{
				_sym.ishiflush = 1;
			}
			// If we have a pocket Queen, and it's the third best card in our flush
			else if (StdDeck_RANK(HandVal_THIRD_CARD(handval)) == 10 && ((int)_sym.pcbits & 0x4))
			{
				_sym.ishiflush = 1;
			}
			// If we have a pocket Jack, and it's the fourth best card in our flush
			else if (StdDeck_RANK(HandVal_FOURTH_CARD(handval)) == 9 && ((int)_sym.pcbits & 0x2))
			{
				_sym.ishiflush = 1;
			}																		// <- ishiflush
		}

		else if (HandVal_HANDTYPE(handval) == HandType_STRAIGHT)
		{
			_sym.isstraight = 1; 													// isstraight

			// If it is an Ace high straight
			if (StdDeck_RANK(HandVal_TOP_CARD(handval)) == 12 )
			{
				_sym.ishistraight = 1;												// ishistraight
			}
		}

		else if (HandVal_HANDTYPE(handval) == HandType_TRIPS)
		{
			_sym.isthreeofakind = 1; 												// isthreeofakind
		}

		else if (HandVal_HANDTYPE(handval) == HandType_TWOPAIR)
		{
			_sym.istwopair = 1; 														// istwopair
		}

		else if (HandVal_HANDTYPE(handval) == HandType_ONEPAIR)
		{
			_sym.isonepair = 1; 														// isonepair

			// hi lo med pair
			if (nCards == 2)
			{
				_sym.ishipair = 1;													// ishipair
			}
			else if (nCards >= 5)
			{
				if ((int) StdDeck_RANK(HandVal_TOP_CARD(handval)) >= hi_common_rank)
				{
					_sym.ishipair = 1;												// ishipair
				}
				else if ((int) StdDeck_RANK(HandVal_TOP_CARD(handval)) < hi_common_rank &&
						 (int) StdDeck_RANK(HandVal_TOP_CARD(handval)) > lo_common_rank)
				{
					_sym.ismidpair = 1;												// ismidpair
				}
				else
				{
					_sym.islopair = 1;												// islopair
				}
			}
		}
		else if (HandVal_HANDTYPE(handval) == HandType_NOPAIR)
		{
			_sym.ishicard = 1; 														// ishicard
		}
		_sym.isfiveofakind = 0;														// isfiveofakind

	LeaveCriticalSection(&cs_symbols);


	///////////////////////////////////////////////////////////////////
	// pokervalplayer
	nCards = 0;
	CardMask_RESET(Cards);
	for (i=0; i<=1; i++)
	{
		// player cards
		if (p_scraper->card_player(_sym.userchair, i) != CARD_BACK && 
			p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD &&
			!CardMask_CARD_IS_SET(Cards, p_scraper->card_player(_sym.userchair, i)) )
		{
			CardMask_SET(Cards, p_scraper->card_player(_sym.userchair, i));
			nCards++;
		}
	}
	handval = Hand_EVAL_N(Cards, nCards);

	EnterCriticalSection(&cs_symbols);	
	_sym.pokervalplayer = CalcPokerval(handval, nCards, &dummy, CARD_NOCARD, CARD_NOCARD); // pokervalplayer
	LeaveCriticalSection(&cs_symbols);


	///////////////////////////////////////////////////////////////////
	// pokervalcommon
	nCards = 0;
	CardMask_RESET(Cards);
	for (i=0; i<=4; i++)
	{
		// common cards
		if (p_scraper->card_common(i) != CARD_BACK && 
			p_scraper->card_common(i) != CARD_NOCARD &&
			!CardMask_CARD_IS_SET(Cards, p_scraper->card_common(i)) )
		{
			CardMask_SET(Cards, p_scraper->card_common(i));
			nCards++;
		}
	}
	handval = Hand_EVAL_N(Cards, nCards);

	EnterCriticalSection(&cs_symbols);	
		_sym.pokervalcommon = CalcPokerval(handval, nCards, &dummy, CARD_NOCARD, CARD_NOCARD);	// pokervalcommon
		_chandval[(int)_sym.br-1] = (int)_sym.pokervalcommon&0xff000000; //change from previous handval assignment 2008-03-02
		if (_sym.br>1 &&	_chandval[(int)_sym.br-1] > _chandval[(int)_sym.br-2])
		{
			_sym.ishandupcommon = 1;															// ishandupcommon
		}
	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcUnknownCards(void)
{
	int				i = 0;
	CardMask		stdCards = {0}, commonCards = {0};
	int				nstdCards = 0, ncommonCards = 0;
	HandVal			handval_std = 0, handval_std_plus1 = 0, handval_common_plus1 = 0;
	double			dummy = 0.;

	CardMask_RESET(stdCards);
	CardMask_RESET(commonCards);

	for (i=0; i<=1; i++)
	{
		// player cards
		if (p_scraper->card_player(_sym.userchair, i) != CARD_BACK && 
			p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD)
		{
			CardMask_SET(stdCards, p_scraper->card_player(_sym.userchair, i));
			nstdCards++;
		}
	}
	for (i=0; i<=4; i++)
	{
		// common cards
		if (p_scraper->card_common(i) != CARD_BACK &&
			p_scraper->card_common(i) != CARD_NOCARD)
		{
			CardMask_SET(stdCards, p_scraper->card_common(i));
			nstdCards++;
			CardMask_SET(commonCards, p_scraper->card_common(i));
			ncommonCards++;
		}
	}

	EnterCriticalSection(&cs_symbols);	

		_sym.ncardsknown = nstdCards;													// ncardsknown
		_sym.ncardsunknown = 52 - _sym.ncardsknown;										// ncardsunknown
		handval_std = Hand_EVAL_N(stdCards, nstdCards);

		if (_sym.br<4 && _user_chair_confirmed)
		{
			// iterate through every unseen card and see what happens to our handvals
			for (i=0; i<=51; i++)
			{
				if (i!=p_scraper->card_player(_sym.userchair, 0) && 
					i!=p_scraper->card_player(_sym.userchair, 1) &&
					i!=p_scraper->card_common(0) &&
					i!=p_scraper->card_common(1) &&
					i!=p_scraper->card_common(2) &&
					i!=p_scraper->card_common(3) &&
					i!=p_scraper->card_common(4))
				{

					CardMask_SET(stdCards, i);
					handval_std_plus1 = Hand_EVAL_N(stdCards, nstdCards+1);
					CardMask_UNSET(stdCards, i);

					CardMask_SET(commonCards, i);
					handval_common_plus1 = Hand_EVAL_N(commonCards, ncommonCards+1);
					CardMask_UNSET(commonCards, i);

					if (HandVal_HANDTYPE(handval_std_plus1) > HandVal_HANDTYPE(handval_std) &&
							CalcPokerval(handval_std_plus1, nstdCards+1, &dummy, CARD_NOCARD, CARD_NOCARD) > _sym.pokerval &&
							HandVal_HANDTYPE(handval_std_plus1) > HandVal_HANDTYPE(handval_common_plus1))
					{
						_sym.nouts++;													// nouts
					}
					if (CalcPokerval(handval_common_plus1, ncommonCards+1, &dummy, CARD_NOCARD, CARD_NOCARD) > _sym.pokerval)
					{
						_sym.ncardsbetter++;												// ncardsbetter
					}
				}
			}
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcHandTests(void)
{
	int		i = 0;

	EnterCriticalSection(&cs_symbols);

		for (i=0; i<=1; i++)
		{
			if (p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD && 
				p_scraper->card_player(_sym.userchair, i) != CARD_BACK)
			{
				_sym.$$pc[i] = ((StdDeck_RANK(p_scraper->card_player(_sym.userchair, i))+2)<<4) |		//$$pcx
							  (StdDeck_SUIT(p_scraper->card_player(_sym.userchair, i)) == StdDeck_Suit_CLUBS ? WH_SUIT_CLUBS :
							   StdDeck_SUIT(p_scraper->card_player(_sym.userchair, i)) == StdDeck_Suit_DIAMONDS ? WH_SUIT_DIAMONDS :
							   StdDeck_SUIT(p_scraper->card_player(_sym.userchair, i)) == StdDeck_Suit_HEARTS ? WH_SUIT_HEARTS :
							   StdDeck_SUIT(p_scraper->card_player(_sym.userchair, i)) == StdDeck_Suit_SPADES ? WH_SUIT_SPADES : 0) ;
				_sym.$$pr[i] = ((int)_sym.$$pc[i] & 0xf0) >> 4;							// $$prx
				_sym.$$ps[i] = (int)_sym.$$pc[i] & 0x0f;									// $$psx
			}
		}

		for (i=0; i<=4; i++)
		{
			if (p_scraper->card_common(i) != CARD_NOCARD && p_scraper->card_common(i) != CARD_BACK)
			{
				_sym.$$cc[i] = ((StdDeck_RANK(p_scraper->card_common(i))+2)<<4) |			// $$ccx
							  (StdDeck_SUIT(p_scraper->card_common(i)) == StdDeck_Suit_CLUBS ? WH_SUIT_CLUBS :
							   StdDeck_SUIT(p_scraper->card_common(i)) == StdDeck_Suit_DIAMONDS ? WH_SUIT_DIAMONDS :
							   StdDeck_SUIT(p_scraper->card_common(i)) == StdDeck_Suit_HEARTS ? WH_SUIT_HEARTS :
							   StdDeck_SUIT(p_scraper->card_common(i)) == StdDeck_Suit_SPADES ? WH_SUIT_SPADES : 0) ;
				_sym.$$cr[i] = ((int)_sym.$$cc[i] & 0xf0) >> 4;							// $$crx
				_sym.$$cs[i] = (int)_sym.$$cc[i] & 0x0f;									// $$csx
			}
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcPocketTests(void)
{
	EnterCriticalSection(&cs_symbols);

		if (p_scraper->card_player(_sym.userchair, 0) != CARD_NOCARD && 
			p_scraper->card_player(_sym.userchair, 0) != CARD_BACK &&
			p_scraper->card_player(_sym.userchair, 1) != CARD_NOCARD && 
			p_scraper->card_player(_sym.userchair, 1) != CARD_BACK)
		{
			if (StdDeck_RANK(p_scraper->card_player(_sym.userchair, 0)) == 
				StdDeck_RANK(p_scraper->card_player(_sym.userchair, 1)))
			{
				_sym.ispair = 1;															// ispair
			}

			if (StdDeck_SUIT(p_scraper->card_player(_sym.userchair, 0)) == 
				StdDeck_SUIT(p_scraper->card_player(_sym.userchair, 1)))
			{
				_sym.issuited = 1;														// issuited
			}

			if (StdDeck_RANK(p_scraper->card_player(_sym.userchair, 0)) - 
				StdDeck_RANK(p_scraper->card_player(_sym.userchair, 1)) == 1 ||
				StdDeck_RANK(p_scraper->card_player(_sym.userchair, 1)) - 
				StdDeck_RANK(p_scraper->card_player(_sym.userchair, 0)) == 1 )
			{
				_sym.isconnector = 1;													// isconnector
			}
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcListTests(void)
{
	int				i = 0, N = 0;
	int				listnum = 0;
	int				tokpos = 0;
	unsigned int	c0 = 0, c1 = 0;

	if (StdDeck_RANK(p_scraper->card_player(_sym.userchair, 0)) >= 
		StdDeck_RANK(p_scraper->card_player(_sym.userchair, 1)))
	{
		c0 = p_scraper->card_player(_sym.userchair, 0);
		c1 = p_scraper->card_player(_sym.userchair, 1);
	}
	else
	{
		c0 = p_scraper->card_player(_sym.userchair, 1);
		c1 = p_scraper->card_player(_sym.userchair, 0);
	}


	N = p_formula->formula()->mHandList.GetSize();
	for (i=0; i<N; i++)
	{
		listnum = atoi(p_formula->formula()->mHandList[i].list.Mid(4).GetString());

		if ((StdDeck_SUIT(c0)==StdDeck_SUIT(c1) && p_formula->formula()->inlist[listnum][StdDeck_RANK(c0)][StdDeck_RANK(c1)]) ||
			(StdDeck_SUIT(c0)!=StdDeck_SUIT(c1) && p_formula->formula()->inlist[listnum][StdDeck_RANK(c1)][StdDeck_RANK(c0)]))
		{
			EnterCriticalSection(&cs_symbols);
				_sym.islist[listnum] = 1;											// islistxx
				if (listnum < _sym.nlistmin || _sym.nlistmin == -1)
				{
					_sym.nlistmin = listnum;											// nlistmin
				}
				if (listnum > _sym.nlistmax)
				{
					_sym.nlistmax = listnum;											// nlistmax
				}
			LeaveCriticalSection(&cs_symbols);
		}
	}

	// islistcall, islistrais, islistalli, isemptylistcall, isemptylistrais, isemptylistalli
	N = p_formula->formula()->mHandList.GetSize();

	EnterCriticalSection(&cs_symbols);
	_sym.isemptylistcall = 1;
	_sym.isemptylistrais = 1;
	_sym.isemptylistalli = 1;
	LeaveCriticalSection(&cs_symbols);

	for (i=0; i<N; i++)
	{
		listnum = atoi(p_formula->formula()->mHandList[i].list.Mid(4).GetString());

		if (listnum == 0)
		{
			EnterCriticalSection(&cs_symbols);
			if (_sym.islist[listnum])
			{
				_sym.islistcall = 1;													// islistcall
			}
			tokpos = 0;
			p_formula->formula()->mHandList[i].list_text.Tokenize(" \t", tokpos);
			if (tokpos != -1)
			{
				_sym.isemptylistcall = 0;											// isemptylistcall
			}
			LeaveCriticalSection(&cs_symbols);
		}

		else if (listnum == 1)
		{
			EnterCriticalSection(&cs_symbols);
			if (_sym.islist[listnum])
			{
				_sym.islistrais = 1;													// islistrais
			}
			tokpos = 0;
			p_formula->formula()->mHandList[i].list_text.Tokenize(" \t", tokpos);
			if (tokpos != -1)
			{
				_sym.isemptylistrais = 0;											// isemptylistrais
			}
			LeaveCriticalSection(&cs_symbols);
		}

		else if (listnum == 7)
		{
			EnterCriticalSection(&cs_symbols);
			if (_sym.islist[listnum])
			{
				_sym.islistalli = 1;													// islistalli
			}
			tokpos = 0;
			p_formula->formula()->mHandList[i].list_text.Tokenize(" \t", tokpos);
			if (tokpos != -1)
			{
				_sym.isemptylistalli = 0;											// isemptylistalli
			}
			LeaveCriticalSection(&cs_symbols);
		}
	}
}

void CSymbols::CalcNhands(void)
{
	CardMask		plCards = {0}, comCards = {0}, oppCards = {0}, playerEvalCards = {0}, opponentEvalCards = {0};
	HandVal			hv_player = 0, hv_opponent = 0;
	unsigned int	pl_pokval = 0, opp_pokval = 0;
	int				i = 0, j = 0;
	double			dummy = 0;
	int				nplCards = 0, ncomCards = 0;

	// player cards
	CardMask_RESET(plCards);
	nplCards = 0;
	for (i=0; i<=1; i++)
	{
		if (p_scraper->card_player(_sym.userchair, i) != CARD_BACK && 
			p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD)
		{
			CardMask_SET(plCards, p_scraper->card_player(_sym.userchair, i));
			nplCards++;
		}
	}

	// common cards
	CardMask_RESET(comCards);
	ncomCards = 0;
	for (i=0; i<=4; i++)
	{
		if (p_scraper->card_common(i) != CARD_BACK && 
			p_scraper->card_common(i) != CARD_NOCARD)
		{
			CardMask_SET(comCards, p_scraper->card_common(i));
			ncomCards++;
		}
	}


	// player/common cards and pokerval
	CardMask_OR(playerEvalCards, plCards, comCards);
	hv_player = Hand_EVAL_N(playerEvalCards, nplCards+ncomCards);
	pl_pokval = CalcPokerval(hv_player, nplCards+ncomCards, &dummy, CARD_NOCARD, CARD_NOCARD);


	for (i=0; i<=50; i++)
	{
		for (j=i+1; j<=51; j++)
		{
			if (!CardMask_CARD_IS_SET(plCards, i) &&
					!CardMask_CARD_IS_SET(plCards, j) &&
					!CardMask_CARD_IS_SET(comCards, i) &&
					!CardMask_CARD_IS_SET(comCards, j))
			{

				// opponent cards
				CardMask_RESET(oppCards);
				CardMask_SET(oppCards, i);
				CardMask_SET(oppCards, j);

				CardMask_OR(opponentEvalCards, oppCards, comCards);
				hv_opponent = Hand_EVAL_N(opponentEvalCards, 2+ncomCards);
				opp_pokval = CalcPokerval(hv_opponent, 2+ncomCards, &dummy, CARD_NOCARD, CARD_NOCARD);

				EnterCriticalSection(&cs_symbols);
					if (pl_pokval > opp_pokval)
						_sym.nhandslo++;

					else if (pl_pokval < opp_pokval)
						_sym.nhandshi++;

					else
						_sym.nhandsti++;
				LeaveCriticalSection(&cs_symbols);
			}
		}
	}

	EnterCriticalSection(&cs_symbols);	
		_sym.nhands = _sym.nhandshi + _sym.nhandsti + _sym.nhandslo;

		_sym.prwinnow = pow((_sym.nhandslo/_sym.nhands), _sym.nopponents);

		_sym.prlosnow = 1 - pow(((_sym.nhandslo + _sym.nhandsti)/_sym.nhands), _sym.nopponents);
	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcHandrank(void)
{
	char		cardstr[10] = {0};
	int			i = 0, count = 0;

	// Get a string containing the players' current cards
	GetCardstring(cardstr, 
				   p_scraper->card_player(_sym.userchair, 0), 
				   p_scraper->card_player(_sym.userchair, 1));


	EnterCriticalSection(&cs_symbols);

		// if nopponents<1 or >9 then default to a sane value
		if (_sym.nopponents<1)
			count = 1;

		else if (_sym.nopponents>9)
			count = 9;

		else
			count = (int) _sym.nopponents;

		for (i=0; i<169; i++)
		{
			if (strcmp(cardstr, handrank169[count-1][i])==0)
			{
				_sym.handrank169 = i+1;													// handrank169
				_sym.handrank2652 = handrank2652[count-1][i];							// handrank2652
				i = 170;
			}
		}
		_sym.handrank1326 = _sym.handrank2652/2;											// handrank1326
		_sym.handrank1000 = 1000*_sym.handrank2652/2652;									// handrank1000
		_sym.handrankp = 2652.0 / (1.0+(double)count);									// handrankp
		if (prefs.sym_handrank_value() == "169")
			_sym.handrank = _sym.handrank169;

		else if (prefs.sym_handrank_value() == "2652")
			_sym.handrank = _sym.handrank2652;

		else if (prefs.sym_handrank_value() == "1326")
			_sym.handrank = _sym.handrank1326;

		else if (prefs.sym_handrank_value() == "1000")
			_sym.handrank = _sym.handrank1000;

		else if (prefs.sym_handrank_value() == "p")
			_sym.handrank = _sym.handrankp;												// handrank

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcFlushesStraightsSets(void)
{
	int				i = 0, j = 0, n = 0;
	CardMask		plCards = {0}, comCards = {0};
	CardMask		heartsCards = {0}, diamondsCards = {0}, clubsCards = {0}, spadesCards = {0}, suittestCards = {0};
	int				max = 0;
	unsigned int	strbits = 0;

	// Set up some suit masks
	CardMask_RESET(heartsCards);
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_2, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_3, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_4, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_5, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_6, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_7, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_8, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_9, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_KING, Suit_HEARTS));
	CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_HEARTS));

	CardMask_RESET(diamondsCards);
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_2, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_3, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_4, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_5, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_6, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_7, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_8, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_9, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_KING, Suit_DIAMONDS));
	CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_DIAMONDS));

	CardMask_RESET(spadesCards);
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_2, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_3, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_4, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_5, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_6, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_7, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_8, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_9, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_KING, Suit_SPADES));
	CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_SPADES));

	CardMask_RESET(clubsCards);
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_2, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_3, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_4, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_5, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_6, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_7, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_8, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_9, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_KING, Suit_CLUBS));
	CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_CLUBS));

	// player cards
	CardMask_RESET(plCards);
	for (i=0; i<=1; i++)
	{
		if (p_scraper->card_player(_sym.userchair, i) != CARD_BACK && 
			p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD)
		{
			CardMask_SET(plCards, p_scraper->card_player(_sym.userchair, i));
		}
	}

	// common cards
	CardMask_RESET(comCards);
	for (i=0; i<=4; i++)
	{
		if (p_scraper->card_common(i) != CARD_BACK && 
			p_scraper->card_common(i) != CARD_NOCARD)
		{
			CardMask_SET(comCards, p_scraper->card_common(i));
			CardMask_SET(plCards, p_scraper->card_common(i));
		}
	}

	EnterCriticalSection(&cs_symbols);

		// nsuited, tsuit
		max = 0;
		CardMask_AND(suittestCards, plCards, spadesCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuit = WH_SUIT_SPADES;
		}
		CardMask_AND(suittestCards, plCards, heartsCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuit = WH_SUIT_HEARTS;
		}
		CardMask_AND(suittestCards, plCards, diamondsCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuit = WH_SUIT_DIAMONDS;
		}
		CardMask_AND(suittestCards, plCards, clubsCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuit = WH_SUIT_CLUBS;													// tsuit
		}
		_sym.nsuited = max;																// nsuited

		// nsuitedcommon, tsuitcommon
		max = 0;
		CardMask_AND(suittestCards, comCards, spadesCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuitcommon = WH_SUIT_SPADES;
		}
		CardMask_AND(suittestCards, comCards, heartsCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuitcommon = WH_SUIT_HEARTS;
		}
		CardMask_AND(suittestCards, comCards, diamondsCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuitcommon = WH_SUIT_DIAMONDS;
		}
		CardMask_AND(suittestCards, comCards, clubsCards);
		n = StdDeck_numCards(suittestCards);
		if ( n>max && n>0)
		{
			max = n;
			_sym.tsuitcommon = WH_SUIT_CLUBS;											// tsuitcommon
		}
		_sym.nsuitedcommon = max;														// nsuitedcommon

		// nranked, trank
		max = 0;
		for (i=12; i>=0; i--)
		{
			n = CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*0)) +
				CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*1)) +
				CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*2)) +
				CardMask_CARD_IS_SET(plCards, i+(Rank_COUNT*3));
			if (n>max && n>0)
			{
				max = n;
				_sym.trank = i+2;														// trank
			}
		}
		_sym.nranked = max;																// nranked

		// nrankedcommon, trankcommon
		max = 0;
		for (i=12; i>=0; i--)
		{
			n = CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*0)) +
				CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*1)) +
				CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*2)) +
				CardMask_CARD_IS_SET(comCards, i+(Rank_COUNT*3));
			if (n>max && n>0)
			{
				max = n;
				_sym.trankcommon = i+2;													// trankcommon
			}
		}
		_sym.nrankedcommon = max;														// nrankedcommon

		// nstraight, nstraightfill
		strbits = 0;
		for (i=0; i<Rank_COUNT; i++)
		{
			if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*0)+i) ||
				CardMask_CARD_IS_SET(plCards, (Rank_COUNT*1)+i) ||
				CardMask_CARD_IS_SET(plCards, (Rank_COUNT*2)+i) ||
				CardMask_CARD_IS_SET(plCards, (Rank_COUNT*3)+i))
			{
				strbits |= (1<<(i+2));
			}
		}
		if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*0)+Rank_ACE) ||
			CardMask_CARD_IS_SET(plCards, (Rank_COUNT*1)+Rank_ACE) ||
			CardMask_CARD_IS_SET(plCards, (Rank_COUNT*2)+Rank_ACE) ||
			CardMask_CARD_IS_SET(plCards, (Rank_COUNT*3)+Rank_ACE))
		{
			strbits |= (1<<1);
		}

		for (i=10; i>=1; i--)
		{
			if (((strbits>>i)&0x1f) == 0x1f)
			{
				_sym.nstraight = (_sym.nstraight<5 ? 5 : _sym.nstraight);
			}
			else if (((strbits>>i)&0x1e) == 0x1e ||
					 ((strbits>>i)&0x0f) == 0x0f)
			{
				_sym.nstraight = (_sym.nstraight<4 ? 4 : _sym.nstraight);
			}
			else if (((strbits>>i)&0x1c) == 0x1c ||
					 ((strbits>>i)&0x0e) == 0x0e ||
					 ((strbits>>i)&0x07) == 0x07)
			{
				_sym.nstraight = (_sym.nstraight<3 ? 3 : _sym.nstraight);
			}
			else if (((strbits>>i)&0x18) == 0x18 ||
					 ((strbits>>i)&0x0c) == 0x0c ||
					 ((strbits>>i)&0x06) == 0x06 ||
					 ((strbits>>i)&0x3) == 0x03)
			{
				_sym.nstraight = (_sym.nstraight<2 ? 2 : _sym.nstraight);
			}
			else {
				_sym.nstraight = (_sym.nstraight<1 ? 1 : _sym.nstraight);					// nstraight
			}
			n = bitcount(((strbits>>i)&0x1f));
			if (5-n < _sym.nstraightfill)					// 2008-03-25 Matrix
			{
				_sym.nstraightfill = 5-n;												// nstraightfill
			}
		}

		// nstraightcommon, nstraightfillcommon
		if (_sym.nflopc<1)
		{
			_sym.nstraightfillcommon = 5;
		}
		else
		{
			strbits = 0;
			for (i=0; i<Rank_COUNT; i++)
			{
				if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*0)+i) ||
					CardMask_CARD_IS_SET(comCards, (Rank_COUNT*1)+i) ||
					CardMask_CARD_IS_SET(comCards, (Rank_COUNT*2)+i) ||
					CardMask_CARD_IS_SET(comCards, (Rank_COUNT*3)+i))
				{
					strbits |= (1<<(i+2));
				}
			}
			if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*0)+Rank_ACE) ||
				CardMask_CARD_IS_SET(comCards, (Rank_COUNT*1)+Rank_ACE) ||
				CardMask_CARD_IS_SET(comCards, (Rank_COUNT*2)+Rank_ACE) ||
				CardMask_CARD_IS_SET(comCards, (Rank_COUNT*3)+Rank_ACE))
			{
				strbits |= (1<<1);
			}

			for (i=10; i>=1; i--)
			{
				if (((strbits>>i)&0x1f) == 0x1f)
				{
					_sym.nstraightcommon = (_sym.nstraightcommon<5 ? 5 : _sym.nstraightcommon);
				}
				else if (((strbits>>i)&0x1e) == 0x1e ||
						 ((strbits>>i)&0x0f) == 0x0f)
				{
					_sym.nstraightcommon = (_sym.nstraightcommon<4 ? 4 : _sym.nstraightcommon);
				}
				else if (((strbits>>i)&0x1c) == 0x1c ||
						 ((strbits>>i)&0x0e) == 0x0e ||
						 ((strbits>>i)&0x07) == 0x07)
				{
					_sym.nstraightcommon = (_sym.nstraightcommon<3 ? 3 : _sym.nstraightcommon);
				}
				else if (((strbits>>i)&0x18) == 0x18 ||
						 ((strbits>>i)&0x0c) == 0x0c ||
						 ((strbits>>i)&0x06) == 0x06 ||
						 ((strbits>>i)&0x03) == 0x03)
				{
					_sym.nstraightcommon = (_sym.nstraightcommon<2 ? 2 : _sym.nstraightcommon);
				}
				else
				{
					_sym.nstraightcommon = (_sym.nstraightcommon<1 ? 1 : _sym.nstraightcommon); // nstraightcommon
				}
				n = bitcount(((strbits>>i)&0x1f));
				if (5-n < _sym.nstraightfillcommon)					//2008-03-25 Matrix
				{
					_sym.nstraightfillcommon = 5-n;										// nstraightfillcommon
				}
			}
		}

		// nstraightflush, nstraightflushfill
		for (j=0; j<4; j++)
		{
			strbits = 0;
			for (i=0; i<Rank_COUNT; i++)
			{
				if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*j)+i))
				{
					strbits |= (1<<(i+2));
				}
			}
			if (CardMask_CARD_IS_SET(plCards, (Rank_COUNT*j)+Rank_ACE))
			{
				strbits |= (1<<1);
			}

			for (i=10; i>=1; i--)
			{
				if (((strbits>>i)&0x1f) == 0x1f)
				{
					_sym.nstraightflush = (_sym.nstraightflush<5 ? 5 : _sym.nstraightflush);
				}
				else if (((strbits>>i)&0x1e) == 0x1e ||
						 ((strbits>>i)&0x0f) == 0x0f)
				{
					_sym.nstraightflush = (_sym.nstraightflush<4 ? 4 : _sym.nstraightflush);
				}
				else if (((strbits>>i)&0x1c) == 0x1c ||
						 ((strbits>>i)&0x0e) == 0x0e ||
						 ((strbits>>i)&0x07) == 0x07)
				{
					_sym.nstraightflush = (_sym.nstraightflush<3 ? 3 : _sym.nstraightflush);
				}
				else if (((strbits>>i)&0x18) == 0x18 ||
						 ((strbits>>i)&0x0c) == 0x0c ||
						 ((strbits>>i)&0x06) == 0x06 ||
						 ((strbits>>i)&0x03) == 0x03)
				{
					_sym.nstraightflush = (_sym.nstraightflush<2 ? 2 : _sym.nstraightflush);
				}
				else
				{
					_sym.nstraightflush = (_sym.nstraightflush<1 ? 1 : _sym.nstraightflush);	// nstraightflush
				}
				n = bitcount(((strbits>>i)&0x1f));
				if (5-n < _sym.nstraightflushfill)						// 2008-03-25 Matrix
				{
					_sym.nstraightflushfill = 5-n;										// nstraightflushfill
				}
			}
		}

		// nstraightflushcommon, nstraightflushfillcommon
		if (_sym.nflopc<1)
		{
			_sym.nstraightflushfillcommon = 5;
		}
		else
		{
			for (j=0; j<=3; j++)
			{
				strbits = 0;
				for (i=0; i<Rank_COUNT; i++)
				{
					if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*j)+i))
					{
						strbits |= (1<<(i+2));
					}
				}
				if (CardMask_CARD_IS_SET(comCards, (Rank_COUNT*j)+Rank_ACE))
				{
					strbits |= (1<<1);
				}

				for (i=10; i>=1; i--)
				{
					if (((strbits>>i)&0x1f) == 0x1f)
					{
						_sym.nstraightflushcommon = (_sym.nstraightflushcommon<5 ? 5 : _sym.nstraightflushcommon);
					}
					else if (((strbits>>i)&0x1e) == 0x1e ||
							 ((strbits>>i)&0x0f) == 0x0f)
					{
						_sym.nstraightflushcommon = (_sym.nstraightflushcommon<4 ? 4 : _sym.nstraightflushcommon);
					}
					else if (((strbits>>i)&0x1c) == 0x1c ||
							 ((strbits>>i)&0x0e) == 0x0e ||
							 ((strbits>>i)&0x07) == 0x07)
					{
						_sym.nstraightflushcommon = (_sym.nstraightflushcommon<3 ? 3 : _sym.nstraightflushcommon);
					}
					else if (((strbits>>i)&0x18) == 0x18 ||
							 ((strbits>>i)&0x0c) == 0x0c ||
							 ((strbits>>i)&0x06) == 0x06 ||
							 ((strbits>>i)&0x03) == 0x03)
					{
						_sym.nstraightflushcommon = (_sym.nstraightflushcommon<2 ? 2 : _sym.nstraightflushcommon);
					}
					else
					{
						_sym.nstraightflushcommon =
							(_sym.nstraightflushcommon<1 ? 1 : _sym.nstraightflushcommon);		// nstraightflushcommon
					}
					n = bitcount(((strbits>>i)&0x1f));
					if (5-n < _sym.nstraightflushfillcommon )			// 2008-03-25 Matrix
					{
						_sym.nstraightflushfillcommon = 5-n;								// nstraightflushfillcommon
					}
				}
			}
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcRankbits(void)
{
	int				i = 0, rank = 0, suit = 0, plcomsuit = 0, comsuit = 0;
	CardMask		plCards = {0}, comCards = {0}, plcomCards = {0};

	CardMask_RESET(plCards);
	CardMask_RESET(comCards);
	CardMask_RESET(plcomCards);

	// player cards
	for (i=0; i<=1; i++)
	{
		if (p_scraper->card_player(_sym.userchair, i) != CARD_BACK && 
			p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD)
		{
			CardMask_SET(plCards, p_scraper->card_player(_sym.userchair, i));
			CardMask_SET(plcomCards, p_scraper->card_player(_sym.userchair, i));
		}
	}

	// common cards
	for (i=0; i<=4; i++)
	{
		if (p_scraper->card_common(i) != CARD_BACK && 
			p_scraper->card_common(i) != CARD_NOCARD)
		{
			CardMask_SET(comCards, p_scraper->card_common(i));
			CardMask_SET(plcomCards, p_scraper->card_common(i));
		}
	}


	EnterCriticalSection(&cs_symbols);

		for (suit=StdDeck_Suit_FIRST; suit<=StdDeck_Suit_LAST; suit++)
		{
			for (rank=StdDeck_Rank_LAST; rank>=StdDeck_Rank_FIRST; rank--)
			{
				if (CardMask_CARD_IS_SET(plCards, StdDeck_MAKE_CARD(rank, suit)))
				{
					_sym.rankbitsplayer = (int) _sym.rankbitsplayer | (1<<(rank+2));		// rankbitsplayer
					if (rank == Rank_ACE)
					{
						_sym.rankbitsplayer = (int) _sym.rankbitsplayer | (1<<1);
					}
					if (rank+2>_sym.rankhiplayer)
					{
						_sym.rankhiplayer = rank+2;										// rankhiplayer
					}
					if (rank+2<_sym.rankloplayer || _sym.rankloplayer==0)
					{
						_sym.rankloplayer = rank+2;										// rankloplayer
					}
				}
				if (CardMask_CARD_IS_SET(comCards, StdDeck_MAKE_CARD(rank, suit)))
				{
					_sym.rankbitscommon = (int) _sym.rankbitscommon | (1<<(rank+2));		// rankbitscommon
					if (rank == Rank_ACE)
					{
						_sym.rankbitscommon = (int) _sym.rankbitscommon | (1<<1);
					}
					if (rank+2>_sym.rankhicommon)
					{
						_sym.rankhicommon = rank+2;										// rankhicommon
					}
					if (rank+2<_sym.ranklocommon || _sym.ranklocommon==0)
					{
						_sym.ranklocommon = rank+2;										// ranklocommon
					}
				}
				if (CardMask_CARD_IS_SET(plcomCards, StdDeck_MAKE_CARD(rank, suit)))
				{
					_sym.rankbits = (int) _sym.rankbits | (1<<(rank+2));					// rankbits
					if (rank == Rank_ACE)
					{
						_sym.rankbits = (int) _sym.rankbits | (1<<1);
					}
					if (rank+2>_sym.rankhi)
					{
						_sym.rankhi = rank+2;											// rankhi
					}
					if (rank+2<_sym.ranklo || _sym.ranklo==0)
					{
						_sym.ranklo = rank+2;											// ranklo
					}
				}
			}
		}
		_sym.rankbitspoker = (1<<(((int)_sym.pokerval>>16)&0xf)) +						// rankbitspoker
							(1<<(((int)_sym.pokerval>>12)&0xf)) +
							(1<<(((int)_sym.pokerval>>8)&0xf)) +
							(1<<(((int)_sym.pokerval>>4)&0xf)) +
							(1<<(((int)_sym.pokerval>>0)&0xf));
		_sym.rankbitspoker += ((int)_sym.rankbitspoker&0x4000) ? (1<<1) : 0; //ace
		for (i=14; i>=2; i--)
		{
			if ( (((int) _sym.rankbitspoker)>>i) & 0x1)
			{
				_sym.rankhipoker = i;													// rankhipoker
				i = 0;
			}
		}
		for (i=2; i<=14; i++)
		{
			if ( (((int) _sym.rankbitspoker)>>i) & 0x1)
			{
				_sym.ranklopoker = i;													// ranklopoker
				i = 15;
			}
		}

		comsuit = (_sym.tsuitcommon==WH_SUIT_CLUBS ? Suit_CLUBS :
				   _sym.tsuitcommon==WH_SUIT_DIAMONDS ? Suit_DIAMONDS :
				   _sym.tsuitcommon==WH_SUIT_HEARTS ? Suit_HEARTS :
				   _sym.tsuitcommon==WH_SUIT_SPADES ? Suit_SPADES : 0);
		plcomsuit = (_sym.tsuit==WH_SUIT_CLUBS ? Suit_CLUBS :
					 _sym.tsuit==WH_SUIT_DIAMONDS ? Suit_DIAMONDS :
					 _sym.tsuit==WH_SUIT_HEARTS ? Suit_HEARTS :
					 _sym.tsuit==WH_SUIT_SPADES ? Suit_SPADES : 0);
		for (rank=StdDeck_Rank_LAST; rank>=StdDeck_Rank_FIRST; rank--)
		{
			if (CardMask_CARD_IS_SET(plCards, StdDeck_MAKE_CARD(rank, plcomsuit)))
			{
				_sym.srankbitsplayer = (int) _sym.srankbitsplayer | (1<<(rank+2));		// srankbitsplayer
				if (rank == Rank_ACE)
				{
					_sym.srankbitsplayer = (int) _sym.srankbitsplayer | (1<<1);
				}
				if (rank+2>_sym.srankhiplayer)
				{
					_sym.srankhiplayer = rank+2;											// srankhiplayer
				}
				if (rank+2<_sym.srankloplayer || _sym.srankloplayer==0)
				{
					_sym.srankloplayer = rank+2;											// srankloplayer
				}
			}

			if (CardMask_CARD_IS_SET(comCards, StdDeck_MAKE_CARD(rank, comsuit)))
			{
				_sym.srankbitscommon = (int) _sym.srankbitscommon | (1<<(rank+2));		// srankbitscommon
				if (rank == Rank_ACE)
				{
					_sym.srankbitscommon = (int) _sym.srankbitscommon | (1<<1);
				}
				if (rank+2>_sym.srankhicommon)
				{
					_sym.srankhicommon = rank+2;											// srankhicommon
				}
				if (rank+2<_sym.sranklocommon || _sym.sranklocommon==0)
				{
					_sym.sranklocommon = rank+2;											// sranklocommon
				}
			}

			if (CardMask_CARD_IS_SET(plcomCards, StdDeck_MAKE_CARD(rank, plcomsuit)))
			{
				_sym.srankbits = (int) _sym.srankbits | (1<<(rank+2));					// srankbits
				if (rank == Rank_ACE)
				{
					_sym.srankbits = (int) _sym.srankbits | (1<<1);
				}
				if (rank+2>_sym.srankhi)
				{
					_sym.srankhi = rank+2;												// srankhi
				}
				if (rank+2<_sym.sranklo || _sym.sranklo==0)
				{
					_sym.sranklo = rank+2;												// sranklo
				}
			}
		}
		_sym.srankbitspoker = 															// srankbitspoker
			(CardMask_CARD_IS_SET(plcomCards, StdDeck_MAKE_CARD((((int)_sym.pokerval>>16)&0xf)-2, plcomsuit)) ?
			 (1<<(((int)_sym.pokerval>>16)&0xf)) : 0) +
			(CardMask_CARD_IS_SET(plcomCards, StdDeck_MAKE_CARD((((int)_sym.pokerval>>12)&0xf)-2, plcomsuit)) ?
			 (1<<(((int)_sym.pokerval>>12)&0xf)) : 0) +
			(CardMask_CARD_IS_SET(plcomCards, StdDeck_MAKE_CARD((((int)_sym.pokerval>>8)&0xf)-2, plcomsuit)) ?
			 (1<<(((int)_sym.pokerval>>8)&0xf)) : 0) +
			(CardMask_CARD_IS_SET(plcomCards, StdDeck_MAKE_CARD((((int)_sym.pokerval>>4)&0xf)-2, plcomsuit)) ?
			 (1<<(((int)_sym.pokerval>>4)&0xf)) : 0) +
			(CardMask_CARD_IS_SET(plcomCards, StdDeck_MAKE_CARD((((int)_sym.pokerval>>0)&0xf)-2, plcomsuit)) ?
			 (1<<(((int)_sym.pokerval>>0)&0xf)) : 0);
		_sym.srankbitspoker += ((int)_sym.srankbitspoker&0x4000) ? (1<<1) : 0; //ace
		for (i=14; i>=2; i--)
		{
			if ( (((int) _sym.srankbitspoker)>>i) & 0x1)
			{
				_sym.srankhipoker = i;													// srankhipoker
				i = 0;
			}
		}
		for (i=2; i<=14; i++)
		{
			if ( (((int) _sym.srankbitspoker)>>i) & 0x1)
			{
				_sym.sranklopoker = i;													// sranklopoker
				i = 15;
			}
		}

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcHistory(void)
{
	double		maxbet = 0.;
	int			i = 0;

	EnterCriticalSection(&cs_symbols);	

		if (_sym.nplayersround[(int) _sym.br-1]==0)
		{
			_sym.nplayersround[(int) _sym.br-1] = _sym.nplayersplaying;					// nplayersroundx
		}
		_sym.nplayersround[4] = _sym.nplayersround[(int) _sym.br-1];						// nplayersround

		maxbet = 0;
		for (i=0; i<p_tablemap->s$items()->num_chairs; i++)
		{
			if (_sym.currentbet[i] > maxbet)
			{
				maxbet = _sym.currentbet[i];
			}
		}
		maxbet /= (_sym.bet[4]==0 ? 1 : _sym.bet[4]);
		if (maxbet > _sym.nbetsround[(int) _sym.br-1])
		{
			_sym.nbetsround[(int) _sym.br-1] = maxbet;									// nbetsroundx
		}
		_sym.nbetsround[4] = _sym.nbetsround[(int) _sym.br-1];								// nbetsround

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcStatistics(void)
{
	double		f$srai = 0., C = 0., R = 0., S = 0., B = 0.;
	int			error = 0;
	CGrammar	gram;

	// f$srai
	error = SUCCESS;

	f$srai = gram.CalcF$symbol(p_formula, "f$srai", prefs.trace_functions(nTraceSwag), &error);

	// B
	B = p_formula->formula()->dBankroll != 0 ? p_formula->formula()->dBankroll : p_scraper->player_balance(_sym.userchair);

	EnterCriticalSection(&cs_symbols);	

		// call
		C = _sym.call;
		R = 0;
		S = C+R;
		_sym.callmean = (S==0) ? 0 : _sym.prwin*((_sym.pot+R)/S) + _sym.prtie*((_sym.pot+R)/2/S) - _sym.prlos*(S/S);
		_sym.callvariance = _sym.prwin*pow(_sym.pot/S - _sym.callmean, 2) +
						   _sym.prtie*pow(_sym.pot/S/2 - _sym.callmean, 2) +
						   _sym.prlos*pow(-1 - _sym.callmean, 2);
		_sym.callror = pow(M_E, ( 2*_sym.callmean*(0/S+_sym.pot/S)/_sym.callvariance ) - 1) /
					  pow(M_E, ( 2*_sym.callmean*(B/S+_sym.pot/S)/_sym.callvariance ) - 1);


		// rais
		C = _sym.call;
		R = _sym.bet[4];
		S = C+R;
		_sym.raismean = (S==0) ? 0 : _sym.prwin*((_sym.pot+R)/S) + _sym.prtie*((_sym.pot+R)/2/S) - _sym.prlos*(S/S);
		_sym.raisvariance = _sym.prwin*pow(_sym.pot/S - _sym.callmean, 2) +
						   _sym.prtie*pow(_sym.pot/S/2 - _sym.callmean, 2) +
						   _sym.prlos*pow(-1 - _sym.callmean, 2);
		_sym.raisror = pow(M_E, ( 2*_sym.raismean*(0/S+_sym.pot/S)/_sym.raisvariance ) - 1) /
					  pow(M_E, ( 2*_sym.raismean*(B/S+_sym.pot/S)/_sym.raisvariance ) - 1);

		// srai
		C = _sym.call;
		R = f$srai;
		S = C+R;
		_sym.sraimean = (S==0) ? 0 : _sym.prwin*((_sym.pot+R)/S) + _sym.prtie*((_sym.pot+R)/2/S) - _sym.prlos*(S/S);
		_sym.sraivariance = _sym.prwin*pow(_sym.pot/S - _sym.callmean, 2) +
						   _sym.prtie*pow(_sym.pot/S/2 - _sym.callmean, 2) +
						   _sym.prlos*pow(-1 - _sym.callmean, 2);
		_sym.srairor = pow(M_E, ( 2*_sym.sraimean*(0/S+_sym.pot/S)/_sym.sraivariance ) - 1) /
					  pow(M_E, ( 2*_sym.sraimean*(B/S+_sym.pot/S)/_sym.sraivariance ) - 1);

		// alli
		C = _sym.call;
		R = _sym.balance[10] - _sym.call;
		S = C+R;
		_sym.allimean = (S==0) ? 0 : _sym.prwin*((_sym.pot+R)/S) + _sym.prtie*((_sym.pot+R)/2/S) - _sym.prlos*(S/S);
		_sym.allivariance = _sym.prwin*pow(_sym.pot/S - _sym.callmean, 2) +
						   _sym.prtie*pow(_sym.pot/S/2 - _sym.callmean, 2) +
						   _sym.prlos*pow(-1 - _sym.callmean, 2);
		_sym.alliror = pow(M_E, ( 2*_sym.allimean*(0/S+_sym.pot/S)/_sym.allivariance ) - 1) /
					  pow(M_E, ( 2*_sym.allimean*(B/S+_sym.pot/S)/_sym.allivariance ) - 1);

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcRunRon(void)
{
	CRunRon			rr;

	rr.GetCounts();

	EnterCriticalSection(&cs_symbols);

		_sym.run$royfl = rr.srun()->royfl;
		_sym.run$strfl = rr.srun()->strfl;
		_sym.run$4kind = rr.srun()->fourkind;
		_sym.run$fullh = rr.srun()->fullh;
		_sym.run$flush = rr.srun()->flush;
		_sym.run$strai = rr.srun()->strai;
		_sym.run$3kind = rr.srun()->threekind;
		_sym.run$2pair = rr.srun()->twopair;
		_sym.run$1pair = rr.srun()->onepair;
		_sym.run$hcard = rr.srun()->hcard;
		_sym.run$total = rr.srun()->royfl+rr.srun()->strfl+rr.srun()->fourkind+rr.srun()->fullh+rr.srun()->flush+
						rr.srun()->strai+rr.srun()->threekind+rr.srun()->twopair+rr.srun()->onepair+rr.srun()->hcard;
		_sym.run$prbest = rr.srun()->pokervalmaxcount/_sym.run$total;
		_sym.run$pokervalmax = rr.srun()->pokervalmax;
		_sym.run$clocks = rr.srun()->clocks;

		_sym.ron$royfl = rr.sron()->royfl;
		_sym.ron$strfl = rr.sron()->strfl;
		_sym.ron$4kind = rr.sron()->fourkind;
		_sym.ron$fullh = rr.sron()->fullh;
		_sym.ron$flush = rr.sron()->flush;
		_sym.ron$strai = rr.sron()->strai;
		_sym.ron$3kind = rr.sron()->threekind;
		_sym.ron$2pair = rr.sron()->twopair;
		_sym.ron$1pair = rr.sron()->onepair;
		_sym.ron$hcard = rr.sron()->hcard;
		_sym.ron$total = rr.sron()->royfl+rr.sron()->strfl+rr.sron()->fourkind+rr.sron()->fullh+rr.sron()->flush+
						rr.sron()->strai+rr.sron()->threekind+rr.sron()->twopair+rr.sron()->onepair+rr.sron()->hcard;
		_sym.ron$prbest = rr.sron()->pokervalmaxcount/_sym.ron$total;
		_sym.ron$pokervalmax = rr.sron()->pokervalmax;
		_sym.ron$clocks = rr.sron()->clocks;


		//////////////////////////////////////////////////////////////
		// probabilities
		//_sym.run$prnuts = totals[];
		//_sym.ron$prnuts = /run$total;

	LeaveCriticalSection(&cs_symbols);
}


const void CSymbols::GetCardstring(char *c, unsigned int c0, unsigned int c1)
{
	char		card0[10] = {0}, card1[10] = {0};

	// figure out the card string to search for
	if (StdDeck_RANK(c0) >= StdDeck_RANK(c1))
	{
		StdDeck_cardToString(c0, card0);
		StdDeck_cardToString(c1, card1);
	}
	else
	{
		StdDeck_cardToString(c1, card0);
		StdDeck_cardToString(c0, card1);
	}
	c[0] = card0[0];
	c[1] = card1[0];

	if (c[0] == c[1] || card0[1] != card1[1])
	{
		c[2] = 'o';
	}
	else 
	{
		c[2] = 's';
	}
	c[3]='\0';
}

const double CSymbols::CalcPokerval(HandVal hv, int n, double *pcb, int card0, int card1)
{
	double			pv = 0.;
	int				i = 0, j = 0, k = 0, max = 0, c = 0, flush_suit = 0; //Matrix 2008-06-28
	double			bits = 0.;
	CardMask		Cards = {0}, heartsCards = {0}, diamondsCards = {0}, clubsCards = {0}, spadesCards = {0}, suittestCards = {0};

	// If we have a straight flush or flush, figure out the suit
	flush_suit = -1;
	if (HandVal_HANDTYPE(hv)==HandType_STFLUSH || HandVal_HANDTYPE(hv)==HandType_FLUSH)
	{
		// Set up some suit masks
		CardMask_RESET(heartsCards);
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_2, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_3, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_4, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_5, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_6, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_7, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_8, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_9, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_KING, Suit_HEARTS));
		CardMask_SET(heartsCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_HEARTS));

		CardMask_RESET(diamondsCards);
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_2, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_3, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_4, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_5, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_6, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_7, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_8, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_9, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_KING, Suit_DIAMONDS));
		CardMask_SET(diamondsCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_DIAMONDS));

		CardMask_RESET(spadesCards);
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_2, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_3, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_4, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_5, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_6, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_7, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_8, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_9, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_KING, Suit_SPADES));
		CardMask_SET(spadesCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_SPADES));

		CardMask_RESET(clubsCards);
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_2, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_3, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_4, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_5, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_6, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_7, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_8, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_9, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_TEN, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_JACK, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_QUEEN, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_KING, Suit_CLUBS));
		CardMask_SET(clubsCards, StdDeck_MAKE_CARD(Rank_ACE, Suit_CLUBS));


		CardMask_RESET(Cards);
		for (i=0; i<=1; i++)
		{
			if (p_scraper->card_player(_sym.userchair, i) != CARD_BACK && 
				p_scraper->card_player(_sym.userchair, i) != CARD_NOCARD)
			{
				CardMask_SET(Cards, p_scraper->card_player(_sym.userchair, i));
			}
		}

		for (i=0; i<=4; i++)
		{
			if (p_scraper->card_common(i) != CARD_BACK && 
				p_scraper->card_common(i) != CARD_NOCARD)
			{
				CardMask_SET(Cards, p_scraper->card_common(i));
			}
		}

		max = 0;
		CardMask_AND(suittestCards, Cards, spadesCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_SPADES;
		}
		CardMask_AND(suittestCards, Cards, heartsCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_HEARTS;
		}
		CardMask_AND(suittestCards, Cards, diamondsCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_DIAMONDS;
		}
		CardMask_AND(suittestCards, Cards, clubsCards);
		c = StdDeck_numCards(suittestCards);
		if ( c>max && c>0)
		{
			max = c;
			flush_suit = Suit_CLUBS;
		}
	}

	// Now figure out the pokerval and pcbits
	switch HandVal_HANDTYPE(hv)
	{
	case HandType_STFLUSH:
		pv += _sym.straightflush;
		pv += (HandVal_TOP_CARD(hv)+2-0)<<16;
		pv += (HandVal_TOP_CARD(hv)+2-1)<<12;
		pv += (HandVal_TOP_CARD(hv)+2-2)<<8;
		pv += (HandVal_TOP_CARD(hv)+2-3)<<4;
		pv += (HandVal_TOP_CARD(hv)+2-4)<<0;

		for (i=0; i<5; i++)
		{
			j = StdDeck_RANK(card0);	//Matrix 2008-06-28
			k = StdDeck_RANK(card1);
			if ( (j == HandVal_TOP_CARD(hv)-i &&
					StdDeck_SUIT(card0) == flush_suit)
					||
					k == HandVal_TOP_CARD(hv)-i &&
					StdDeck_SUIT(card1) == flush_suit)
			{
				bits = (int) bits | (1<<(4-i));
			}
			//In straight evaluation an Ace can appear in hv as either 0x0c or 0xff. 
			//We need to do an ugly test for both cases.

			if((j==12)||(k==12))  //Matrix 2008-10-14 !!KLUDGE ALERT!!
			{
				if(j==12)j=-1;
				if(k==12)k=-1;
				if ( (j == HandVal_TOP_CARD(hv)-i &&
					StdDeck_SUIT(card0) == flush_suit)
					||
					k == HandVal_TOP_CARD(hv)-i &&
					StdDeck_SUIT(card1) == flush_suit)
				{
					bits = (int) bits | (1<<(4-i));
				}
			}


		}
		break;

	case HandType_QUADS:
		pv += _sym.fourofakind;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_TOP_CARD(hv)+2)<<8;
		pv += (HandVal_TOP_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_SECOND_CARD(hv)+2)<<0;

		// Either hole card is used in quads
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits = (int) bits | (1<<4);
		}
		// Both hole cards are used in quads
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits = (int) bits | (1<<4);
			bits = (int) bits | (1<<3);
		}
		// Either hole card is used as kicker
		if (n>=5 &&
				StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv))
		{
			bits = (int) bits | (1<<0);
		}
		break;

	case HandType_FULLHOUSE:
		pv += _sym.fullhouse;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_TOP_CARD(hv)+2)<<8;
		pv += (HandVal_SECOND_CARD(hv)+2)<<4;
		pv += (HandVal_SECOND_CARD(hv)+2)<<0;

		// Either hole card is used in top of boat
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits = (int) bits | (1<<4);
		}
		// Both hole cards are used in top of boat
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits = (int) bits | (1<<4);
			bits = (int) bits | (1<<3);
		}
		// Either hole card is used in bottom of boat
		if ( (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv))  !=
				(StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv)) )
		{
			bits = (int) bits | (1<<1);
		}
		// Both hole cards are used in bottom of boat
		else if (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv))
		{
			bits = (int) bits | (1<<1);
			bits = (int) bits | (1<<0);
		}
		break;

	case HandType_FLUSH:
		pv += _sym.flush;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_SECOND_CARD(hv)+2)<<12;
		pv += (HandVal_THIRD_CARD(hv)+2)<<8;
		pv += (HandVal_FOURTH_CARD(hv)+2)<<4;
		pv += (HandVal_FIFTH_CARD(hv)+2)<<0;

		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits = (int) bits | (1<<4);
		}
		if ( (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits = (int) bits | (1<<3);
		}
		if ( (StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits = (int) bits | (1<<2);
		}
		if ( (StdDeck_RANK(card0) == HandVal_FOURTH_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_FOURTH_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits = (int) bits | (1<<1);
		}
		if ( (StdDeck_RANK(card0) == HandVal_FIFTH_CARD(hv) &&
				StdDeck_SUIT(card0) == flush_suit)
				||
				(StdDeck_RANK(card1) == HandVal_FIFTH_CARD(hv) &&
				 StdDeck_SUIT(card1) == flush_suit) )
		{
			bits = (int) bits | (1<<0);
		}
		break;

	case HandType_STRAIGHT:
		pv += _sym.straight;
		pv += (HandVal_TOP_CARD(hv)+2-0)<<16;
		pv += (HandVal_TOP_CARD(hv)+2-1)<<12;
		pv += (HandVal_TOP_CARD(hv)+2-2)<<8;
		pv += (HandVal_TOP_CARD(hv)+2-3)<<4;
		pv += (HandVal_TOP_CARD(hv)+2-4)<<0;
		for (i=0; i<5; i++)
		{
			j = StdDeck_RANK(card0);	//Matrix 2008-06-28
			k = StdDeck_RANK(card1);
			if (j == HandVal_TOP_CARD(hv)-i ||
					k == HandVal_TOP_CARD(hv)-i)
			{
				bits = (int) bits | (1<<(4-i));
			}
			if((j==12)||(k==12))  //Matrix 2008-10-14 !!KLUDGE ALERT!!
			{
				if(j==12)j=-1;
				if(k==12)k=-1;
				if (j == HandVal_TOP_CARD(hv)-i ||
					k == HandVal_TOP_CARD(hv)-i)
					{
						bits = (int) bits | (1<<(4-i));
					}
			}

		}
		break;

	case HandType_TRIPS:
		pv += _sym.threeofakind;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_TOP_CARD(hv)+2)<<8;
		if (n>=4)  pv += (HandVal_SECOND_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_THIRD_CARD(hv)+2)<<0;

		// either hole card used in trips
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits = (int) bits | (1<<4);
		}
		// both hole cards used in trips
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits = (int) bits | (1<<4);
			bits = (int) bits | (1<<3);
		}
		// either hole card is first kicker
		if (n>=4 &&
				StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv) )
		{
			bits = (int) bits | (1<<1);
		}
		// either hole card is second kicker
		if (n>=5 &&
				StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv) )
		{
			bits = (int) bits | (1<<1);
		}
		break;

	case HandType_TWOPAIR:
		pv += _sym.twopair;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		pv += (HandVal_SECOND_CARD(hv)+2)<<8;
		pv += (HandVal_SECOND_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_THIRD_CARD(hv)+2)<<0;

		// either hole card used in top pair
		if ( (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_TOP_CARD(hv)) )
		{
			bits = (int) bits | (1<<4);
		}
		// both hole cards used in top pair
		else if (StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_TOP_CARD(hv))
		{
			bits = (int) bits | (1<<4);
			bits = (int) bits | (1<<3);
		}
		// either hole card used in bottom pair
		if ( (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv)) !=
				(StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv)) )
		{
			bits = (int) bits | (1<<2);
		}
		// both hole cards used in bottom pair
		else if (StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) &&
				 StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv))
		{
			bits = (int) bits | (1<<2);
			bits = (int) bits | (1<<1);
		}
		// either hole card kicker
		if (n>=5 &&
				StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv) )
		{
			bits = (int) bits | (1<<0);
		}
		break;

	case HandType_ONEPAIR:
		pv += _sym.onepair;
		pv += (HandVal_TOP_CARD(hv)+2)<<16;
		pv += (HandVal_TOP_CARD(hv)+2)<<12;
		if (n>=3)  pv += (HandVal_SECOND_CARD(hv)+2)<<8;
		if (n>=4)  pv += (HandVal_THIRD_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_FOURTH_CARD(hv)+2)<<0;

		// either hole card used in pair
		if ( (StdDeck_RANK(card0) == StdDeck_RANK(HandVal_TOP_CARD(hv))) !=
				(StdDeck_RANK(card1) == StdDeck_RANK(HandVal_TOP_CARD(hv))) )
		{
			bits = (int) bits | (1<<4);
		}
		// both hole cards used in pair
		else if (StdDeck_RANK(card0) == StdDeck_RANK(HandVal_TOP_CARD(hv)) &&
				 StdDeck_RANK(card1) == StdDeck_RANK(HandVal_TOP_CARD(hv)))
		{
			bits = (int) bits | (1<<4);
			bits = (int) bits | (1<<3);
		}
		// either hole card used as first kicker
		if (n>=3 &&
				StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv) )
		{
			bits = (int) bits | (1<<2);
		}
		// either hole card used as second kicker
		if (n>=4 &&
				StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv) )
		{
			bits = (int) bits | (1<<1);
		}
		// either hole card used as third kicker
		if (n>=5 &&
				StdDeck_RANK(card0) == HandVal_FOURTH_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_FOURTH_CARD(hv) )
		{
			bits = (int) bits | (1<<0);
		}
		break;

	case HandType_NOPAIR:
		if (n>=1)  pv += (HandVal_TOP_CARD(hv)+2)<<16;
		if (n>=2)  pv += (HandVal_SECOND_CARD(hv)+2)<<12;
		if (n>=3)  pv += (HandVal_THIRD_CARD(hv)+2)<<8;
		if (n>=4)  pv += (HandVal_FOURTH_CARD(hv)+2)<<4;
		if (n>=5)  pv += (HandVal_FIFTH_CARD(hv)+2)<<0;

		// either hole card used as top card
		if (n>=1 &&
				StdDeck_RANK(card0) == HandVal_TOP_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_TOP_CARD(hv) )
		{
			bits = (int) bits | (1<<4);
		}
		// either hole card used as first kicker
		if (n>=2 &&
				StdDeck_RANK(card0) == HandVal_SECOND_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_SECOND_CARD(hv) )
		{
			bits = (int) bits | (1<<3);
		}
		// either hole card used as second kicker
		if (n>=3 &&
				StdDeck_RANK(card0) == HandVal_THIRD_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_THIRD_CARD(hv) )
		{
			bits = (int) bits | (1<<2);
		}
		// either hole card used as third kicker
		if (n>=3 &&
				StdDeck_RANK(card0) == HandVal_FOURTH_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_FOURTH_CARD(hv) )
		{
			bits = (int) bits | (1<<1);
		}
		// either hole card used as fourth kicker
		if (n>=3 &&
				StdDeck_RANK(card0) == HandVal_FIFTH_CARD(hv) ||
				StdDeck_RANK(card1) == HandVal_FIFTH_CARD(hv) )
		{
			bits = (int) bits | (1<<0);
		}
		break;
	}

	*pcb = bits;
	return pv;
}

void CSymbols::CalcPrimaryFormulas(const bool final_answer)
{
	int			e = SUCCESS;
	CGrammar	gram;

	EnterCriticalSection(&cs_symbols);

	_sym.isfinalanswer = final_answer;

	e = SUCCESS;
	_f$alli = gram.CalcF$symbol(p_formula, "f$alli", (final_answer && prefs.trace_functions(nTraceAlli)), &e);

	e = SUCCESS;
	_f$swag = gram.CalcF$symbol(p_formula, "f$swag", (final_answer && prefs.trace_functions(nTraceSwag)), &e);

	e = SUCCESS;
	_f$rais = gram.CalcF$symbol(p_formula, "f$rais", (final_answer && prefs.trace_functions(nTraceRais)), &e);

	e = SUCCESS;
	_f$call = gram.CalcF$symbol(p_formula, "f$call", (final_answer && prefs.trace_functions(nTraceCall)), &e);

	_sym.isfinalanswer = false;

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::CalcSecondaryFormulas(void)
{
	int			e = SUCCESS;
	CGrammar	gram;

	EnterCriticalSection(&cs_symbols);

		e = SUCCESS;
		_f$play = gram.CalcF$symbol(p_formula, "f$play", prefs.trace_functions(nTracePlay), &e);

		e = SUCCESS;
		_f$prefold = gram.CalcF$symbol(p_formula, "f$prefold", prefs.trace_functions(nTracePrefold), &e);

		e = SUCCESS;
		_f$delay = gram.CalcF$symbol(p_formula, "f$delay", &e);

		e = SUCCESS;
		_f$chat = gram.CalcF$symbol(p_formula, "f$chat", &e);

	LeaveCriticalSection(&cs_symbols);
}

void CSymbols::UpdateAutoplayerInfo(void)
{
	int		i = 0;

	EnterCriticalSection(&cs_symbols);

		_sym.prevaction = p_autoplayer->prevaction();

		for (i=0; i<=4; i++)
		{
			_sym.didchec[i] = p_autoplayer->didchec(i);
			_sym.didcall[i] = p_autoplayer->didcall(i);
			_sym.didrais[i] = p_autoplayer->didrais(i);
			_sym.didswag[i] = p_autoplayer->didswag(i);
		}

	LeaveCriticalSection(&cs_symbols);
}

const double CSymbols::GetSymbolVal(const char *a, int *e)
{
	if (memcmp(a, "ismanual", 8)==0 && strlen(a)==8)					return _sym.ismanual;
	if (memcmp(a, "isppro", 6)==0 && strlen(a)==6)						return _sym.isppro;
	if (memcmp(a, "site", 4)==0 && strlen(a)==4)						return _sym.site;
	if (memcmp(a, "nchairs", 7)==0 && strlen(a)==7)						return _sym.nchairs;
	if (memcmp(a, "isbring", 7)==0 && strlen(a)==7)						return _sym.isbring;
	if (memcmp(a, "session", 7)==0 && strlen(a)==7)						return _sym.session;
	if (memcmp(a, "handnumber", 10)==0 && strlen(a)==10)				return _sym.handnumber;
	if (memcmp(a, "version", 7)==0 && strlen(a)==7)						return _sym.version;

	//PROFILE
	if (memcmp(a, "sitename$", 9)==0)									return p_tablemap->s$items()->sitename.Find(&a[9])!=-1;
	if (memcmp(a, "network$", 8)==0)									return p_tablemap->s$items()->network.Find(&a[8])!=-1;
	if (memcmp(a, "swagdelay", 9)==0 && strlen(a)==9)					return _sym.swagdelay;
	if (memcmp(a, "allidelay", 9)==0 && strlen(a)==9)					return _sym.allidelay;
	if (memcmp(a, "swagtextmethod", 14)==0 && strlen(a)==14)			return _sym.swagtextmethod;
	if (memcmp(a, "potmethod", 9)==0 && strlen(a)==9)					return _sym.potmethod;
	if (memcmp(a, "activemethod", 12)==0 && strlen(a)==12)				return _sym.activemethod;

	//FORMULA FILE
	if (memcmp(a, "rake", 4)==0 && strlen(a)==4)						return _sym.rake;
	if (memcmp(a, "nit", 3)==0 && strlen(a)==3)							return _sym.nit;
	if (memcmp(a, "bankroll", 8)==0 && strlen(a)==8)					return _sym.bankroll;

	//LIMITS
	if (memcmp(a, "bblind", 6)==0 && strlen(a)==6)						return _sym.bblind;
	if (memcmp(a, "sblind", 6)==0 && strlen(a)==6)						return _sym.sblind;
	if (memcmp(a, "ante", 4)==0 && strlen(a)==4)						return _sym.ante;
	if (memcmp(a, "lim", 3)==0 && strlen(a)==3)							return _sym.lim;
	if (memcmp(a, "isnl", 4)==0 && strlen(a)==4)						return _sym.isnl;
	if (memcmp(a, "ispl", 4)==0 && strlen(a)==4)						return _sym.ispl;
	if (memcmp(a, "isfl", 4)==0 && strlen(a)==4)						return _sym.isfl;
	if (memcmp(a, "sraiprev", 8)==0 && strlen(a)==8)					return _sym.sraiprev;
	if (memcmp(a, "sraimin", 7)==0 && strlen(a)==7)						return _sym.sraimin;
	if (memcmp(a, "sraimax", 7)==0 && strlen(a)==7)						return _sym.sraimax;
	if (memcmp(a, "istournament", 12)==0 && strlen(a)==12)				return _sym.istournament;

	//HAND RANK
	if (memcmp(a, "handrank", 8)==0 && strlen(a)==8)					return _sym.handrank;
	if (memcmp(a, "handrank169", 11)==0 && strlen(a)==11)				return _sym.handrank169;
	if (memcmp(a, "handrank2652", 12)==0 && strlen(a)==12)				return _sym.handrank2652;
	if (memcmp(a, "handrank1326", 12)==0 && strlen(a)==12)				return _sym.handrank1326;
	if (memcmp(a, "handrank1000", 12)==0 && strlen(a)==12)				return _sym.handrank1000;
	if (memcmp(a, "handrankp", 9)==0 && strlen(a)==9)					return _sym.handrankp;

	//CHAIRS
	if (memcmp(a, "chair", 5)==0 && strlen(a)==5)						return _sym.chair;
	if (memcmp(a, "userchair", 9)==0 && strlen(a)==9)					return _sym.userchair;
	if (memcmp(a, "dealerchair", 11)==0 && strlen(a)==11)				return _sym.dealerchair;
	if (memcmp(a, "raischair", 9)==0 && strlen(a)==9)					return _sym.raischair;
	if (memcmp(a, "chair$", 6)==0)										return Chair$(a);
	if (memcmp(a, "chairbit$", 9)==0)									return Chairbit$(a);

	//ROUND&POSITIONS
	if (memcmp(a, "betround", 8)==0 && strlen(a)==8)					return _sym.betround;
	if (memcmp(a, "br", 2)==0 && strlen(a)==2)							return _sym.br;
	if (memcmp(a, "betposition", 11)==0 && strlen(a)==11)				return _sym.betposition;
	if (memcmp(a, "dealposition", 12)==0 && strlen(a)==12)				return _sym.dealposition;
	if (memcmp(a, "callposition", 12)==0 && strlen(a)==12)				return _sym.callposition;
	if (memcmp(a, "seatposition", 12)==0 && strlen(a)==12)				return _sym.seatposition;
	if (memcmp(a, "dealpositionrais", 16)==0 && strlen(a)==16)			return _sym.dealpositionrais;
	if (memcmp(a, "betpositionrais", 15)==0 && strlen(a)==15)			return _sym.betpositionrais;
	if (memcmp(a, "originaldealposition", 20)==0 && strlen(a)==20)		return _sym.originaldealposition; //Matrix 2008-05-09

	//PROBABILITIES
	if (memcmp(a, "prwin", 5)==0 && strlen(a)==5)						return _sym.prwin;
	if (memcmp(a, "prlos", 5)==0 && strlen(a)==5)						return _sym.prlos;
	if (memcmp(a, "prtie", 5)==0 && strlen(a)==5)						return _sym.prtie;
	if (memcmp(a, "random", 6)==0 && strlen(a)==6)						return _sym.random;
	if (memcmp(a, "randomhand", 10)==0 && strlen(a)==10)				return _sym.randomhand;
	if (memcmp(a, "randomround", 11)==0 && strlen(a)==11)				return _sym.randomround[4];
	if (memcmp(a, "randomround", 11)==0 && strlen(a)==12)				return _sym.randomround[a[11]-'0'-1];

	//STATISTICS
	if (memcmp(a, "callror", 7)==0 && strlen(a)==7)						return _sym.callror;
	if (memcmp(a, "raisror", 7)==0 && strlen(a)==7)  					return _sym.raisror;
	if (memcmp(a, "srairor", 7)==0 && strlen(a)==7)  					return _sym.srairor;
	if (memcmp(a, "alliror", 7)==0 && strlen(a)==7)  					return _sym.alliror;
	if (memcmp(a, "callmean", 8)==0 && strlen(a)==8)  					return _sym.callmean;
	if (memcmp(a, "raismean", 8)==0 && strlen(a)==8)  					return _sym.raismean;
	if (memcmp(a, "sraimean", 8)==0 && strlen(a)==8)  					return _sym.sraimean;
	if (memcmp(a, "allimean", 8)==0 && strlen(a)==8)  					return _sym.allimean;
	if (memcmp(a, "callvariance", 12)==0 && strlen(a)==12)				return _sym.callvariance;
	if (memcmp(a, "raisvariance", 12)==0 && strlen(a)==12)  			return _sym.raisvariance;
	if (memcmp(a, "sraivariance", 12)==0 && strlen(a)==12)  			return _sym.sraivariance;
	if (memcmp(a, "allivariance", 12)==0 && strlen(a)==12)  			return _sym.allivariance;

	//P FORMULA
	if (memcmp(a, "defcon", 6)==0 && strlen(a)==6)						return _sym.defcon;
	if (memcmp(a, "isdefmode", 9)==0 && strlen(a)==9)					return _sym.isdefmode;
	if (memcmp(a, "isaggmode", 9)==0 && strlen(a)==9)					return _sym.isaggmode;

	//CHIP AMOUNTS
	if (memcmp(a, "balance", 7)==0 && strlen(a)==7)						return _sym.balance[10];
	if (memcmp(a, "balance", 7)==0 && strlen(a)==8)						return _sym.balance[a[7]-'0'];
	if (memcmp(a, "stack", 5)==0 && strlen(a)==6)						return _sym.stack[a[5]-'0'];
	if (memcmp(a, "currentbet", 10)==0 && strlen(a)==10)				return _sym.currentbet[10];
	if (memcmp(a, "currentbet", 10)==0 && strlen(a)==11)				return _sym.currentbet[a[10]-'0'];
	if (memcmp(a, "call", 4)==0 && strlen(a)==4)						return _sym.call;
	if (memcmp(a, "bet", 3)==0 && strlen(a)==3)							return _sym.bet[4];
	if (memcmp(a, "bet", 3)==0 && strlen(a)==4)							return _sym.bet[a[3]-'0'-1];
	if (memcmp(a, "pot", 3)==0 && strlen(a)==3)							return _sym.pot;
	if (memcmp(a, "potcommon", 9)==0 && strlen(a)==9)					return _sym.potcommon;
	if (memcmp(a, "potplayer", 9)==0 && strlen(a)==9)					return _sym.potplayer;
	if (memcmp(a, "callshort", 9)==0 && strlen(a)==9)					return _sym.callshort;
	if (memcmp(a, "raisshort", 9)==0 && strlen(a)==9)					return _sym.raisshort;

	//NUMBER OF BETS
	if (memcmp(a, "nbetstocall", 11)==0 && strlen(a)==11)				return _sym.nbetstocall;
	if (memcmp(a, "nbetstorais", 11)==0 && strlen(a)==11)				return _sym.nbetstorais;
	if (memcmp(a, "ncurrentbets", 12)==0 && strlen(a)==12)				return _sym.ncurrentbets;
	if (memcmp(a, "ncallbets", 9)==0 && strlen(a)==9)					return _sym.ncallbets;
	if (memcmp(a, "nraisbets", 9)==0 && strlen(a)==9)					return _sym.nraisbets;

	//LIST TESTS
	if (memcmp(a, "islistcall", 10)==0 && strlen(a)==10)				return _sym.islistcall;
	if (memcmp(a, "islistrais", 10)==0 && strlen(a)==10)				return _sym.islistrais;
	if (memcmp(a, "islistalli", 10)==0 && strlen(a)==10)				return _sym.islistalli;
	if (memcmp(a, "islist", 6)==0 && (strlen(a)<10) && (atoi(a+6)<MAX_HAND_LISTS)) return _sym.islist[atoi(a+6)]; //Matrix 2008-05-14
	if (memcmp(a, "isemptylistcall", 15)==0 && strlen(a)==15)			return _sym.isemptylistcall;
	if (memcmp(a, "isemptylistrais", 15)==0 && strlen(a)==15)			return _sym.isemptylistrais;
	if (memcmp(a, "isemptylistalli", 15)==0 && strlen(a)==15)			return _sym.isemptylistalli;

	//LIST NUMBERS
	if (memcmp(a, "nlistmax", 8)==0 && strlen(a)==8)					return _sym.nlistmax;
	if (memcmp(a, "nlistmin", 8)==0 && strlen(a)==8)					return _sym.nlistmin;

	//POKER VALUES
	if (memcmp(a, "pokerval", 8)==0 && strlen(a)==8)					return _sym.pokerval;
	if (memcmp(a, "pokervalplayer", 14)==0 && strlen(a)==14)			return _sym.pokervalplayer;
	if (memcmp(a, "pokervalcommon", 14)==0 && strlen(a)==14)			return _sym.pokervalcommon;
	if (memcmp(a, "pcbits", 6)==0 && strlen(a)==6)						return _sym.pcbits;
	if (memcmp(a, "npcbits", 7)==0 && strlen(a)==7)						return _sym.npcbits;

	//POKER VALUE CONSTANTS
	if (memcmp(a, "hicard", 6)==0 && strlen(a)==6)						return _sym.hicard;
	if (memcmp(a, "onepair", 7)==0 && strlen(a)==7)						return _sym.onepair;
	if (memcmp(a, "twopair", 7)==0 && strlen(a)==7)						return _sym.twopair;
	if (memcmp(a, "threeofakind", 12)==0 && strlen(a)==12)				return _sym.threeofakind;
	if (memcmp(a, "straight", 8)==0 && strlen(a)==8)					return _sym.straight;
	if (memcmp(a, "flush", 5)==0 && strlen(a)==5)						return _sym.flush;
	if (memcmp(a, "fullhouse", 9)==0 && strlen(a)==9)					return _sym.fullhouse;
	if (memcmp(a, "fourofakind", 11)==0 && strlen(a)==11)				return _sym.fourofakind;
	if (memcmp(a, "straightflush", 13)==0 && strlen(a)==13)				return _sym.straightflush;
	if (memcmp(a, "royalflush", 10)==0 && strlen(a)==10)				return _sym.royalflush;
	if (memcmp(a, "fiveofakind", 11)==0 && strlen(a)==11)				return _sym.fiveofakind;

	//HAND TESTS
	if (memcmp(a, "$$pc", 4)==0)										return _sym.$$pc[a[4]-'0'];
	if (memcmp(a, "$$pr", 4)==0)										return _sym.$$pr[a[4]-'0'];
	if (memcmp(a, "$$ps", 4)==0)  										return _sym.$$ps[a[4]-'0'];
	if (memcmp(a, "$$cc", 4)==0)  										return _sym.$$cc[a[4]-'0'];
	if (memcmp(a, "$$cr", 4)==0)  										return _sym.$$cr[a[4]-'0'];
	if (memcmp(a, "$$cs", 4)==0)  										return _sym.$$cs[a[4]-'0'];
	if (memcmp(a, "$", 1)==0)  											return IsHand(a, e);
	if (memcmp(a, "ishandup", 8)==0 && strlen(a)==8)					return _sym.ishandup;
	if (memcmp(a, "ishandupcommon", 14)==0 && strlen(a)==14)			return _sym.ishandupcommon;
	if (memcmp(a, "ishicard", 8)==0 && strlen(a)==8)					return _sym.ishicard;
	if (memcmp(a, "isonepair", 9)==0 && strlen(a)==9)					return _sym.isonepair;
	if (memcmp(a, "istwopair", 9)==0 && strlen(a)==9)					return _sym.istwopair;
	if (memcmp(a, "isthreeofakind", 14)==0 && strlen(a)==14)			return _sym.isthreeofakind;
	if (memcmp(a, "isstraight", 10)==0 && strlen(a)==10)				return _sym.isstraight;
	if (memcmp(a, "isflush", 7)==0 && strlen(a)==7)						return _sym.isflush;
	if (memcmp(a, "isfullhouse", 11)==0 && strlen(a)==11)				return _sym.isfullhouse;
	if (memcmp(a, "isfourofakind", 13)==0 && strlen(a)==13)				return _sym.isfourofakind;
	if (memcmp(a, "isstraightflush", 15)==0 && strlen(a)==15)			return _sym.isstraightflush;
	if (memcmp(a, "isroyalflush", 12)==0 && strlen(a)==12)				return _sym.isroyalflush;
	if (memcmp(a, "isfiveofakind", 13)==0 && strlen(a)==13)				return _sym.isfiveofakind;

	//POCKET TESTS
	if (memcmp(a, "ispair", 6)==0 && strlen(a)==6)						return _sym.ispair;
	if (memcmp(a, "issuited", 8)==0 && strlen(a)==8)					return _sym.issuited;
	if (memcmp(a, "isconnector", 11)==0 && strlen(a)==11)				return _sym.isconnector;

	//POCKET/COMMON TESTS
	if (memcmp(a, "ishipair", 8)==0 && strlen(a)==8)					return _sym.ishipair;
	if (memcmp(a, "islopair", 8)==0 && strlen(a)==8)					return _sym.islopair;
	if (memcmp(a, "ismidpair", 9)==0 && strlen(a)==9)					return _sym.ismidpair;
	if (memcmp(a, "ishistraight", 12)==0 && strlen(a)==12)				return _sym.ishistraight;
	if (memcmp(a, "ishiflush", 9)==0 && strlen(a)==9)					return _sym.ishiflush;

	//PLAYERS FRIENDS OPPONENTS
	if (memcmp(a, "nopponents", 10)==0 && strlen(a)==10)				return _sym.nopponents;
	if (memcmp(a, "nopponentsmax", 13)==0 && strlen(a)==13)				return _sym.nopponentsmax;
	if (memcmp(a, "nplayersseated", 14)==0 && strlen(a)==14)			return _sym.nplayersseated;
	if (memcmp(a, "nplayersactive", 14)==0 && strlen(a)==14)			return _sym.nplayersactive;
	if (memcmp(a, "nplayersdealt", 13)==0 && strlen(a)==13)				return _sym.nplayersdealt;
	if (memcmp(a, "nplayersplaying", 15)==0 && strlen(a)==15)			return _sym.nplayersplaying;
	if (memcmp(a, "nplayersblind", 13)==0 && strlen(a)==13)				return _sym.nplayersblind;
	if (memcmp(a, "nfriendsseated", 14)==0 && strlen(a)==14)			return _sym.nfriendsseated;
	if (memcmp(a, "nfriendsactive", 14)==0 && strlen(a)==14)			return _sym.nfriendsactive;
	if (memcmp(a, "nfriendsdealt", 13)==0 && strlen(a)==13)				return _sym.nfriendsdealt;
	if (memcmp(a, "nfriendsplaying", 15)==0 && strlen(a)==15)			return _sym.nfriendsplaying;
	if (memcmp(a, "nfriendsblind", 13)==0 && strlen(a)==13)				return _sym.nfriendsblind;
	if (memcmp(a, "nopponentsseated", 16)==0 && strlen(a)==16)			return _sym.nopponentsseated;
	if (memcmp(a, "nopponentsactive", 16)==0 && strlen(a)==16)			return _sym.nopponentsactive;
	if (memcmp(a, "nopponentsdealt", 15)==0 && strlen(a)==15)			return _sym.nopponentsdealt;
	if (memcmp(a, "nopponentsplaying", 17)==0 && strlen(a)==17)			return _sym.nopponentsplaying;
	if (memcmp(a, "nopponentsblind", 15)==0 && strlen(a)==15)			return _sym.nopponentsblind;
	if (memcmp(a, "nopponentschecking", 18)==0 && strlen(a)==18)		return _sym.nopponentschecking;
	if (memcmp(a, "nopponentscalling", 17)==0 && strlen(a)==17)			return _sym.nopponentscalling;
	if (memcmp(a, "nopponentsraising", 17)==0 && strlen(a)==17)			return _sym.nopponentsraising;
	if (memcmp(a, "nopponentsbetting", 17)==0 && strlen(a)==17)			return _sym.nopponentsbetting;
	if (memcmp(a, "nopponentsfolded", 16)==0 && strlen(a)==16)			return _sym.nopponentsfolded;
	if (memcmp(a, "nplayerscallshort", 17)==0 && strlen(a)==17)			return _sym.nplayerscallshort;
	if (memcmp(a, "nchairsdealtright", 17)==0 && strlen(a)==17)			return _sym.nchairsdealtright;
	if (memcmp(a, "nchairsdealtleft", 16)==0 && strlen(a)==16)			return _sym.nchairsdealtleft;
	if (memcmp(a, "playersseatedbits", 17)==0 && strlen(a)==17)			return _sym.playersseatedbits;
	if (memcmp(a, "playersactivebits", 17)==0 && strlen(a)==17)			return _sym.playersactivebits;
	if (memcmp(a, "playersdealtbits", 16)==0 && strlen(a)==16)			return _sym.playersdealtbits;
	if (memcmp(a, "playersplayingbits", 18)==0 && strlen(a)==18)		return _sym.playersplayingbits;
	if (memcmp(a, "playersblindbits", 16)==0 && strlen(a)==16)			return _sym.playersblindbits;
	if (memcmp(a, "opponentsseatedbits", 19)==0 && strlen(a)==19)		return _sym.opponentsseatedbits;
	if (memcmp(a, "opponentsactivebits", 19)==0 && strlen(a)==19)		return _sym.opponentsactivebits;
	if (memcmp(a, "opponentsdealtbits", 18)==0 && strlen(a)==18)		return _sym.opponentsdealtbits;
	if (memcmp(a, "opponentsplayingbits", 20)==0 && strlen(a)==20)		return _sym.opponentsplayingbits;
	if (memcmp(a, "opponentsblindbits", 18)==0 && strlen(a)==18)		return _sym.opponentsblindbits;
	if (memcmp(a, "friendsseatedbits", 17)==0 && strlen(a)==17)			return _sym.friendsseatedbits;
	if (memcmp(a, "friendsactivebits", 17)==0 && strlen(a)==17)			return _sym.friendsactivebits;
	if (memcmp(a, "friendsdealtbits", 16)==0 && strlen(a)==16)			return _sym.friendsdealtbits;
	if (memcmp(a, "friendsplayingbits", 18)==0 && strlen(a)==18)		return _sym.friendsplayingbits;
	if (memcmp(a, "friendsblindbits", 16)==0 && strlen(a)==16)			return _sym.friendsblindbits;

	//FLAGS
	if (memcmp(a, "fmax", 4)==0 && strlen(a)==4)						return _sym.fmax;
	if (memcmp(a, "f", 1)==0 && strlen(a)==2)							return _sym.f[a[1]-'0'];
	if (memcmp(a, "fbits", 5)==0 && strlen(a)==5)						return _sym.fbits;

	//COMMON CARDS
	if (memcmp(a, "ncommoncardspresent", 19)==0 && strlen(a)==19)		return _sym.ncommoncardspresent;
	if (memcmp(a, "ncommoncardsknown", 17)==0 && strlen(a)==17)			return _sym.ncommoncardsknown;
	if (memcmp(a, "nflopc", 6)==0 && strlen(a)==6)						return _sym.nflopc;

	//(UN)KNOWN CARDS
	if (memcmp(a, "nouts", 5)==0 && strlen(a)==5)						return _sym.nouts;
	if (memcmp(a, "ncardsknown", 11)==0 && strlen(a)==11)				return _sym.ncardsknown;
	if (memcmp(a, "ncardsunknown", 13)==0 && strlen(a)==13)				return _sym.ncardsunknown;
	if (memcmp(a, "ncardsbetter", 12)==0 && strlen(a)==12)				return _sym.ncardsbetter;

	//NHANDS
	if (memcmp(a, "nhands", 6)==0 && strlen(a)==6)						return _sym.nhands;
	if (memcmp(a, "nhandshi", 8)==0 && strlen(a)==8)					return _sym.nhandshi;
	if (memcmp(a, "nhandslo", 8)==0 && strlen(a)==8)					return _sym.nhandslo;
	if (memcmp(a, "nhandsti", 8)==0 && strlen(a)==8)					return _sym.nhandsti;
	if (memcmp(a, "prwinnow", 8)==0 && strlen(a)==8)					return _sym.prwinnow;
	if (memcmp(a, "prlosnow", 8)==0 && strlen(a)==8)					return _sym.prlosnow;

	//FLUSHES SETS STRAIGHTS
	if (memcmp(a, "nsuited", 7)==0 && strlen(a)==7)						return _sym.nsuited;
	if (memcmp(a, "nsuitedcommon", 13)==0 && strlen(a)==13)				return _sym.nsuitedcommon;
	if (memcmp(a, "tsuit", 5)==0 && strlen(a)==5)						return _sym.tsuit;
	if (memcmp(a, "tsuitcommon", 11)==0 && strlen(a)==11)				return _sym.tsuitcommon;
	if (memcmp(a, "nranked", 7)==0 && strlen(a)==7)						return _sym.nranked;
	if (memcmp(a, "nrankedcommon", 13)==0 && strlen(a)==13)				return _sym.nrankedcommon;
	if (memcmp(a, "trank", 5)==0 && strlen(a)==5)						return _sym.trank;
	if (memcmp(a, "trankcommon", 11)==0 && strlen(a)==11)				return _sym.trankcommon;
	if (memcmp(a, "nstraight", 9)==0 && strlen(a)==9)					return _sym.nstraight;
	if (memcmp(a, "nstraightcommon", 15)==0 && strlen(a)==15)			return _sym.nstraightcommon;
	if (memcmp(a, "nstraightfill", 13)==0 && strlen(a)==13)				return _sym.nstraightfill;
	if (memcmp(a, "nstraightfillcommon", 19)==0 && strlen(a)==19)		return _sym.nstraightfillcommon;
	if (memcmp(a, "nstraightflush", 14)==0 && strlen(a)==14)			return _sym.nstraightflush;
	if (memcmp(a, "nstraightflushcommon", 20)==0 && strlen(a)==20)		return _sym.nstraightflushcommon;
	if (memcmp(a, "nstraightflushfill", 18)==0 && strlen(a)==18)		return _sym.nstraightflushfill;
	if (memcmp(a, "nstraightflushfillcommon", 24)==0 && strlen(a)==24)  return _sym.nstraightflushfillcommon;

	//RANK BITS
	if (memcmp(a, "rankbits", 8)==0 && strlen(a)==8)					return _sym.rankbits;
	if (memcmp(a, "rankbitscommon", 14)==0 && strlen(a)==14)			return _sym.rankbitscommon;
	if (memcmp(a, "rankbitsplayer", 14)==0 && strlen(a)==14)			return _sym.rankbitsplayer;
	if (memcmp(a, "rankbitspoker", 13)==0 && strlen(a)==13)				return _sym.rankbitspoker;
	if (memcmp(a, "srankbits", 9)==0 && strlen(a)==9)					return _sym.srankbits;
	if (memcmp(a, "srankbitscommon", 15)==0 && strlen(a)==15)			return _sym.srankbitscommon;
	if (memcmp(a, "srankbitsplayer", 15)==0 && strlen(a)==15)			return _sym.srankbitsplayer;
	if (memcmp(a, "srankbitspoker", 14)==0 && strlen(a)==14)			return _sym.srankbitspoker;

	//RANK HI
	if (memcmp(a, "rankhi", 6)==0 && strlen(a)==6)						return _sym.rankhi;
	if (memcmp(a, "rankhicommon", 12)==0 && strlen(a)==12)				return _sym.rankhicommon;
	if (memcmp(a, "rankhiplayer", 12)==0 && strlen(a)==12)				return _sym.rankhiplayer;
	if (memcmp(a, "rankhipoker", 11)==0 && strlen(a)==11)				return _sym.rankhipoker;
	if (memcmp(a, "srankhi", 7)==0 && strlen(a)==7)						return _sym.srankhi;
	if (memcmp(a, "srankhicommon", 13)==0 && strlen(a)==13)				return _sym.srankhicommon;
	if (memcmp(a, "srankhiplayer", 13)==0 && strlen(a)==13)				return _sym.srankhiplayer;
	if (memcmp(a, "srankhipoker", 12)==0 && strlen(a)==12)				return _sym.srankhipoker;

	//RANK LO
	if (memcmp(a, "ranklo", 6)==0 && strlen(a)==6)						return _sym.ranklo;
	if (memcmp(a, "ranklocommon", 12)==0 && strlen(a)==12)				return _sym.ranklocommon;
	if (memcmp(a, "rankloplayer", 12)==0 && strlen(a)==12)				return _sym.rankloplayer;
	if (memcmp(a, "ranklopoker", 11)==0 && strlen(a)==11)				return _sym.ranklopoker;
	if (memcmp(a, "sranklo", 7)==0 && strlen(a)==7)						return _sym.sranklo;
	if (memcmp(a, "sranklocommon", 13)==0 && strlen(a)==13)				return _sym.sranklocommon;
	if (memcmp(a, "srankloplayer", 13)==0 && strlen(a)==13)				return _sym.srankloplayer;
	if (memcmp(a, "sranklopoker", 12)==0 && strlen(a)==12)				return _sym.sranklopoker;

	//TIME
	if (memcmp(a, "elapsed", 7)==0 && strlen(a)==7)						return _sym.elapsed;
	if (memcmp(a, "elapsedhand", 11)==0 && strlen(a)==11)				return _sym.elapsedhand;
	if (memcmp(a, "elapsedauto", 11)==0 && strlen(a)==11)				return _sym.elapsedauto;
	if (memcmp(a, "elapsedtoday", 12)==0 && strlen(a)==12)				return _sym.elapsedtoday;
	if (memcmp(a, "elapsed1970", 11)==0 && strlen(a)==11)				return _sym.elapsed1970;
	if (memcmp(a, "clocks", 6)==0 && strlen(a)==6)						return _sym.clocks;
	if (memcmp(a, "nclockspersecond", 16)==0 && strlen(a)==16)			return _sym.nclockspersecond;
	if (memcmp(a, "ncps", 4)==0 && strlen(a)==4)						return _sym.ncps;


	//AUTOPLAYER
	if (memcmp(a, "myturnbits", 10)==0 && strlen(a)==10)				return _sym.myturnbits;
	if (memcmp(a, "ismyturn", 8)==0 && strlen(a)==8)					return _sym.ismyturn;
	if (memcmp(a, "issittingin", 11)==0 && strlen(a)==11)				return _sym.issittingin;
	if (memcmp(a, "issittingout", 12)==0 && strlen(a)==12)				return _sym.issittingout;
	if (memcmp(a, "isautopost", 10)==0 && strlen(a)==10)				return _sym.isautopost;
	if (memcmp(a, "isfinalanswer", 13)==0 && strlen(a)==13)				return _sym.isfinalanswer;

	//HISTORY
	if (memcmp(a, "nplayersround", 13)==0 && strlen(a)==13)				return _sym.nplayersround[4];
	if (memcmp(a, "nplayersround", 13)==0 && strlen(a)==14)				return _sym.nplayersround[a[13]-'0'-1];
	if (memcmp(a, "prevaction", 10)==0 && strlen(a)==10)				return _sym.prevaction;
	if (memcmp(a, "didchec", 7)==0 && strlen(a)==7)						return _sym.didchec[4];
	if (memcmp(a, "didcall", 7)==0 && strlen(a)==7)						return _sym.didcall[4];
	if (memcmp(a, "didrais", 7)==0 && strlen(a)==7)						return _sym.didrais[4];
	if (memcmp(a, "didswag", 7)==0 && strlen(a)==7)						return _sym.didswag[4];
	if (memcmp(a, "nbetsround", 10)==0 && strlen(a)==10)				return _sym.nbetsround[4];
	if (memcmp(a, "nbetsround", 10)==0 && strlen(a)==11)				return _sym.nbetsround[a[10]-'0'-1];
	if (memcmp(a, "didchecround", 12)==0 && strlen(a)==13)				return _sym.didchec[a[12]-'0'-1];
	if (memcmp(a, "didcallround", 12)==0 && strlen(a)==13)				return _sym.didcall[a[12]-'0'-1];
	if (memcmp(a, "didraisround", 12)==0 && strlen(a)==13)				return _sym.didrais[a[12]-'0'-1];
	if (memcmp(a, "didswaground", 12)==0 && strlen(a)==13)				return _sym.didswag[a[12]-'0'-1];

	//run$ ron$
	if (memcmp(a, "ron$royfl", 9)==0 && strlen(a)==9)					return _sym.ron$royfl;
	if (memcmp(a, "ron$strfl", 9)==0 && strlen(a)==9)					return _sym.ron$strfl;
	if (memcmp(a, "ron$4kind", 9)==0 && strlen(a)==9)					return _sym.ron$4kind;
	if (memcmp(a, "ron$fullh", 9)==0 && strlen(a)==9)					return _sym.ron$fullh;
	if (memcmp(a, "ron$flush", 9)==0 && strlen(a)==9)					return _sym.ron$flush;
	if (memcmp(a, "ron$strai", 9)==0 && strlen(a)==9)					return _sym.ron$strai;
	if (memcmp(a, "ron$3kind", 9)==0 && strlen(a)==9)					return _sym.ron$3kind;
	if (memcmp(a, "ron$2pair", 9)==0 && strlen(a)==9)					return _sym.ron$2pair;
	if (memcmp(a, "ron$1pair", 9)==0 && strlen(a)==9)					return _sym.ron$1pair;
	if (memcmp(a, "ron$hcard", 9)==0 && strlen(a)==9)					return _sym.ron$hcard;
	if (memcmp(a, "ron$total", 9)==0 && strlen(a)==9)					return _sym.ron$total;
	if (memcmp(a, "ron$pokervalmax", 15)==0 && strlen(a)==15)			return _sym.ron$pokervalmax;
	if (memcmp(a, "ron$prnuts", 10)==0 && strlen(a)==10)				return _sym.ron$prnuts;
	if (memcmp(a, "ron$prbest", 10)==0 && strlen(a)==10)				return _sym.ron$prbest;
	if (memcmp(a, "ron$clocks", 10)==0 && strlen(a)==10)				return _sym.ron$clocks;
	if (memcmp(a, "run$royfl", 9)==0 && strlen(a)==9)					return _sym.run$royfl;
	if (memcmp(a, "run$strfl", 9)==0 && strlen(a)==9)  					return _sym.run$strfl;
	if (memcmp(a, "run$4kind", 9)==0 && strlen(a)==9)  					return _sym.run$4kind;
	if (memcmp(a, "run$fullh", 9)==0 && strlen(a)==9)  					return _sym.run$fullh;
	if (memcmp(a, "run$flush", 9)==0 && strlen(a)==9)  					return _sym.run$flush;
	if (memcmp(a, "run$strai", 9)==0 && strlen(a)==9)  					return _sym.run$strai;
	if (memcmp(a, "run$3kind", 9)==0 && strlen(a)==9)  					return _sym.run$3kind;
	if (memcmp(a, "run$2pair", 9)==0 && strlen(a)==9)  					return _sym.run$2pair;
	if (memcmp(a, "run$1pair", 9)==0 && strlen(a)==9)  					return _sym.run$1pair;
	if (memcmp(a, "run$hcard", 9)==0 && strlen(a)==9)  					return _sym.run$hcard;
	if (memcmp(a, "run$total", 9)==0 && strlen(a)==9)  					return _sym.run$total;
	if (memcmp(a, "run$pokervalmax", 15)==0 && strlen(a)==15)			return _sym.run$pokervalmax;
	if (memcmp(a, "run$prnuts", 10)==0 && strlen(a)==10)				return _sym.run$prnuts;
	if (memcmp(a, "run$prbest", 10)==0 && strlen(a)==10)				return _sym.run$prbest;
	if (memcmp(a, "run$clocks", 10)==0 && strlen(a)==10)				return _sym.run$clocks;

	// Versus
	if (memcmp(a, "vs$nhands", 10)==0 && strlen(a)==10)					return _sym.vs$nhands;
	if (memcmp(a, "vs$nhandshi", 10)==0 && strlen(a)==10)				return _sym.vs$nhandshi;
	if (memcmp(a, "vs$nhandsti", 10)==0 && strlen(a)==10)				return _sym.vs$nhandsti;
	if (memcmp(a, "vs$nhandslo", 10)==0 && strlen(a)==10)				return _sym.vs$nhandslo;
	if (memcmp(a, "vs$prwin", 8)==0 && strlen(a)==8)  					return _sym.vs$prwin;
	if (memcmp(a, "vs$prtie", 8)==0 && strlen(a)==8)  					return _sym.vs$prtie;
	if (memcmp(a, "vs$prlos", 8)==0 && strlen(a)==8)  					return _sym.vs$prlos;
	if (memcmp(a, "vs$prwinhi", 10)==0 && strlen(a)==10)				return _sym.vs$prwinhi;
	if (memcmp(a, "vs$prtiehi", 10)==0 && strlen(a)==10)  				return _sym.vs$prtiehi;
	if (memcmp(a, "vs$prloshi", 10)==0 && strlen(a)==10)  				return _sym.vs$prloshi;
	if (memcmp(a, "vs$prwinti", 10)==0 && strlen(a)==10)  				return _sym.vs$prwinti;
	if (memcmp(a, "vs$prtieti", 10)==0 && strlen(a)==10)  				return _sym.vs$prtieti;
	if (memcmp(a, "vs$prlosti", 10)==0 && strlen(a)==10)  				return _sym.vs$prlosti;
	if (memcmp(a, "vs$prwinlo", 10)==0 && strlen(a)==10)  				return _sym.vs$prwinlo;
	if (memcmp(a, "vs$prtielo", 10)==0 && strlen(a)==10)  				return _sym.vs$prtielo;
	if (memcmp(a, "vs$prloslo", 10)==0 && strlen(a)==10)  				return _sym.vs$prloslo;

	// GameState symbols
	if (memcmp(a,"lastraised",10)==0 && strlen(a)==11)  				return p_game_state->LastRaised(a[10]-'0');
	if (memcmp(a,"raisbits",8)==0 && strlen(a)==9)  					return p_game_state->RaisBits(a[8]-'0');
	if (memcmp(a,"callbits",8)==0 && strlen(a)==9)  					return p_game_state->CallBits(a[8]-'0');
	if (memcmp(a,"foldbits",8)==0 && strlen(a)==9)  					return p_game_state->FoldBits(a[8]-'0');
	if (memcmp(a,"oppdealt",8)==0 && strlen(a)==8)  					return p_game_state->oppdealt();
	if (memcmp(a,"floppct",7)==0 && strlen(a)==7)  						return p_game_state->FlopPct();
	if (memcmp(a,"turnpct",7)==0 && strlen(a)==7)  						return p_game_state->TurnPct();
	if (memcmp(a,"riverpct",8)==0 && strlen(a)==8)  					return p_game_state->RiverPct();
	if (memcmp(a,"avgbetspf",9)==0 && strlen(a)==9)  					return p_game_state->AvgBetsPf();
	if (memcmp(a,"tablepfr",8)==0 && strlen(a)==8)  					return p_game_state->TablePfr();
	if (memcmp(a,"maxbalance",10)==0 && strlen(a)==10)  				return p_game_state->max_balance();
	if (memcmp(a,"handsplayed",11)==0 && strlen(a)==11)  				return p_game_state->hands_played();
	if (memcmp(a,"balance_rank",12)==0 && strlen(a)==13)  				return p_game_state->SortedBalance(a[12]-'0');
	if (memcmp(a,"hi_",3)==0)
	{
		char	sym[50] = {0};
		int		round = 0;
		strcpy_s(sym, 50, &a[3]);
		round = sym[strlen(sym)-1]-'0';
		sym[strlen(sym)-1] = '\0';
		return p_game_state->OHSymHist(sym, round);
	}

	// PokerTracker symbols
	if (memcmp(a,"pt_",3)==0 || memcmp(a,"ptt_",4)==0)					return p_pokertracker_thread->ProcessQuery(a);


	*e = ERR_INVALID_SYM;
	return 0.0;

}

const double CSymbols::IsHand(const char *a, int *e)
{
	int				cardrank[2] = {0}, temp;
	int				suited = 0;  //0=not specified, 1=suited, 2=offsuit
	int				i, cardcnt = 0;
	int				plcardrank[2] = {0}, plsuited = 0;

	if (strlen(a)<=1)
	{
		if (e)
			*e = ERR_INVALID_SYM;
		return 0;
	}

	// passed in symbol query
	for (i=1; i<(int) strlen(a); i++)
	{
		if (a[i]>='2' && a[i]<='9')
			cardrank[cardcnt++] =  a[i] - '0';

		else if (a[i]=='T' || a[i]=='t')
			cardrank[cardcnt++] = 10;

		else if (a[i]=='J' || a[i]=='j')
			cardrank[cardcnt++] = 11;

		else if (a[i]=='Q' || a[i]=='q')
			cardrank[cardcnt++] = 12;

		else if (a[i]=='K' || a[i]=='k')
			cardrank[cardcnt++] = 13;

		else if (a[i]=='A' || a[i]=='a')
			cardrank[cardcnt++] = 14;

		else if (a[i]=='X' || a[i]=='x')
			cardrank[cardcnt++] = -1;

		else if (a[i]=='S' || a[i]=='s')
			suited=1;

		else if (a[i]=='O' || a[i]=='o')
			suited=2;

		else
		{
			if (e)
				*e = ERR_INVALID_SYM;
			return 0;
		}
	}

	if (!_user_chair_confirmed)
		return 0;

	// sort
	if (cardrank[1] > cardrank[0])
	{
		temp = cardrank[0];
		cardrank[0] = cardrank[1];
		cardrank[1] = temp;
	}

	// playercards
	plcardrank[0] = Deck_RANK(p_scraper->card_player(_sym.userchair, 0))+2;
	plcardrank[1] = Deck_RANK(p_scraper->card_player(_sym.userchair, 1))+2;
	if (plcardrank[1] > plcardrank[0])
	{
		temp = plcardrank[0];
		plcardrank[0] = plcardrank[1];
		plcardrank[1] = temp;
	}
	if (Deck_SUIT(p_scraper->card_player(_sym.userchair, 0)) == 
		Deck_SUIT(p_scraper->card_player(_sym.userchair, 1)))
	{
		plsuited = 1;
	}
	else
	{
		plsuited = 0;
	}

	// check for non suited/offsuit match
	if (suited==1 && plsuited==0)
		return 0;

	if (suited==2 && plsuited==1)
		return 0;

	// check for non rank match
	// two wildcards
	if (cardrank[0]==-1 && cardrank[1]==-1)
		return 1;

	// one card passed in, or one with a wildcard
	if (cardrank[1]==0 || cardrank[1]==-1)
	{
		if (cardrank[0] != plcardrank[0] &&
				cardrank[0] != plcardrank[1])
		{
			return 0;
		}
	}

	// two cards passed in
	else
	{
		if (cardrank[0]!=-1 && cardrank[0]!=plcardrank[0])
			return 0;

		if (cardrank[1]!=-1 && cardrank[1]!=plcardrank[1])
			return 0;
	}

	return 1;
}

const double CSymbols::Chair$(const char *a)
{
	int i = 0;

	for (i=0; i<p_tablemap->s$items()->num_chairs; i++)
	{
		if (p_scraper->player_name(i).Find(&a[6])!=-1)
			return i;
	}

	return -1;
}

const double CSymbols::Chairbit$(const char *a)
{
	int i = 0, bits = 0;

	for (i=0; i<p_tablemap->s$items()->num_chairs; i++)
	{
		if (p_scraper->player_name(i).Find(&a[9])!=-1)
			bits |= (1<<i);
	}

	return bits;
}

const int CSymbols::GetSiteId (void)
{
	int			i = 0;
	CString		sym = "";
	double		result = 0.;

	if (_sym.isppro)
	{
		return 3;
	}
	else
	{
		if (_sym.ismanual)
		{
			for (i=0; i<=20; i++)
			{
				if ((p_tablemap->s$items()->network == networkid[i]) && strlen(networkid[i]))
					return i;
			}
		}
		else
		{
			//Is s$sitename one of the supported PT sites?  Return the proper site_id for db queries.
			for (i=0; i<=20; i++)
			{
				sym.Format("sitename$%s", networkid[i]);

				int e = SUCCESS;
				result = GetSymbolVal(sym.MakeLower().GetString(), &e);

				if (result && strlen(networkid[i]))
					return i;
			}
			//Is s$network one of the supported PT sites?  Return the proper site_id for db queries.
			for (i=0; i<=20; i++)
			{
				sym.Format("network$%s", networkid[i]);

				int e = SUCCESS;
				result = GetSymbolVal(sym.MakeLower().GetString(), &e);

				if (result && strlen(networkid[i]))
					return i;
			}
		}
	}

	return -1 ;
}