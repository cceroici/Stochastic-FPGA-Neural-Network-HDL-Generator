# Stochastic-FPGA-Neural-Network-HDL-Generator
High-level code for automatically generating a specified deep learning architecture using stochastic signal processing Verilog components

# Generating Verilog Code

To generate a Verilog project with a specified model architecture, run "main.cpp".

To customize the model architecture, within "main.cpp", modify the specified layers:

    AddInputLayer(sConfig, N_INPUTS, 1, layerN);
    AddConvolutionalLayer(sConfig, 6, 4 * 4, 2, layerN);
    AddFullyConnectedLayer(sConfig, 25, layerN);
    AddFullyConnectedLayer(sConfig, 8, layerN);
    N_HIDDENLYR = AddOutputLayer(sConfig, N_OUTPUTS, layerN);

# Synthesizing/Simulating Generated Verilog Code

The generated files have be tested with ModelSim simulation as well as Vivado simulation/synthesis.


# Supported Layer Types

This project support fully-connected/dense and convoluational 2D layers. In addition, these layers can be made recurrent by specifying a "sConfig.maxTimeSteps" > 1 in "main.cpp"

# Examples

For an example of the generated HDL project files, see [here](https://github.com/cceroici/Stochastic-Neural-Network.git)




