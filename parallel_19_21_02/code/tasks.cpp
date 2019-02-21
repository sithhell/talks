// Copyright (c) 2018 Thomas Heller

#include <hpx/runtime/resource/partitioner.hpp>
#include <hpx/runtime/threads/executors/pool_executor.hpp>
#include <hpx/traits/executor_traits.hpp>
#include <hpx/include/parallel_execution.hpp>
#include <hpx/hpx_init.hpp>

#include <iostream>

std::vector<std::string> pools;

using executor = hpx::threads::executors::pool_executor;

int hpx_main(int argc, char* argv[])
{
    std::vector<executor> executors;

    for(auto pool: pools)
        executors.emplace_back(pool);

    std::size_t num_futures = 1'000'000;

    std::vector<hpx::future<std::size_t>> futures;
    futures.reserve(num_futures);

    for (std::size_t i = 0; i != num_futures; ++i)
    {
        futures.push_back(
            hpx::parallel::execution::async_execute(
                executors[i % executors.size()],
                [](std::size_t i)
                {
                    return i;
                }, i));
    }

    hpx::wait_all(futures);

    std::size_t sum = 0;
    for(auto& fut: futures)
    {
        sum += fut.get();
    }

    std::cout << "Summing up to " << sum << "\n";

    return hpx::finalize();
}

int main(int argc, char **argv)
{
    hpx::resource::partitioner rp(argc, argv);
    rp.set_default_pool_name("pool-0");

    for (auto& d: rp.numa_domains())
    {
        std::string pool_name = "pool-" + std::to_string(d.id());
        pools.push_back(pool_name);
        rp.create_thread_pool(pool_name);
        for (auto& c : d.cores())
        {
            for (auto& p : c.pus())
            {
                rp.add_resource(p, pool_name);
            }
        }
    }

//     // One Executor per core:
//     for (auto& d: rp.numa_domains())
//     {
//         for (auto& c : d.cores())
//         {
//             for (auto& p : c.pus())
//             {
//                 std::string pool_name = "pool-" + std::to_string(p.id());
//                 pools.push_back(pool_name);
//                 rp.create_thread_pool(pool_name);
//                 rp.add_resource(p, pool_name);
//             }
//         }
//     }

    return hpx::init();
}
