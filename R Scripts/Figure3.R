# Load the required libraries 
library(tidyverse)
library(dplyr)
library(purrr)
library(readr)

# A C++ simulation generates one CSV file for each unique combination 
# of parameters (BC, Liquidity) tested during an experiment. The output files follow a naming convention like:
# Example filename: "param_7_sim_BC3_L10_183418.csv"
# Each CSV file contains simulation results for a specific parameter set.

# An *experiment* refers to a batch run of simulations over various parameter combinations, 
# with all corresponding CSV files saved in a designated output directory (see C++ code).

# The code below lists all CSV files produced by the C++ simulation in the target folder.

# Define the directory where the simulation outputs are stored.
file_path <- "C:\\Users\\Francesco\\Desktop\\money_c++\\Main\\Figure3"  # Replace with your actual directory path
csv_files <- list.files(path = file_path, 
                        pattern = "*.csv", 
                        full.names = TRUE
                        )

# Read and combine all CSV files
all_data <- csv_files %>%
  map_df(~read_csv(.x, show_col_types = FALSE))  # This reads each file and row-binds them together

# Data from an experiment (i.e., a batch of BC, liquidity combinations) in a single dataframe
df_core_money <- 
  all_data %>%
  as_tibble()  %>% 
  mutate(
    BCRatio = as.factor(BCRatio),
    Liquidity = as.factor(Liquidity),
    Total = Cooperators + 
      Defectors + 
      DirectReciprocators + 
      IndirectReciprocators + 
      MoneyUsers
  ) %>% 
  mutate(
    ShareCooperators = Cooperators / Total,
    ShareDefectors = Defectors / Total,
    ShareDR = DirectReciprocators / Total,
    ShareIR = IndirectReciprocators / Total,
    ShareMoney = MoneyUsers / Total
  ) %>% 
  select(-(c("Cooperators", 
             "Defectors", 
             "DirectReciprocators", 
             "IndirectReciprocators", 
             "MoneyUsers"))) %>% 
  pivot_longer(
    cols = starts_with("Share"), 
    names_to = "Strategy", 
    values_to = "SurvivorCount") %>%
  mutate(Strategy =
           case_when(
             Strategy == "ShareCooperators" ~ "Cooperators",
             Strategy == "ShareDefectors" ~ "Defectors",
             Strategy == "ShareDR" ~ "Direct-reciprocators",
             Strategy == "ShareIR" ~ "Indirect-reciprocators",
             Strategy == "ShareMoney" ~ "Money-users",
           )
  )

######### Cooperation by liquidity levels in equilibrium (Figure 3) #########

# For simplicity, only when BCRatio = 2

Figure3 <- df_core_money %>% 
  filter(Step == 30000) %>% 
  filter(BCRatio == 2) %>% 
  ggplot(aes(x = factor(Liquidity), y = CooperationRate)) +
  # Add nicer boxplots with custom colors
  geom_boxplot(
    fill = "#eeeeee", 
    color = "#023047",
    alpha = 0.7,
    outlier.shape = NA  # Hide outliers
  ) +
  # Add the median connecting line with a smoother curve
  stat_summary(
    aes(x = as.numeric(factor(Liquidity)), group = 1),
    fun = "median",
    geom = "line",
    color = "#fb8500",
    size = 0.8,
    lineend = "round"
  ) +
  # Add points at the medians
  stat_summary(
    aes(x = as.numeric(factor(Liquidity))),
    fun = "median",
    geom = "point",
    color = "#fb8500",
    size = 3,
    shape = 16
  ) +
  # Improve the scale for the x-axis
  scale_x_discrete(
    name = "Liquidity"
  ) +
  # Add better labels
  labs(
    y = "Cooperation Rates"
  ) +
  # Use a nicer theme
  theme_minimal() +
  theme(
    text = element_text(family = "sans", color = "#333333"),
    plot.title = element_text(face = "bold", size = 16),
    plot.subtitle = element_text(size = 12, color = "#666666"),
    axis.title = element_text(size = 14, face = "bold"),
    axis.text = element_text(size = 12),
    panel.grid.minor = element_blank(),
    panel.grid.major = element_line(color = "#eeeeee"),
    panel.border = element_blank(),
    legend.position = "none"
  ) +
  # Ensure y-axis starts at 0 for better context
  coord_cartesian(ylim = c(0, 1))

Figure3 # Show Figure 3
