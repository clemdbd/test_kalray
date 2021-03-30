# Transform images
./build/png_transform imgs/kalray.png imgs_transformed/kalray_Rx1.2_Gx0.8_Bx0.8.png 1 1.2 0.8 0.8
./build/png_transform imgs/manycore-img.png imgs_transformed/manycore-img_Rx1.2_Gx0.8_Bx0.8.png 1 1.2 0.8 0.8

# Compared transformed images with expected result (in goldens)
compare -verbose -metric rmse imgs_transformed/kalray_Rx1.2_Gx0.8_Bx0.8.png goldens/kalray_Rx1.2_Gx0.8_Bx0.8.png kalray_diff_Rx1.2_Gx0.8_Bx0.8.gif
compare -verbose -metric rmse imgs_transformed/manycore-img_Rx1.2_Gx0.8_Bx0.8.png goldens/manycore-img_Rx1.2_Gx0.8_Bx0.8.png manycore-img_diff_Rx1.2_Gx0.8_Bx0.8.gif
