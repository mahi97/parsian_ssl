#ifndef THEIRPENALTYF_H
#define THEIRPENALTYF_H

#include "masterplay.h"

class CTheirPenalty : public CMasterPlay{
public:
	CTheirPenalty();
	~CTheirPenalty();
	void execute_x();
	void init(const QList<CAgent*>& _agents);
private:
	void reset();
};

#endif // THEIRPENALTYF_H
