#include<bits/stdc++.h>
using namespace std;


int main(){

    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the parameters for the exponential distribution
    double lambda = 0.01; // lambda parameter for the exponential distribution

    // Create a distribution object for the exponential distribution
    std::exponential_distribution<> exponentialDist(lambda);

    // Generate a random number from the exponential distribution
    double randomNumber = exponentialDist(gen);

    // Output the generated number
    std::cout << "Random number from exponential distribution: " << randomNumber << std::endl;

    return 0;
}