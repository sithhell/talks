
class: center, middle

# Massive Parallelism in C++

### Munich C++ User Group Meeting - 25.01.2018

Thomas Heller (thom.heller@gmail.com)

---

class: center, middle

# Why Parallelism?
### The free lunch is over

---

# Why Parallelism?
## It's everywhere!

.center.middletext.bold.large[Everywhere?]

---

# Why Parallelism?
## Architectural Processor Improvements

 * Single core performance hard to improve

    * Scaling the frequence unfeasible
    * Increasing complexity not viable

&rArr; Instead of making a single core faster, just add more cores

---

# Why Parallelism?
## It's everywhere!

 * Microcontroller

---

# Why Parallelism?
## It's everywhere!

 * .strike[Microcontroller]

--

 * Embedded Devices

--

    * Intelligent Sensors/Actors
    * Internet of Things
    * Smartphones
    * Gaming Devices

--

 * Personal Computer

--

    * Work Station
    * Laptop
    * Desktop

--

 * Servers

---

# Why Parallelism?
## Distributed Computing

 * Cloud

    - Size hard to tell
    - Google: about 2.5 million servers in total
    - Elastic Scalabilty

 * Supercomputers

    - Tightly couple applications
    - TOP500:
        - 1: Sunway TaihuLight, 10,469,600 cores
        - 2: Tianhe-2, 3,120,000 cores
        - 3: Piz Daint, 361,760 cores
        - 4: Gyoukou, 19,860,000 cores
        - 5: Titan, 560,640 cores
        - average number of cores: 138,009.28


---

# Parallelism vs. Concurrency
## What's the difference?

 * Waiter preparing a meal and drink
 * Tasks:
    - Accept order
    - Prepare drink
    - Prepare meal
    - Give out order

---

# Test embedding...

<iframe src="https://www.desmos.com/calculator/0n7duwlvdp" width="100%" height="80%" style="border: 1px solid #ccc" frameborder=0></iframe>

---
