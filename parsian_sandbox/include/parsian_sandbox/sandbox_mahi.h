//
// Created by parsian-ai on 11/13/17.
//

#ifndef PARSIAN_SANDBOX_SANDBOX_MAHI_H
#define PARSIAN_SANDBOX_SANDBOX_MAHI_H

#include <parsian_sandbox/sandbox.h>

class SandBoxMahi : public SandBox {
public:
    SandBoxMahi();
    ~SandBoxMahi();

    void init();
    void execute();

private:
protected:
};

#endif //PARSIAN_SANDBOX_SANDBOX_MAHI_H
