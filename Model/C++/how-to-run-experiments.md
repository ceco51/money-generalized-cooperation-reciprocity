# How to Run Experiments with C++ Code

In the `int main(int argc, char* argv[])` function, you will find the following default parameter values:

```cpp
std::vector<double> benefitToCostRatios = {2, 3, 5, 10};
std::vector<double> initialLiquidityValues = {0.25, 1, 10, 50};
```

These values are used to generate **Figure 2** of the main manuscript, as well as the majority of the **sensitivity analyses** and 
**robustness checks** in the Supplementary Information (SI).

To run experiments with different parameter values, simply modify the numbers inside the curly brackets. For example:

```cpp
std::vector<double> initialLiquidityValues = {0.25, 0.75, 1, 5, 10, 15, 20, 35, 50};
```

This will allow the model to investigate a broader range of initial liquidity levels. The total number of parameter combinations will 
be the Cartesian product of the two vectors.

Further down in the code, you will find a nested loop that creates parameter sets for each combination:

```cpp
for (double bcr : benefitToCostRatios) {
    for (double liq : initialLiquidityValues) {
        parameterSets.push_back(SimulationParameters(
            bcr,        // Benefit-to-cost ratio
            true,       // Evolutionary updating (keep this 'true' by default)
            100,        // Initial number of Cooperators
            100,        // Initial number of Defectors
            100,        // Initial number of Direct Reciprocators
            100,        // Initial number of Indirect Reciprocators
            100,        // Initial number of Money Users
            1,          // Initial reputation score (same for all agents)
            liq,        // Initial liquidity level
            30000       // Number of simulation steps
        ));
    }
}
```

You can also modify other parameters in the `SimulationParameters` constructor—for example, the initial population composition or the number of simulation steps. However, our primary interest lies in exploring the **joint effects** of `benefitToCostRatios` and `initialLiquidityValues`. For this reason, we do not perform parameter sweeps over population compositions. Instead, we condition the analysis on a few selected initial population structures to isolate the effects of interest.

To use more threads and run multiple parameter combinations in parallel, modify the following line in the function:

```cpp
const int MAX_CONCURRENT_PARAMS = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) / 2);
```
- For example, on our machine with 12 cores, `std::thread::hardware_concurrency()` returns `12`, so dividing by `2` gives `6`. This means `std::thread::hardware_concurrency()/2` parameter combinations will run concurrently by default.
- If you want to use **all of your cores**, simply divide by `1`:

```cpp
const int MAX_CONCURRENT_PARAMS = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) / 1);
```

- If you prefer to use **fewer cores**, increase the denominator (e.g., dividing by `3` will run 4 combinations concurrently on a 12-core machine).
- This value is stored as an `int`, with a minimum value of `1` to ensure that at least one parameter combination is processed.

Finally, in the function `void runParameterSet(const SimulationParameters& params, const std::string& prefix)`, you will see the variable:

```cpp
int parallelRepetitions = 100;
```

By default, the code runs **100 parallel repetitions** for each parameter combination. To increase or decrease this, simply modify the number, for example, to run 200 repetitions:

```cpp
int parallelRepetitions = 200;
```
