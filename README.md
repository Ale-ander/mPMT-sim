# Conda Environment:
Firstly, we create the environment: in the same directory where you downloaded the files

```shell
mamba env create -f environment.devenv.yml
```

Then, to **activate** the environment:

```shell
mamba activate mpmtsim
```

# Prepare the Simulation Code
When the _mpmtsim_ environment is active and in the same directory where you downloaded the files, type:
```shell
mkdir build
cd build
cmake ..
make
```

# Run the Simulation
After preparing the simulation code, you can launch the simulation by running it in the build directory.
```shell
./mpmt_sim macros/<macros.mac>
```

### Options
| Option      | Description |
| ----------- | ----------- |
| -i/--interactive      |  launch also vis.mac and start event visualizer |