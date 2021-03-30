# test_kalray
Transform a png file.

## Building png_transform

The png_transform programm is built with Makefile, using gcc compiler.
type make in the root folder to build project in build folder.

tyoe lake clean to clean build folder

## png_transform execution

A new argument has been added to the programm to apply the new transformation (using colors weight)

Usage (In the root folder): 
> ./build/png_transform <file_to_transform> <transformed_file> [<transform_type> <transf_arg1> ...] 

<file_to_transform> : (mandatory Arg.) png file to transform
<transformed_file> : (mandatory Arg.) path and name of the png transformed image
<transform_type> : (optional) type of the transformation to apply
					0 -> default transformation (0 set to red, blue value set to green). 
					1 -> colors weight transformation (3 more arguments required)
					
					> ./build/png_transform <file_to_transform> <transformed_file> 1 <green_weight> <blue_weight>
					
					If <transform_type> is not given, default transformation is applied
					
## Tests and Results 

test.sh script has been modified to generate 2 transformed images kalray_Rx1.2_Gx0.8_Bx0.8.png and manycore-img_Rx1.2_Gx0.8_Bx0.8.png
using weight transformation and parameter Red=1.2 Green=0.8 and Blue=0.8.
The script then compare the generated images with the first transformations given in goldens

The result of the RMSE comparaison for the 2 pictures is 0 for all RGB value.
=> The new transformation seems correct

## Extensibility & Reusability

An enum e_transform_type has been added to declare the new transformation. 
For each new transformation, a new transforme function must be added and called switch the 4th programm argument value

## Debug steps

I have not a full linux distribution and IDE to debug the programm.
As the programm is pretty short, I prefer to use a Ubuntu 20.04 LTS distribution for windows (console available only)
and edit code with Notepad++
For the debug, I have just used the printf and read the code (the bugs are easy to find but maybe I missed few other bugs)

I didn't check performance of the read_png_file and write_png_file function (elapsed time analysis should be done),
I only improve performance of the process_file function in joining together the 2 loops.






 

