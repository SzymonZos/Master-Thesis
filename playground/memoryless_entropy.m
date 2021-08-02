lena = imread('./img/tmp_2/lena_dwt_0.png');
img = lena;
[ent_1, counts_1] = entropy_1(img);
[ent_2, counts_2] = entropy_2(img);
% ent_3 = entropy_3(img(:));
