/*
	   Author	:	spud
	   Date		:	2007-MAR-08

	  Notation: P(i,n)=probability that player i finishes in nth place
	  P(w|z)=probability that w occurs given that z occurs
	  I=i's stack
	  T=SUM[I]

	  P(i,1)=I/T
	  P(i,n)=SUM(j!=i)[P(j,1)*P(i,n|j,1)]

	  ICM assumes that P(i,n|j,1) = (P(i,n-1) in a game where j has been removed)
*/

#include "stdafx.h"

#include "CICMCalculator.h"

#include "CSymbols.h"
#include "CPreferences.h"

double P(int i, int n, double *s, int N)
{
	double p = 0.;

	if (n==1)
	{
		double I = s[i];
		double T = 0;

		for (int k=0; k < N; k++) T+=s[k];

		p = (T > 0.) ? I / T : 0.;
	}
	else
	{
		for (int j=0; j < N; j++)
		{
			if (j != i)
			{
				double *ss = (double *) calloc(N-1,sizeof(double));
				int ii = (i > j) ? i-1 : i;
				memcpy(ss, s, j * sizeof(double));
				memcpy(ss + j, s + (j + 1), (N - j -1) * sizeof(double));

				p += P(j, 1, s, N) * P(ii, n-1, ss, N-1);

				free(ss);
			}
		}
	}
	return p;
}

CICMCalculator::CICMCalculator ()
{
	__SEH_SET_EXCEPTION_HANDLER
}

CICMCalculator::~CICMCalculator ()
{
}

const double CICMCalculator::ProcessQueryICM(const char* pquery, int *e)
{
	double		prizes[MAX_PLAYERS] = {0};
	double		stacks[MAX_PLAYERS] = {0};
	int			i = 0, j = 0;
	prizes[0] = prefs.icm_prize1();
	prizes[1] = prefs.icm_prize2();
	prizes[2] = prefs.icm_prize3();
	prizes[3] = prefs.icm_prize4();

	int			sym_userchair = (int) p_symbols->sym()->userchair;
	int			sym_opponentsplayingbits = (int) p_symbols->sym()->opponentsplayingbits;
	int			sym_nopponentsplaying = (int) p_symbols->sym()->nopponentsplaying;
	int			sym_nplayersseated =  (int) p_symbols->sym()->nplayersseated;
	int			sym_playersseatedbits =  (int) p_symbols->sym()->playersseatedbits;
	double		sym_pot = p_symbols->sym()->pot;
	double		sym_call = p_symbols->sym()->call;
	double		sym_currentbet[MAX_PLAYERS], stacks_at_hand_start[MAX_PLAYERS];

	for (i=0; i<MAX_PLAYERS; i++)
	{
		sym_currentbet[i] = p_symbols->sym()->currentbet[i];
		stacks_at_hand_start[i] = p_symbols->stacks_at_hand_start(i);
	}

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if ((sym_playersseatedbits>>i)&1)
		{
			stacks[i] = stacks_at_hand_start[i] - sym_currentbet[i];
			//ASSERT(stacks[i] >= 0.0001);
		}
	}

	if (strncmp(pquery,"_fold",5)==0)
	{
		double win = sym_pot / sym_nopponentsplaying;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if ((sym_opponentsplayingbits>>i)&1)
			{
				stacks[i] += win;
			}
		}
	}

	else if (strncmp(pquery,"_callwin",8)==0)
	{
		double call = sym_call;

		if (stacks[sym_userchair] < call)
		{
			double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];

			for (i = 0; i < MAX_PLAYERS; i++)
			{
				if ((sym_opponentsplayingbits>>i)&1 && myTotalBet < sym_currentbet[i])
				{
					double extra = sym_currentbet[i] - myTotalBet;

					stacks[i] += extra;
					sym_pot -= extra;
				}
			}
		}
		stacks[sym_userchair] += sym_pot;
	}

	else if (strncmp(pquery,"_calllose",9)==0)
	{
		double mycall = min(sym_call,stacks[sym_userchair]);
		double win = (sym_pot + mycall) / sym_nopponentsplaying;

		stacks[sym_userchair] -= mycall;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if ((sym_opponentsplayingbits>>i)&1)
			{
				stacks[i] += win;
			}
		}
	}

	else if (strncmp(pquery,"_calltie",8)==0)
	{
		double win = 0.;

		if (stacks[sym_userchair] < sym_call)
		{
			double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];

			for (i = 0; i < MAX_PLAYERS; i++)
			{
				if ((sym_opponentsplayingbits>>i)&1 && myTotalBet<sym_currentbet[i])
				{
					double extra = sym_currentbet[i] - myTotalBet;

					stacks[i] += extra;
					sym_pot -= extra;
				}
			}
		}
		sym_pot += min(sym_call, stacks[sym_userchair]);
		win = sym_pot / (sym_nopponentsplaying +1);
		stacks[sym_userchair] += win;
		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if ((sym_opponentsplayingbits>>i)&1)
			{
				stacks[i] += win;
			}
		}
	}

	else if (strncmp(pquery,"_alliwin",8)==0)
	{
		if (isdigit(pquery[8]))
		{
			//assume callers are n smallest stacks
			bool callers[MAX_PLAYERS] = {0};
			int ncallers = min(pquery[8]-'0', sym_nopponentsplaying);

			for (i = 0; i < ncallers; i++)
			{
				int jsmallest = -1;
				double smalleststack = MAX_DOUBLE;

				for (j = 0; j < MAX_PLAYERS; j++)
				{
					if ((sym_opponentsplayingbits>>j)&1)
					{
						if (!callers[j] && (sym_currentbet[j] + stacks[j]) < smalleststack)
						{
							smalleststack = sym_currentbet[j] + stacks[j];
							jsmallest = j;
						}
					}
				}
				if (jsmallest > -1)
				{
					double oppCurrentBet = sym_currentbet[jsmallest];
					double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];
					double extra = ((oppCurrentBet + stacks[jsmallest]) > myTotalBet) ? myTotalBet - oppCurrentBet :stacks[jsmallest];
					stacks[jsmallest] -= extra;
					callers[jsmallest] = true;
					sym_pot+=extra;
				}
			}
			stacks[sym_userchair]+=sym_pot;
		}
		else
		{
			//assume only one particular caller
			int oppChair = GetChairFromDealPos(pquery + 8);

			if (oppChair >= 0)
			{
				double oppCurrentBet = sym_currentbet[oppChair];
				double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];
				double extra = ((oppCurrentBet + stacks[oppChair]) > myTotalBet) ? myTotalBet - oppCurrentBet :stacks[oppChair];

				stacks[oppChair] -= extra;
				stacks[sym_userchair]+= (sym_pot + extra);
			}
		}
	}

	else if (strncmp(pquery,"_allilose",9)==0)
	{
		if (isdigit(pquery[9]))
		{
			//assume callers are n biggest stacks
			int ncallers = min(pquery[9]-'0', sym_nopponentsplaying);

			if (ncallers > 0)
			{
				bool callers[MAX_PLAYERS] = {0};
				int *biggest =(int *) calloc(ncallers, sizeof(int));
				double *sidepots = (double *)calloc(ncallers, sizeof(double));
				double mybet = 0.;

				memset(sidepots,0, ncallers * sizeof(double));

				for (i = 0; i < ncallers; i++)
				{
					int jbiggest = -1;
					double biggeststack = 0.;

					for (j = 0; j < MAX_PLAYERS; j++)
					{
						if ((sym_opponentsplayingbits>>j)&1)
						{
							if (!callers[j] && (sym_currentbet[j] + stacks[j]) > biggeststack)
							{
								biggeststack = sym_currentbet[j] + stacks[j];
								jbiggest = j;
							}
						}
					}
					biggest[i] = jbiggest;
					if (jbiggest > -1)
					{
						callers[jbiggest] = true;
						stacks[jbiggest] += sym_currentbet[jbiggest];
						sym_pot -= sym_currentbet[jbiggest];
					}
				}
				stacks[sym_userchair] += sym_currentbet[sym_userchair];
				sym_pot -= sym_currentbet[sym_userchair];
				for (i = (ncallers -1); i>=0; i--)
				{
					ASSERT(biggest[i] >= 0);
					sidepots[i] = min(stacks[biggest[i]], stacks[sym_userchair]) - mybet;
					mybet += sidepots[i];
				}
				stacks[sym_userchair] -= mybet;
				sidepots[ncallers - 1] += sym_pot;
				for (i = 0; i<ncallers; i++)
				{
					double win = sidepots[i] / (i + 1);

					for (j = 0; j<=i; j++)
					{
						ASSERT(biggest[j] >= 0);
						stacks[biggest[j]] += win;
					}
				}
				free(biggest);
				free(sidepots);
			}
		}
		else
		{
			//assume only one particular caller
			int oppChair = GetChairFromDealPos(pquery + 9);

			if (oppChair >= 0)
			{
				double oppTotalBet = sym_currentbet[oppChair] + stacks[oppChair];
				double myTotalBet = sym_currentbet[sym_userchair] + stacks[sym_userchair];
				double extra = (oppTotalBet > myTotalBet) ? stacks[sym_userchair] : oppTotalBet - sym_currentbet[sym_userchair];

				stacks[oppChair]+= (sym_pot + extra);
				stacks[sym_userchair]-= extra;
			}
		}
	}

	else if (strncmp(pquery,"_allitie",8)==0)
	{
		//assume only one particular caller
		int oppChair = GetChairFromDealPos(pquery + 8);

		if (oppChair >= 0)
		{
			stacks[oppChair]+= sym_pot / 2;
			stacks[sym_userchair]+= sym_pot / 2;
		}
	}

	else
	{
		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if ((sym_playersseatedbits>>i)&1)
			{
				stacks[i] += sym_currentbet[i];
			}
		}
	}

	return EquityICM(stacks, prizes, MAX_PLAYERS, sym_userchair);
}

double CICMCalculator::EquityICM(double *stacks, double *prizes, int playerNB, int player)
{
	double ICM = 0.;
	int i = 0;

	int			sym_opponentsseatedbits = (int) p_symbols->sym()->opponentsseatedbits;

	for (i = 0; i < playerNB; i++)
	{
		//printf("player %d  stack = %1.2f \n", i, stacks[i]);
	}

	// Degenerate case when the player's stack is zero.  Place
	// him after all opponents who had stacks at the start of this
	// hand.  This will not tie-break if two players bust on the
	// same hand.
	if (stacks[player] == 0.)
	{
		int place = 0;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if ((sym_opponentsseatedbits >> i) & 1)
				place++;
		}

		return prizes[place];

	}

	i = 0;
	while (i < playerNB && prizes[i] > 0.)
	{
		double p = P(player, i + 1, stacks, playerNB);

		//printf("=> prob place %d = %1.4f \n", i + 1, p);
		ICM += prizes[i] * p;
		i++;
	}

	return ICM;
}

double CICMCalculator::GetPlayerCurrentBet(int pos)
{
	return p_symbols->sym()->currentbet[pos];
}

int CICMCalculator::GetChairFromDealPos(const char* pquery)
{
	int			sym_nplayersseated =  (int) p_symbols->sym()->nplayersseated;
	int			sym_playersseatedbits =  (int) p_symbols->sym()->playersseatedbits;
	int			sym_dealerchair =  (int) p_symbols->sym()->dealerchair;

	int			chair = -1;

	if (sym_nplayersseated > 0)
	{
		int dealPos = -1;

		if (strcmp(pquery,"SB")==0)
			dealPos = 1;
		else if (strcmp(pquery,"BB")==0)
			dealPos = 2;
		else if (strcmp(pquery,"UTG")==0)
			dealPos = 3;
		else if (strcmp(pquery,"UTG1")==0)
			dealPos = 4;
		else if (strcmp(pquery,"UTG2")==0)
			dealPos = 5;
		else if (strcmp(pquery,"UTG3")==0)
			dealPos = 6;
		else if (strcmp(pquery,"UTG4")==0)
			dealPos = 7;
		else if (strcmp(pquery,"UTG5")==0)
			dealPos = 8;
		else if (strcmp(pquery,"UTG6")==0)
			dealPos = 9;
		else if (strcmp(pquery,"D")==0)
			dealPos = 0;
		else if (strcmp(pquery,"CO")==0)
			dealPos = sym_nplayersseated - 1;

		if (dealPos >= 0)
		{
			chair = sym_dealerchair;

			while (dealPos > 0)
			{
				chair = (chair + 1) % MAX_PLAYERS;

				if ((sym_playersseatedbits>>chair)&1)
					dealPos--;
			}
		}
	}

	return chair;
}
