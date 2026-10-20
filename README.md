# Dowload Files from GitLab

```shell
git clone https://baltig.infn.it/dambrosino/multipmt-simulation.git
```

# Conda Environment:
Firstly, we create the environment: in the same directory where you downloaded the files

```shell
mamba env create -f ./multipmt-simulation/environment.devenv.yml
```

if you use conda, type conda instead of mamba.

Then, to **activate** the environment:

```shell
mamba activate mpmtsim
```

# Prepare the Simulation Code
When the _mpmtsim_ environment is active and in the same directory where you downloaded the files, type:
```shell
mkdir build
cd build
cmake ../multipmt-simulation/
make
```

# Run the Simulation
After preparing the simulation code, you can launch the simulation by running it in the build directory.
```shell
./task macros/ui.mac
```
