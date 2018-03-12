#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include<parsian_msgs/parsian_world_model.h>
#include<parsian_msgs/ssl_refree_wrapper.h>
#include<parsian_ai/util/worldmodel.h>
class Behavior
{
public:
	Behavior(std::string _name, std::string _description) : name(_name), description(_description) {

	}
	virtual int execute() = 0;
	virtual double eval() = 0;
	std::string getName() const { return name; }
	std::string getDescription() const { return description; }
	double probability() const { return prob; }
	void updateReferee(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref) { /*TODO FIX*/ }
	void updateWM(const parsian_msgs::parsian_world_modelConstPtr& _wm) { wm->update( _wm); }
protected:
	double prob;
	const std::string name;
	const std::string description;
};

#endif // BEHAVIOR_H
