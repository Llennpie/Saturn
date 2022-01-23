#ifndef MPLATFORM_H
#define MPLATFORM_H

class PlatformImpl {
public:
    virtual void pre_init() = 0;
    virtual void init() = 0;
    virtual void update() = 0;
}

#endif