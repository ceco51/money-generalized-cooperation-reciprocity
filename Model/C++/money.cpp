// compilation command: g++ -std=c++17 -pthread money.cpp -o money -O3
// run the compiled program: ./money
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <memory>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>

// Forward declarations
class Agent;
class Simulation;

// Parameter combination struct to run multiple simulations
struct SimulationParameters {
    double benefitToCostRatio;
    bool evolutionaryUpdating;
    int initialCooperators;
    int initialDefectors;
    int initialDirects;
    int initialIndirects;
    int initialMoneys;
    int initialReputations;
    double initialLiquidity;
    int steps;
    
    // Constructor with default values
    SimulationParameters(
        double bcr = 2.0, 
        bool evolve = true,
        int coop = 20,
        int defect = 20,
        int direct = 20,
        int indirect = 20,
        int money = 20,
        int rep = 5,
        double liq = 0.5,
        int s = 1000
    ) : benefitToCostRatio(bcr),
        evolutionaryUpdating(evolve),
        initialCooperators(coop),
        initialDefectors(defect),
        initialDirects(direct),
        initialIndirects(indirect),
        initialMoneys(money),
        initialReputations(rep),
        initialLiquidity(liq),
        steps(s) {}
        
    // Generate parameter description for filename
    std::string getDescription() const {
        std::ostringstream oss;
        oss << "BC" << benefitToCostRatio 
            << "_L" << initialLiquidity;
        return oss.str();
    }
};

// Random number generation utilities
class Random {
private:
    static thread_local std::mt19937 generator;

public:
    static void initialize() {
        std::random_device rd;
        generator = std::mt19937(rd());
    }

    static double uniform() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(generator);
    }

    static int uniformInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator);
    }

    template<typename T>
    static T weightedChoice(const std::vector<T>& items, const std::vector<double>& weights) {
        if (items.empty() || items.size() != weights.size()) {
            throw std::runtime_error("Items and weights must have the same non-zero size");
        }

        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        int index = dist(generator);
        return items[index];
    }
};

thread_local std::mt19937 Random::generator;

// Agent strategy types
enum class Strategy {
    Cooperator,
    Defector,
    Direct,
    Indirect,
    Money
};

std::string strategyToString(Strategy strategy) {
    switch (strategy) {
        case Strategy::Cooperator: return "Cooperator";
        case Strategy::Defector: return "Defector";
        case Strategy::Direct: return "Direct";
        case Strategy::Indirect: return "Indirect";
        case Strategy::Money: return "Money";
        default: return "Unknown";
    }
}

// Agent class
class Agent {
private:
    int id;
    Strategy strategy;
    double fitness;
    std::shared_ptr<Agent> currentPartner;
    std::vector<int> memory;
    int score;
    int balance;
    bool standing;

public:
    Agent(int id, Strategy strategy, int initialScore, double initialLiquidity) 
        : id(id), strategy(strategy), fitness(0), currentPartner(nullptr), 
          score(initialScore), standing(true) {
        
        // Assign initial money based on liquidity parameter
        if (initialLiquidity < 1.0) {
            balance = (Random::uniform() < initialLiquidity) ? 1 : 0;
        } else {
            balance = static_cast<int>(initialLiquidity);
        }
    }
    
    // Some getters
    int getId() const { return id; }
    Strategy getStrategy() const { return strategy; }
    double getFitness() const { return fitness; }
    std::shared_ptr<Agent> getCurrentPartner() const { return currentPartner; }
    int getScore() const { return score; }
    bool getStanding() const { return standing; }
    int getBalance() const { return balance; }
    
    // Some setters
    void setStrategy(Strategy newStrategy) { strategy = newStrategy; }
    void setFitness(double newFitness) { fitness = newFitness; }
    void setCurrentPartner(std::shared_ptr<Agent> partner) { currentPartner = partner; }
    void setBalance(int newBalance) { balance = newBalance; }

    void resetFitness() { fitness = 0; }
    
    bool isInMemory(int agentId) const {
        return std::find(memory.begin(), memory.end(), agentId) != memory.end();
    }

    void addToMemory(int agentId) {
        if (!isInMemory(agentId)) {
            memory.push_back(agentId);
        }
    }

    void removeFromMemory(int agentId) {
        memory.erase(std::remove(memory.begin(), memory.end(), agentId), memory.end());
    }

    void cooperate(double cost, double benefit);
    void defect();
};

// Structure to hold simulation results
struct SimulationResult {
    int step;
    double cooperationRate;
    int cooperators;
    int defectors;
    int directReciprocators;
    int indirectReciprocators;
    int moneyUsers;
    double bcRatio;
    double liquidity;
};

// Simulation class
class Simulation {
private:
    // Simulation parameters
    double benefit;
    double cost;
    double benefitToCostRatio;
    int cooperationsThisTurn;
    double cooperationRate;
    bool evolutionaryUpdating;
    int initialReputations;
    double initialLiquidity;
    
    // Agent populations
    int initialCooperators;
    int initialDefectors;
    int initialDirects;
    int initialIndirects;
    int initialMoneys;
    
    // Collection of agents
    std::vector<std::shared_ptr<Agent>> agents;
    
    // Data collection
    std::vector<SimulationResult> results;
    int runId;
    
    // Important methods
    void matchAgents();
    void executeActions();
    void evolve();
    void updateStatistics();
    
public:
    // Constructor with parameter object and run ID
    Simulation(const SimulationParameters& params, int runId);
    
    void setup();
    void step();
    void run(int steps);
    void collectData(int currentStep);
    
    double getCooperationRate() const { return cooperationRate; }
    const std::vector<SimulationResult>& getResults() const { return results; }
    int getRunId() const { return runId; }
    
    std::unordered_map<Strategy, int> countStrategies() const;
    
    std::vector<std::shared_ptr<Agent>>& getAgents() { return agents; }
};

// Implementation of Agent's action methods
void Agent::cooperate(double cost, double benefit) {
    // The cooperator (acting agent) pays a fitness cost
    fitness -= cost;
    
    // Partner receives a benefit
    if (currentPartner) {
        currentPartner->setFitness(currentPartner->getFitness() + benefit);
    }
    
    // Reputation of the helping agent increases (choose either standing or image scoring)
    //score += 1;
    standing = true;
}

void Agent::defect() {
    // Reputation of the defecting agent decreases (uncomment for image scoring, comment out standing)
    //score -= 1;

    // Add this defecting agent to partner's memory
    if (currentPartner) {
        currentPartner->addToMemory(id);
    }
    if (currentPartner && currentPartner->getStanding()){ //if true
        standing = false; 
    }
}

// Implementation of Simulation methods
Simulation::Simulation(const SimulationParameters& params, int runId)
    : benefitToCostRatio(params.benefitToCostRatio),
      evolutionaryUpdating(params.evolutionaryUpdating),
      initialCooperators(params.initialCooperators),
      initialDefectors(params.initialDefectors),
      initialDirects(params.initialDirects),
      initialIndirects(params.initialIndirects),
      initialMoneys(params.initialMoneys),
      initialReputations(params.initialReputations),
      initialLiquidity(params.initialLiquidity),
      runId(runId),
      cost(1.0)
{
    benefit = cost * benefitToCostRatio;
}

void Simulation::setup() {
    // Initialize random number generator
    Random::initialize();
    
    // Clear any existing agents and results
    agents.clear();
    results.clear();
    
    // Create agents for each strategy
    int nextId = 0;
    
    // Create cooperators
    for (int i = 0; i < initialCooperators; i++) {
        agents.push_back(std::make_shared<Agent>(nextId++, Strategy::Cooperator, initialReputations, initialLiquidity));
    }
    
    // Create defectors
    for (int i = 0; i < initialDefectors; i++) {
        agents.push_back(std::make_shared<Agent>(nextId++, Strategy::Defector, initialReputations, initialLiquidity));
    }
    
    // Create direct reciprocators
    for (int i = 0; i < initialDirects; i++) {
        agents.push_back(std::make_shared<Agent>(nextId++, Strategy::Direct, initialReputations, initialLiquidity));
    }
    
    // Create indirect reciprocators
    for (int i = 0; i < initialIndirects; i++) {
        agents.push_back(std::make_shared<Agent>(nextId++, Strategy::Indirect, initialReputations, initialLiquidity));
    }
    
    // Create money users
    for (int i = 0; i < initialMoneys; i++) {
        agents.push_back(std::make_shared<Agent>(nextId++, Strategy::Money, initialReputations, initialLiquidity));
    }
    
    // Save initial state
    collectData(0);
}

void Simulation::matchAgents() {
    // Reset cooperation counter
    cooperationsThisTurn = 0;
    
    // Reset fitness of all agents and match with a new partner
    for (auto& agent : agents) { // every agent is asked to play the 'helping role' once
        agent->resetFitness();   // but an agent (due to stochasticity) could be the currentPartner of multiple others
        
        // Select random partner
        if (agents.size() > 1) {  // Need at least 2 agents for matching
            int partnerIndex;
            do {
                partnerIndex = Random::uniformInt(0, agents.size() - 1);
            } while (agents[partnerIndex]->getId() == agent->getId());
            
            agent->setCurrentPartner(agents[partnerIndex]);
        }
    }
}

void Simulation::executeActions() {
    for (auto& agent : agents) {
        auto currentPartner = agent->getCurrentPartner();
        if (!currentPartner) continue;  // Skip if no partner
        
        switch (agent->getStrategy()) {
            case Strategy::Cooperator:
                // Cooperators always cooperate
                agent->cooperate(cost, benefit);
                cooperationsThisTurn++;
                break;
                
            case Strategy::Defector:
                // Defectors always defect
                agent->defect();
                break;
                
            case Strategy::Direct:
                // Direct reciprocators cooperate unless the partner is in memory
                if (!agent->isInMemory(currentPartner->getId())) {
                    agent->cooperate(cost, benefit);
                    cooperationsThisTurn++;
                } else { // After defecting, currentPartner is released from memory
                    agent->defect();
                    agent->removeFromMemory(currentPartner->getId());
                }
                break;
                
            case Strategy::Indirect:
                // Indirect reciprocators cooperate if partner's reputation is positive (image score)
                //if (currentPartner->getScore() > 0) {
                    //agent->cooperate(cost, benefit);
                    //cooperationsThisTurn++;
                //} else {
                    //agent->defect();
                //}
                // Indirect reciprocators cooperate if partner's standing is good
                if (currentPartner->getStanding()) {
                    agent->cooperate(cost, benefit);
                    cooperationsThisTurn++;
                } else {
                    agent->defect();
                }
                break;
                
            case Strategy::Money:
                // Money users cooperate if partner has positive balance
                if (currentPartner->getBalance() > 0) {
                    agent->cooperate(cost, benefit);
                    cooperationsThisTurn++;
                    agent->setBalance(agent->getBalance() + 1);
                    currentPartner->setBalance(currentPartner->getBalance() - 1);
                } else {
                    agent->defect();
                }
                break;
        }
    }
}

void Simulation::evolve() {
    if (agents.size() <= 1) return;  // Need at least 2 agents to evolve
    
    // Select a random agent to potentially change strategy
    int agentIndex = Random::uniformInt(0, agents.size() - 1);
    auto targetAgent = agents[agentIndex];
    
    // Prepare for roulette wheel selection
    std::vector<Strategy> strategies;
    std::vector<double> weights;
    
    for (const auto& agent : agents) {
        // We keep correspondence between strategies and weights
        strategies.push_back(agent->getStrategy());
        weights.push_back(agent->getFitness());
    }
    
    // Select new strategy based on fitness-weighted probability
    Strategy newStrategy = Random::weightedChoice(strategies, weights);
    targetAgent->setStrategy(newStrategy);
}

void Simulation::updateStatistics() {
    // Calculate cooperation rate
    cooperationRate = agents.size() > 0 ? 
        static_cast<double>(cooperationsThisTurn) / agents.size() : 0.0;
    
    // Add base fitness to all agents to avoid negative probabilities
    for (auto& agent : agents) {
        agent->setFitness(agent->getFitness() + 1.0);
    }
}

void Simulation::step() {
    // Match agents with partners
    matchAgents();
    
    // Execute actions based on strategies
    executeActions();
    
    // Update statistics
    updateStatistics();
    
    // Perform evolutionary updating if enabled
    if (evolutionaryUpdating) {
        evolve();
    }
}

void Simulation::run(int steps) {
    for (int i = 1; i <= steps; i++) {  // Start from 1 since we already saved step 0 in setup
        step();

        // Invasion tests are temporarily commented out. Uncomment the desired test to run it.

        // Note: The code block only runs the invasion once at step 15,000. 
        // You can modify this condition if you want to trigger the 
        // tests at a different point in the simulation.
        
        // Uncomment ONLY one of the following test blocks to run it:
        
        // Test 1: Transform "Money" users into "Defectors"
        //if (i == 15000) {
            // First, count the current number of agents with the "Money" strategy
            //auto strategyCounts = countStrategies();
            //int moneyUsersToTransform = std::min(250, strategyCounts[Strategy::Money]);

            // Track how many agents have been transformed
            //int transformed = 0;

            // Loop over all agents and transform up to the specified number of "Money" users to "Defectors"
            //for (auto& agent : agents) {
                //if (agent->getStrategy() == Strategy::Money && transformed < moneyUsersToTransform) {
                    // Change strategy to Defector
                    //agent->setStrategy(Strategy::Defector);
                    // Increment the counter for transformed agents
                    //transformed++;
                //}
            //}
        //}
        // End of Test 1. Uncomment the block above to run Test 1.

        // Test 2: Transform "Money" users into "Cooperators"
        //if (i == 15000) {
            // First, count the current number of agents with the "Money" strategy
            //auto strategyCounts = countStrategies();
            //int moneyUsersToTransform = std::min(250, strategyCounts[Strategy::Money]);

            // Track how many agents have been transformed
            //int transformed = 0;

            // Loop over all agents and transform up to the specified number of "Money" users to "Cooperators"
            //for (auto& agent : agents) {
                //if (agent->getStrategy() == Strategy::Money && transformed < moneyUsersToTransform) {
                    // Change strategy to Cooperator
                    //agent->setStrategy(Strategy::Cooperator);
                    // Increment the counter for transformed agents
                    //transformed++;
                //}
            //}
        //}
        // End of Test 2. Uncomment the block above to run Test 2.

        // Test 3: Transform "Money" users into a mix of "Defectors" and "Cooperators"
        //if (i == 15000) {
            // First, count the current number of agents with the "Money" strategy
            //auto strategyCounts = countStrategies();
            //int moneyUsersToTransform = std::min(250, strategyCounts[Strategy::Money]);

            // Track how many agents have been transformed
            //int transformed = 0;

            // Loop over all agents and randomly choose whether to transform "Money" users to 
            // either "Defectors" or "Cooperators"
            //for (auto& agent : agents) {
                //if (agent->getStrategy() == Strategy::Money && transformed < moneyUsersToTransform) {
                    // Randomly choose between Defector or Cooperator
                    //Strategy newStrategy = (Random::uniform() < 0.5) ? 
                    //Strategy::Defector : Strategy::Cooperator;
                    // Change the agent's strategy
                    //agent->setStrategy(newStrategy);
                    // Increment the counter for transformed agents
                    //transformed++;
                //}
            //}
        //}
        // End of Test 3. Uncomment the block above to run Test 3.

        // End of invasion tests. Only one of the tests should be uncommented at a time.

        // Collect data every 250 steps or at the end
        if (i % 250 == 0 || i == steps) {
            collectData(i);
        }
    }
}

std::unordered_map<Strategy, int> Simulation::countStrategies() const {
    std::unordered_map<Strategy, int> counts;
    
    // Initialize counts
    counts[Strategy::Cooperator] = 0;
    counts[Strategy::Defector] = 0;
    counts[Strategy::Direct] = 0;
    counts[Strategy::Indirect] = 0;
    counts[Strategy::Money] = 0;
    
    // Count each strategy
    for (const auto& agent : agents) {
        counts[agent->getStrategy()]++;
    }
    
    return counts;
}

void Simulation::collectData(int currentStep) {
    auto strategyCounts = countStrategies();
    
    // Store the results
    SimulationResult result;
    result.step = currentStep;
    result.cooperationRate = cooperationRate;
    result.cooperators = strategyCounts[Strategy::Cooperator];
    result.defectors = strategyCounts[Strategy::Defector];
    result.directReciprocators = strategyCounts[Strategy::Direct];
    result.indirectReciprocators = strategyCounts[Strategy::Indirect];
    result.moneyUsers = strategyCounts[Strategy::Money];
    result.bcRatio = benefitToCostRatio;
    result.liquidity = initialLiquidity;

    results.push_back(result);
}

// Mutex for thread-safe operations
std::mutex consoleMutex;
std::mutex fileMutex;

// Class to manage multiple simulation runs and aggregate their results
class SimulationManager {
private:
    SimulationParameters params;
    std::string outputFilename;
    std::map<int, std::vector<SimulationResult>> allResults; // Map from run ID to results
    
public:
    SimulationManager(const SimulationParameters& params, const std::string& prefix = "") 
        : params(params) {
        // Create output file name with parameters in the filename
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        
        if (!prefix.empty()) {
            oss << prefix << "_";
        }
        
        oss << "sim_" << params.getDescription() << "_" 
            << std::put_time(&tm, "%H%M%S") << ".csv";
        
        outputFilename = oss.str();
    }
    
    void addResults(int runId, const std::vector<SimulationResult>& results) {
        std::lock_guard<std::mutex> lock(fileMutex);
        allResults[runId] = results;
    }
    
    // Add getter methods for params and steps
    const SimulationParameters& getParams() const {
        return params;
    }
    
    int getSteps() const {
        return params.steps;
    }
    
    void saveAllResults() {
        std::lock_guard<std::mutex> lock(fileMutex);
        
        std::ofstream dataFile(outputFilename);
        if (!dataFile.is_open()) {
            std::cerr << "Error opening file: " << outputFilename << std::endl;
            return;
        }
        
        // Write header
        dataFile << "RunID,Step,CooperationRate,Cooperators,Defectors,DirectReciprocators,IndirectReciprocators,MoneyUsers,BCRatio,Liquidity\n";

        
        // Write all results
        for (const auto& pair : allResults) {
            int runId = pair.first;
            const auto& results = pair.second;
            
            for (const auto& result : results) {
                dataFile << runId << ","
                     << result.step << ","
                     << result.cooperationRate << ","
                     << result.cooperators << ","
                     << result.defectors << ","
                     << result.directReciprocators << ","
                     << result.indirectReciprocators << ","
                     << result.moneyUsers << ","
                     << result.bcRatio << ","
                     << result.liquidity        
                     << std::endl;
            }
        }
        
        dataFile.close();
        
        std::lock_guard<std::mutex> consoleLock(consoleMutex);
        std::cout << "Results saved to " << outputFilename << std::endl;
    }
    
    const std::string& getOutputFilename() const {
        return outputFilename;
    }
};

// Function to run a single simulation with given parameters
void runSingleSimulation(SimulationManager& manager, int runId) {
    Simulation sim(manager.getParams(), runId);
    sim.setup();
    sim.run(manager.getSteps());
    
    // Add results to the manager
    manager.addResults(runId, sim.getResults());
    
    // Print completion message
    {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "Completed run " << runId << "/" << 100 
                  << " for " << manager.getOutputFilename() << std::endl;
    }
}

// Function to run a parameter combination with multiple repetitions
void runParameterSet(const SimulationParameters& params, const std::string& prefix) {
    // Create a simulation manager for this parameter set
    SimulationManager manager(params, prefix);
    
    {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "Starting 100 runs for parameters: " << params.getDescription() << std::endl;
    }
    
    // Create threads for parallel execution
    std::vector<std::thread> threads;
    int parallelRepetitions = 100; // MODIFY THIS TO GENERATE MORE/LESS PARALLEL RUNS FOR EACH PARAMETER COMBINATION
    for (int i = 1; i <= parallelRepetitions; i++) {
        threads.push_back(std::thread(runSingleSimulation, std::ref(manager), i));
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Save all results to a single file
    manager.saveAllResults();
    
    {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "Completed all 100 runs for parameters: " << params.getDescription() << std::endl;
        std::cout << std::endl;
    }
}

// Main function to run multiple parameter combinations (CHANGE HERE PARAMETERS FOR THE EXPERIMENT)
int main(int argc, char* argv[]) {
    // Set the maximum number of concurrent threads to avoid system overload.
    // The higher the value, the more cores you will use.
    // By default, we use half of the available cores (hardware_concurrency()) to avoid overwhelming the system.
    const int MAX_CONCURRENT_PARAMS = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) / 2);
    
    // Vector to store all parameter combinations to test
    std::vector<SimulationParameters> parameterSets;
    
    // Define different parameter values to test
    std::vector<double> benefitToCostRatios = {1.25, 1.5, 2, 3, 4, 5, 10, 20, 50, 100};
    std::vector<double> initialLiquidityValues = {1, 10, 50, 75};
    
    // This creates a full factorial design testing all combinations of the parameters.
    // Each combination will be used to initialize a simulation with different parameter values.
    for (double bcr : benefitToCostRatios) {
        for (double liq : initialLiquidityValues) {
            parameterSets.push_back(SimulationParameters(
                bcr,        // Benefit-to-cost ratio (read from above)
                true,       // Evolutionary updating enabled (keep it 'true' by default)
                100,        // Initial number of Cooperators
                100,        // Initial number of Defectors
                100,        // Initial number of Direct Reciprocators
                100,        // Initial number of Indirect Reciprocators
                100,        // Initial number of Money Users
                1,          // Initial reputation score (same for all agents)
                liq,        // Initial Liquidity level (read from above)
                30000       // Number of steps in the simulation
            ));
        }
    }                  

    std::cout << "Will run " << parameterSets.size() << " parameter combinations, each with 100 parallel instances" << std::endl;
    std::cout << "Running up to " << MAX_CONCURRENT_PARAMS << " parameter sets at once." << std::endl;
    std::cout << std::endl;
    
    // Process parameter sets in batches to avoid too many threads
    for (size_t i = 0; i < parameterSets.size(); i += MAX_CONCURRENT_PARAMS) {
        std::vector<std::thread> paramThreads;
        
        // Create a thread for each parameter set in this batch
        for (size_t j = i; j < std::min(i + MAX_CONCURRENT_PARAMS, parameterSets.size()); j++) {
            std::ostringstream prefixStream;
            prefixStream << "param_" << (j + 1);
            paramThreads.push_back(std::thread(
                runParameterSet, 
                parameterSets[j], 
                prefixStream.str()
            ));
        }
        
        // Wait for all parameter sets in this batch to complete
        for (auto& thread : paramThreads) {
            thread.join();
        }
    }
    
    std::cout << "All simulations complete!" << std::endl;
    return 0;
}
