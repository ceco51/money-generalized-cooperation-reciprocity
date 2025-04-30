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
file_path <- "C:\\Users\\Francesco\\Desktop\\money_c++\\Main\\Figure4"  # Replace with your actual directory path
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

######### Equilibrium Cooperation Rate and Dominant Strategy (full sweep) #########

Figure4 <- df_core_money %>% 
  filter(Step == 30000) %>% # Keep only the last iteration for each parameter combination
  pivot_wider(names_from = Strategy, values_from = SurvivorCount) %>%
  mutate(
    MostPrevalent = case_when( # Dominant strategy
      Cooperators > 0.5 ~ "Cooperators",
      Defectors > 0.5 ~ "Defectors",
      `Money-users` > 0.5 ~ "Money-users",
      TRUE ~ "none above 50%"
    ),
    # Extract the value of the most prevalent strategy
    MostPrevalentValue = case_when(
      MostPrevalent == "Cooperators" ~ Cooperators,
      MostPrevalent == "Defectors" ~ Defectors,
      MostPrevalent == "Money-users" ~ `Money-users`,
      TRUE ~ 0.5  # Default value for "none above 50%"
    )
  ) %>%
  group_by(BCRatio, Liquidity) %>% 
  mutate(
    Liquidity = fct_rev(as.factor(Liquidity)),
    BCRatio = as.factor(BCRatio)
  ) %>% 
  dplyr::summarize(
    MeanCooperation = mean(CooperationRate), 
    SDCooperation = sd(CooperationRate), 
    MostPrevalent = names(which.max(table(MostPrevalent))),
    # Calculate the mean value of the most prevalent strategy for each group
    MostPrevalentValue = mean(MostPrevalentValue, na.rm = TRUE)
  ) %>% 
  ggplot(aes(x = BCRatio, y = Liquidity)) + 
  # Use alpha to adjust the shade based on MostPrevalentValue
  geom_tile(aes(fill = MostPrevalent, alpha = MeanCooperation), 
            color = "black") +
  geom_text(aes(label = sprintf("%.2f", MeanCooperation)), 
            nudge_y = 0.3, size = 5, alpha = 0.9) +
  geom_text(aes(label = sprintf("(%.2f)", SDCooperation)),
                  #sprintf("±%.2f", SDCooperation)), 
            nudge_y = -0.1, size = 4, alpha = 0.8) +
  scale_fill_manual(
    values = c("Defectors" = "#ABA300",
               "Money-users" = "#00BE67",
               "Cooperators" = "#F5A0B0",
               "none above 50%" = "#EEEEEE")
  ) +
  # Control the alpha range to make sure all tiles remain visible
  scale_alpha_continuous(range = c(0.2, 0.8), guide = "none") +
  labs(
    x = "Benefit-Cost Ratio",
    y = "Liquidity",
    fill = "Dominant Strategy"
  ) +
  theme_minimal(base_size = 16) +
  theme(
    panel.grid = element_blank(),
    axis.text = element_text(size = 14),
    axis.title = element_text(size = 20),
    legend.text = element_text(size = 16),
    legend.title = element_text(size = 18)
  ) +
  coord_fixed(ratio = 1)

Figure4 # Show Figure 4
