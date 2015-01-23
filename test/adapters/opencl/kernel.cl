__kernel void concKernel1(__global float* a)
{
    int z = 0;

    int tx = get_global_id(0);

    for (int i = 0; i < 10; i++)
    {
        a[tx] = a[tx] * a[tx] / 47 + a[tx] * a[tx] / 17;
    }
}

__kernel void concKernel2(__global float* a)
{
    int z = 0;

    int tx = get_global_id(0);

    for (int i = 0; i < 100; i++)
    {
        a[tx] = a[tx] * a[tx] / 47 + a[tx] * a[tx] / 17;
    }
}

__kernel void concKernel3(__global float* a)
{
    int z = 0;

    int tx = get_global_id(0);

    for (int i = 0; i < 1000; i++)
    {
        a[tx] = a[tx] * a[tx] / 47 + a[tx] * a[tx] / 17;
    }
}

__kernel void concKernel4(__global float* a)
{
    int z = 0;

    int tx = get_global_id(0);

    for (int i = 0; i < 10000; i++)
    {
        a[tx] = a[tx] * a[tx] / 47 + a[tx] * a[tx] / 17;
    }
}

__kernel void concKernel5(__global float* a)
{
    int z = 0;

    int tx = get_global_id(0);

    for (int i = 0; i < 100000; i++)
    {
        a[tx] = a[tx] * a[tx] / 47 + a[tx] * a[tx] / 17;
    }
}

__kernel void concKernel6(__global float* a)
{
    int z = 0;

    int tx = get_global_id(0);

    for (int i = 0; i < 1000000; i++)
    {
        a[tx] = a[tx] * a[tx] / 47 + a[tx] * a[tx] / 17;
    }
}

__kernel void concKernel7(__global float* a)
{
    int z = 0;

    int tx = get_global_id(0);

    for (int i = 0; i < 10000000; i++)
    {
        a[tx] = a[tx] * a[tx] / 47 + a[tx] * a[tx] / 17;
    }
}
