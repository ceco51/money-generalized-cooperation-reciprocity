# NetLogo Code 

The model is also implemented in the `money.nlogo` file (compatible with NetLogo version 6.4.0). This version offers the **same core functionalities** as the C++ implementation, but with an **interactive graphical user interface (GUI)**. Users can directly tweak parameters of interest and observe **live updates of key system metrics** (see screenshot below).

The GUI makes it easy to **explore and understand the model dynamics**, making it ideal for demonstration and experimentation. However, it is **not optimized for large-scale batch experiments**, as it tends to run more slowly — especially when simulating many parameter combinations or long time horizons.

![image](https://github.com/user-attachments/assets/11897fcc-d090-4213-b905-172991bdccef)

---

### Running Experiments in NetLogo via BehaviorSpace

To run experiments in the NetLogo version of the model, use **BehaviorSpace**. The following setup reproduces the data used for **Figure 2** of the main manuscript.

**BehaviorSpace experiment name:** `MAIN - Figure2`

#### Parameter Settings
In the BehaviorSpace configuration, vary the parameters as follows:

```netlogo
["initial-liquidity" 0.25 1 10 50]
["benefit-to-cost-ratio" 2 3 5 10]
["initial-cooperators" 100]
["initial-defectors" 100]
["initial-directs" 100]
["initial-indirects" 100]
["initial-moneys" 100]
["evolutionary-updating?" true]
["visualization?" false] ; disables drawing for faster execution
["initial-reputations" 1]
```

#### Experiment Settings

- **Repetitions:** `100`  
  (Each combination of parameters will be run 100 times.)
- **Measure runs using these reporters (i.e., output variables):**

```netlogo
count cooperators
count defectors
count directs
count indirects
count moneys
cooperation-rate
```

- **Run metrics when:**  
  ```netlogo
  ticks mod 250 = 0
  ```

- **Setup and go commands:** leave as default.
- **Time limit (ticks):** `30000`

---

This setup allows for systematic exploration of how different combinations of liquidity and benefit-to-cost ratios affect the model’s dynamics. The use of `visualization? false` ensures faster runs by disabling the GUI rendering.
