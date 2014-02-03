#ifndef FUNC1_H
#define FUNC1_H
#ifndef DELAY
#define DELAY 0.01
#endif
#include "function.h"

class func1 : public function
{
private:
    int f_count;
public:
    func1();
    virtual double
    eval( double x );
    virtual int
    get_count();
};

#endif
