# CoInGP -- Convolutional Inpainting with Genetic Programming

GitHub repository for the source code used in the paper:

D. Jakobovic, L. Manzoni, L. Mariot, S. Picek, M. Castelli: CoInGP: Convolutional Inpainting with Genetic Programming (accepted at GECCO 2021, preprint available at https://arxiv.org/abs/2004.11300)

## Structure

The content of this repository is organized as follows:

- `images/`: test greyscale images used in the second experiment of the paper, namely boat.png and goldhill.png. Both images are already resized to 256x256 pixels, as used in the experiments of the paper.

- `src/`: C++ source code for the implementation of CoInGP used in the paper, integrated with some Python utility scripts to convert the test images in fitness cases and reconstruct them with the values predicted by the best evolved GP individual.

## Compiling under Linux and GCC 7.5.0

CoInGP has been developed using the [Evolutionary Computation Framework (ECF)](http://ecf.zemris.fer.hr/). Therefore, to compile CoInGP you first need a working [installation](http://ecf.zemris.fer.hr/install.html) of ECF on your system. Once this is done, you can compile all C++ source files by simply running

`$ make all`

from inside the `src/` directory. Beside three object files (`Evaluator.o`, `main.o` and `SymbRegEvalOp.o`), the compilation will produce the `coingp` executable.

## Running Example under Linux and Python 3.6.9

This section describes a short tutorial on how to use the CoInGP executable to reconstruct a single test image, for instance `images/goldhill.png` using the Von Neumann neighborhood.

### Converting an image to GP fitness cases
Before starting, we need to convert the image to a set of fitness cases readable by CoInGP. For the Von Neumann neighborhood, a fitness case is of the form `(pixel1 pixel2 pixel3 pixel4 value)`, where the first four coordinates represents the intensities of the 4 pixels on the frontier, while the fifth is the correct label of the central pixel (i.e. the value of the pixel in the original image that CoInGP must learn to predict). To this end, run the following script from inside the `src/` directory:

`$ python3 img_convert_von_neumann.py ../images/goldhill.png`

(Notice: this script might take a little time to terminate, and does not print anything on standard output). This command will produce 6 files in output:

- `greyscale.png`: the damaged image with random pixels removed following the procedure described in section 4.3 of the paper
- `points.txt`: text file containing the coordinates of the removed pixels (will be used later for reconstruction)
- `train.txt`: text file containing the fitness cases for the training set
- `test.txt`: text file containing the fitness cases for the test set
- `baseline.png`: the image reconstructed by the *baseline* predictor used for benchmarking purposes in the paper, that is, the predictor that computes the average of the pixels's intensities on the frontier
- `baselines.txt`: text file reporting the RMSE for the baseline predictor on the training and the test sets

### Training phase
Now we can run CoInGP for the training phase, using this command:

`$ ./coingp parameters.txt`

The file `parameters.txt` contains all experimental settings needed to run CoInGP on the required training set. For the sake of this tutorial, the number of generations has been set to 50, instead of 500 as in the paper, in order to perform the training phase more quickly. The population size is 500, mutation probability is 0.3, and the GP functionals set includes the operators `sin, cos, +, -, /, *, pos, min, max, avg, sqrt`. Tournament size is 3, and the maximum depth of a GP tree is set to 4 since we are using the Von Neumann neighborhood; thus, the input variables for a GP tree are the 4 pixels north, west, south and east of the central one. The entry key `input_file` refers to the file containing the fitness cases to be used by CoInGP. Since we are in the training phase, we use the file train.txt for now. `linear_scaling` refers to the value of the linear scaling operator described in Section 4.1 of the paper (default to 1), while `data_offset` and `data_skip` refer respectively to the line from where to start reading the input file and how many lines to skip for every read line (default respectively to 0 and 1, i.e. we read the whole input file). `batch_repeats` indicates how many independent runs of CoInGP we want to run on the same training set. While in the paper we performed 100 independent runs, here in this tutorial we perform only a single demonstrative run, hence it this parameter is set to 0 and no `stats.txt` file is produced.

The output of this command is entirely saved in the `log.txt` file. After the last GP generation elapses, the program will output the best GP tree in the population as an XML tag, such as:

`<Individual size="1" gen="46">
		<FitnessMin value="5.41849"/>
		<Tree size="29">avg avg max + x1 x3 + x1 x3 avg sqrt x1 / x1 x4 avg min + x4 x3 max x2 x2 max max x4 x4 sqrt x1 </Tree>
</Individual>`

Copy this tag in a separate file, e.g. `best.xml`.

### Testing phase
Now, we first need to modify `parameters.txt` by changing the `input_file` entry key to `test.txt`, since we want to apply the GP tree to predict the actual missing pixels in the image `greyscale.png`.

Optionally, we can also perform the test phase without reconstruction for this best GP individual by running:

`$ ./coingp parameters.txt best.xml`

This will only give in output the file `xval.txt` containing the RMSE achieved by the GP tree defined in `best.xml` on the test set.

To obtain also the predicted values for all missing pixels in the test set, we need to run CoInGP with an additional command line argument:

`$ ./coingp parameters.txt best.xml out.txt`

This will produce a further text file `out.txt` containing the intensities predicted by the GP tree in `best.xml` for all missing pixels in the test set.

### Reconstruction
Finally, we can reconstruct the damaged image by calling the `reconstruct.py` script as follows:

`$ python3 reconstruct.py ../images/goldhill.png`

This will produce in output two files:

- `corrected.png`: the reconstructed image with the missing pixels filled by the values predicted by the GP tree in `best.xml`
- `difference.png`: the image showing the pixel-by-pixel difference between the reconstructed image `corrected.png` and the original image. The differences are increased tenfold to make them visible.

