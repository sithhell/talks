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

    hpx::future<std::size_t> future
        = hpx::parallel::execution::async_execute(
            executors[0],
            []() -> std::size_t { return 0; });


    for (std::size_t i = 1; i != num_futures; ++i)
    {
        future = future.then(
            executors[i % executors.size()],
            [i](hpx::future<std::size_t> fut)
            {
                return fut.get() + i;
            });
    }

//     for (std::size_t i = 1; i != num_futures; ++i)
//     {
//         future = hpx::dataflow(
//             executors[i % executors.size()],
//             [i](hpx::future<std::size_t> fut)
//             {
//                 return fut.get() + i;
//             }, std::move(future));
//     }

    std::cout << "Summing up to " << future.get() << "\n";

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
