#ifndef FUNCTION_H
#define FUNCTION_H

class function
{
public:
    virtual double
    eval( double x ) = 0;
    virtual int
    get_count() = 0;
};

#endif
