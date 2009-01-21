#ifndef INC_CVERSUS_H
#define INC_CVERSUS_H

#include "..\CCritSec\CCritSec.h"

extern class CVersus 
{
public:
	// public functions
	CVersus();
	~CVersus();
	bool GetCounts(void);
	double GetSymbol(const char *a, int *e);

private:
	// private functions and variables - not available via accessors or mutators
	void DoCalc(const CardMask plCards, const CardMask oppCards, const CardMask comCards, 
				unsigned int *wintemp, unsigned int *tietemp, unsigned int *lostemp);

	int				_nhands, _nwin, _ntie, _nlos, _nhandshi, _nhandsti, _nhandslo;
	double			_vsprwin, _vsprtie, _vsprlos;
	double			_vsprwinhi, _vsprtiehi, _vsprloshi;
	double			_vsprwinti, _vsprtieti, _vsprlosti;
	double			_vsprwinlo, _vsprtielo, _vsprloslo;
	int				_nlistwin[MAX_HAND_LISTS], _nlisttie[MAX_HAND_LISTS], _nlistlos[MAX_HAND_LISTS];
	int				_versus_fh;

	CCritSec		m_critsec;

} *p_versus;

#endif INC_CVERSUS_H