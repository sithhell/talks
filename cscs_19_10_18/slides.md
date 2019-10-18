class: center, middle

# The road to adopt HPX in large code bases
## &nbsp;
## Cracking HPX open

### 18.10.2019

Thomas Heller (thom.heller@gmail.com)

Slides: [https://sithhell.github.io/talks/cscs_19_10_18/](.)

---

# What is HPX?

* A C++ Standard Library for Concurrency and Parallelism

* Solidly based on a theoretical foundation – a well defined, new execution model
* Exposes a coherent and uniform, standards-oriented API for ease of
  programming parallel and distributed applications.
    * Enables to write fully asynchronous, blocking free code
    * Provides unified syntax and semantics for local and remote operations.
* Developed to run at any scale
* Compliant C++ Standard implementation (and more)
* Open Source: Published under the Boost Software License

---

# What is HPX not (currently)?

* Easy to use in existing projects

* Lightweight

* Fully modular

---

# Roadblocks for adoption

* `hpx::init`

* "All or nothing" mentality

---

# Main contributions of HPX

* `hpx::future`, `hpx::dataflow`, `hpx::when_all`

* *LOTS* of high quality C++ utilities

* Distributed computing with modern C++ (serialization, low overhead RPCs)

--

    &rArr; Decouple The HPX asynchronous programming model from the runtime
--

    &rArr; Create Building blocks for heterogeneous Execution of tasks

---
class: center, middle

# Basic Definitions
## &nbsp;
## Customizing the HPX programming model

Based on [P0443](https://wg21.link/p0443)

---

# Execution Resource

* "Places" in a system that might execute tasks, or where tasksa need to run in
  proximity

    * CPU Cores

    * GPUs

    * Memory Hierarchies

    * Remote nodes

---

# Execution Context

* An `Execution Context` uses one or more `Execution Resource`

* Specifies how tasks are being created (`Execution Agent`)

* Examples:

    * OpenMP

    * `std::thread`

    * `hpx::thread`

    * Stackless tasks

    * GPU offloading

    * Remote offloading

---

# Execution Agent

* An `Execution Agent` is created by the `Execution Context`

* Runs on the specified `Execution Resource`

* Executes the task

* May be suspend, resume or yield the thread of execution

---

# Adapting HPX Concepts

* Using Execution Contexts as the main API to spawn tasks

    &rArr; Provides customization points for other runtimes

    &rArr; `hpx::async` (and friends) now potentially work in any environment

* Using Execution Agents to build synchronization primitives

    &rArr; Basic building blocks using agents to yield/suspend/resume execution

    &rArr; `hpx::future` (and friends) now potentially work in any environment

---

# Using `hpx::future` with OpenMP

```
namespace hpx { namespace openmp {
    struct context : hpx::basic_execution::default_context
    {
        void post(hpx::util::unique_function_nonser<void()> f) const override;

        hpx::basic_execution::default_executor executor() const
        {
            return hpx::basic_execution::default_executor(*this);
        }
    };

    struct agent : hpx::basic_execution::default_agent
    {
        agent(context const& ctx) : hpx::basic_execution::default_agent(ctx)
        {}

        hpx::openmp::context const& context() const override
        {
            return context_;
        }
    };

    void context::post(hpx::util::unique_function_nonser<void()> f) const
    {
#pragma omp task firstprivate(f)
        {
            f();
        }
    }
}}
```
---

# Using `hpx::future` with OpenMP

```
int main()
{
    hpx::future<int> f;
#pragma omp parallel
    {
        hpx::openmp::context ctx;

#pragma omp single
        f = hpx::async(ctx.executor(), []()
        {
            std::cout << "Hello" << " World\n";
            return 42;
        });
    }

    std::cout << "Result: " << f.get();
}
```

---

# Using `hpx::future` with MPI

(Credit to John Biddiscombe)

```
struct mpi_future_data : hpx::lcos::detail::task_base<void>
{
    HPX_NON_COPYABLE(mpi_future_data);

    using init_no_addref = hpx::lcos::detail::task_base<void>::init_no_addref;

    mpi_future_data() = default;

    mpi_future_data(init_no_addref no_addref)
      : hpx::lcos::detail::task_base<void>(no_addref)
    {}

    void do_run()
    {
        boost::intrusive_ptr<mpi_future_data> this_(this);
        // FIXME: Do a proper progress thread
        std::thread progress([this_]()
        {
            MPI_Status status;
            MPI_Wait(&this_->request, &status);
            this_->set_data(hpx::util::unused);
        });
        progress.detach();
    }

    MPI_Request request;
};
```

---

# Using `hpx::future` with MPI

(Credit to John Biddiscombe)

```
template <typename F, typename ...Ts>
hpx::future<void> invoke(F f, Ts &&...ts)
{
    boost::intrusive_ptr<mpi_future_data> data =
        new mpi_future_data(mpi_future_data::init_no_addref{});
    f(std::forward<Ts>(ts)..., &data->request);

    using traits::future_access;
    return future_access<hpx::future<void>>::create(std::move(data));
}
```

---

# Using `hpx::future` with MPI

(Credit to John Biddiscombe)

```
int rank = 0; int size = 0; int result = 0;

MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

int dst_rank = rank == size - 1 ? 0 : rank + 1;
int src_rank = rank == 0 ? size - 1 : rank - 1;

if (rank == 0) {
    MPI_Send(&result, 1, MPI_INT, dst_rank, 0, MPI_COMM_WORLD);
}

auto f = hpx::mpi::invoke(
    MPI_Irecv, &result, 1, MPI_INT, src_rank, 0, MPI_COMM_WORLD);

f.then([&](auto f) mutable {
    f.get(); // propagate exceptions...
    if (rank == 0) {
        std::cout << "Result: " << result << "\n";
        return hpx::make_ready_future();
    }

    result += rank;

    return hpx::mpi::invoke(
        MPI_Isend, &result, 1, MPI_INT, dst_rank, 0, MPI_COMM_WORLD);
}).get();
```


---

# HPX modularization

* Splitting HPX into self contained smaller libraries

* Make the different parts usable independently

.left-column[
* algorithms
* assertion
* cache
* collectives
* config
* coroutines
* datastructures
* errors
* execution
* format
]
.right-column[
* functional
* iterator_support
* logging
* plugin
* preprocessor
* program_options
* serialization
* testing
* timing
* topology
]

* Mainly driven by Mikael Simberg and Auriane Reverdell at CSCS

---

# Outlook

* Very much WIP

* Proof of concept: https://github.com/STEllAR-GROUP/hpx/tree/execution_context_post

* Major roadblocks eliminated?

---

class: center, middle

# Questions?
