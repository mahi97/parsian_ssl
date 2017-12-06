#ifndef THEIRINDIRECT_H
#define THEIRINDIRECT_H

#include "masterplay.h"

class CTheirIndirect : public CMasterPlay{
public:
	CTheirIndirect();
	~CTheirIndirect();
	void execute_x();
	void init(QList <CAgent*> _agents);
private:
	void reset();
};

#endif // THEIRINDIRECT_H
