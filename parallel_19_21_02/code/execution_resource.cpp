// Copyright (c) 2018 Thomas Heller

#include <hpx/runtime/resource/partitioner.hpp>

#include <iostream>

int main(int argc, char **argv)
{
    hpx::resource::partitioner rp(argc, argv);

    std::cout << "NUMA domains:\n";

    for (auto& domain: rp.numa_domains())
    {
        std::cout << domain.id() << "\n";
    }

    std::cout << "Cores within first NUMA domain:\n";

    for (auto& core: rp.numa_domains()[0].cores())
    {
        std::cout << core.id() << "\n";
    }

    std::cout << "Hyper threads within the first core:\n";

    for (auto& pu: rp.numa_domains()[0].cores()[0].pus())
    {
        std::cout << pu.id() << "\n";
    }
}
