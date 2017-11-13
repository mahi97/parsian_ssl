#ifndef PARSIAN_SANDBOX_SANDBOX_H_H
#define PARSIAN_SANDBOX_SANDBOX_H_H

class SandBox {
public:
    SandBox();
    ~SandBox();

    virtual void init() = 0;
    virtual void execute() = 0;
    static double durationTime;

private:
protected:
};

#endif //PARSIAN_SANDBOX_SANDBOX_H_H
