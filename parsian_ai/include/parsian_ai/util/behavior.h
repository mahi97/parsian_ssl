#ifndef BEHAVIOR_H
#define BEHAVIOR_H


class Behavior
{
public:
	Behavior(std::string _name, std::string _description) : name(_name), description(_description) {

	}
	int execute() = 0;
	double eval() = 0;
	std::string getName() const { return name; }
	std::string getDescription() const { return description; }
	double probability() { return prob; } const
protected:
	double prob;
	const std::string name;
	const std::string description;
};

#endif // BEHAVIOR_H
