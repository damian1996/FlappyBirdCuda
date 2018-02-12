# FLAPPY BIRD NEURAL NETWORK WITH GENETIC ALGORITHM + GUI WITH ALLEGRO5 #

### Authors ###

Damian Stachura

### Execute ###

- **Install allegro5**
    sudo add-apt-repository ppa:allegro/5.2
    sudo apt-get update
    sudo apt-get install liballegro5-dev
- **Clone repository**
    git clone https://github.com/damian1996/FlappyGil.git
- **Run program**
    cd BotVersionCUDA
    ./run.sh
    **or**
    cd BotVersionCUDA
    make
    ./solution.x

### Neural Network ###

**Network**
I created neural network with three values in the input layer, six in the hidden
layer and one in the output layer.

**Input Layer**
- Distance of the nearest tree by x coordinate.
- Distance from the middle between trees
- Speed of the bird

**Output Layer**
If output > bias, then bird will move up. Otherwise, bird will down.

**Activation function**
I used [sigmoid function](https://en.wikipedia.org/wiki/Sigmoid_function)

**Fitness**
This is distance traveled by bird. Additionally, I subtracted distance from
the middle between trees, when bird hit the tree or the floor

**Mutation**
I chose three the best fitnesses and I was mutating some of the best units.
Additionally, two new units are totally random.

### CUDA ###

**Kernels**
- to **initialize curand**
- to **compute fitnesses**, I simulate on the CUDA five games on 1024 units.
  Each unity write to array fitness own fitness. Then I choose the best units
  and make crossover
- to **mutations**, I created several units in crossover and I random new weights
  for all 1024 units from these several special units (3 the best from previous
  round, 3 crossovers and 2 random units)

Additionally I created several kernels only for device, because I must move logic
from CPU to the CUDA.
