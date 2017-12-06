#ifndef OURPENALTY_H
#define OURPENALTY_H

#include "parsian_ai/plays/masterplay.h"

class COurPenalty : public CMasterPlay{
public:
	COurPenalty();
	~COurPenalty();
	void penaltyKick();
	void execute_x();
	void init(QList <CAgent*> _agents);
private:
	void reset();
};

#endif // OURPENALTY_H
